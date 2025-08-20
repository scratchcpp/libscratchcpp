// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/Verifier.h>

#include <scratchcpp/target.h>
#include <iostream>

#include "llvmcompilercontext.h"
#include "llvmcoroutine.h"
#include "llvmtypes.h"

using namespace libscratchcpp;

LLVMCompilerContext::LLVMCompilerContext(IEngine *engine, Target *target) :
    CompilerContext(engine, target),
    m_llvmCtx(std::make_unique<llvm::LLVMContext>()),
    m_module(std::make_unique<llvm::Module>(target ? target->name() : "", *m_llvmCtx)),
    m_llvmCtxPtr(m_llvmCtx.get()),
    m_modulePtr(m_module.get()),
    m_jit((initTarget(), llvm::orc::LLJITBuilder().create()))
{
    // Create functions
    m_llvmCoroResumeFunction = createCoroResumeFunction();
    m_llvmCoroDestroyFunction = createCoroDestroyFunction();

    // Create types
    m_valueDataType = LLVMTypes::createValueDataType(*m_llvmCtx);
    m_stringPtrType = LLVMTypes::createStringPtrType(*m_llvmCtx);

    if (!m_jit) {
        llvm::errs() << "error: failed to create JIT: " << toString(m_jit.takeError()) << "\n";
        return;
    }
}

void LLVMCompilerContext::preoptimize()
{
    initJit();
}

llvm::LLVMContext *LLVMCompilerContext::llvmCtx()
{
    return m_llvmCtxPtr;
}

llvm::Module *LLVMCompilerContext::module()
{
    return m_modulePtr;
}

void LLVMCompilerContext::initJit()
{
    if (m_jitInitialized) {
        std::cout << "warning: JIT compiler is already initialized" << std::endl;
        return;
    }

    assert(m_llvmCoroDestroyFunction);
    const std::string coroDestroyFuncName = m_llvmCoroDestroyFunction->getName().str();
    m_jitInitialized = true;
    assert(m_llvmCtx);
    assert(m_module);

#ifdef PRINT_LLVM_IR
    std::cout << std::endl << "=== LLVM IR (" << m_module->getName().str() << ") ===" << std::endl;
    m_module->print(llvm::outs(), nullptr);
    std::cout << "==============" << std::endl << std::endl;
#endif

    // Optimize
    optimize(llvm::OptimizationLevel::O3);

    const auto &functions = m_module->getFunctionList();
    std::vector<std::string> lookupNames;

    for (const llvm::Function &func : functions) {
        if (func.hasExternalLinkage() && !func.isDeclaration())
            lookupNames.push_back(func.getName().str());
    }

    // Init JIT compiler
    std::string name = m_module->getName().str();
    auto err = m_jit->get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(m_module), std::move(m_llvmCtx)));

    if (err) {
        llvm::errs() << "error: failed to add module '" << name << "' to JIT: " << toString(std::move(err)) << "\n";
        return;
    }

    // Lookup functions to JIT-compile ahead of time
    for (const std::string &name : lookupNames) {
#ifndef NDEBUG
        std::cout << "debug: looking up function: " << name << std::endl;
#endif
        lookupFunction<void *>(name);
    }

    // Lookup coro_destroy()
    m_coroDestroyFunction = lookupFunction<DestroyCoroFuncType>(coroDestroyFuncName);
    assert(m_coroDestroyFunction);
}

bool LLVMCompilerContext::jitInitialized() const
{
    return m_jitInitialized;
}

llvm::Function *LLVMCompilerContext::coroutineResumeFunction() const
{
    return m_llvmCoroResumeFunction;
}

void LLVMCompilerContext::destroyCoroutine(void *handle)
{
    if (!m_jitInitialized) {
        std::cout << "error: JIT must be initialized to destroy coroutines" << std::endl;
        assert(false);
    }

    assert(m_coroDestroyFunction);
    m_coroDestroyFunction(handle);
}

llvm::StructType *LLVMCompilerContext::valueDataType() const
{
    return m_valueDataType;
}

llvm::StructType *LLVMCompilerContext::stringPtrType() const
{
    return m_stringPtrType;
}

void LLVMCompilerContext::initTarget()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    createTargetMachine();

    m_module->setDataLayout(m_targetMachine->createDataLayout());
}

void LLVMCompilerContext::createTargetMachine()
{
    std::string error;
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    m_module->setTargetTriple(targetTriple);

    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        llvm::errs() << error;
        return;
    }

    llvm::TargetOptions opt;
    const char *cpu = "generic";
    const char *features = "";

    m_targetMachine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_));
}

void LLVMCompilerContext::optimize(llvm::OptimizationLevel optLevel)
{
    llvm::PassBuilder passBuilder(m_targetMachine.get());
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cGSCCAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cGSCCAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cGSCCAnalysisManager, moduleAnalysisManager);

    llvm::ModulePassManager modulePassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

    std::string pipeline;

    if (optLevel == llvm::OptimizationLevel::O0)
        pipeline = "default<O0>";
    else if (optLevel == llvm::OptimizationLevel::O1)
        pipeline = "default<O1>";
    else if (optLevel == llvm::OptimizationLevel::O2)
        pipeline = "default<O2>";
    else if (optLevel == llvm::OptimizationLevel::O3)
        pipeline = "default<O3>";
    else if (optLevel == llvm::OptimizationLevel::Os)
        pipeline = "default<Os>";
    else if (optLevel == llvm::OptimizationLevel::Oz)
        pipeline = "default<Oz>";
    else
        assert(false);

    if (passBuilder.parsePassPipeline(modulePassManager, pipeline)) {
        llvm::errs() << "Failed to parse pipeline\n";
        return;
    }

    modulePassManager.run(*m_module, moduleAnalysisManager);
}

llvm::Function *LLVMCompilerContext::createCoroResumeFunction()
{
    llvm::IRBuilder<> builder(*m_llvmCtx);

    // bool coro_resume(void *handle)
    llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getInt1Ty(), builder.getVoidTy()->getPointerTo(), false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "coro_resume", m_module.get());
    func->setComdat(m_module->getOrInsertComdat(func->getName()));
    func->setDSOLocal(true);
    func->addFnAttr(llvm::Attribute::NoInline);
    func->addFnAttr(llvm::Attribute::OptimizeNone);

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*m_llvmCtx, "entry", func);
    builder.SetInsertPoint(entry);
    builder.CreateRet(LLVMCoroutine::createResume(m_module.get(), &builder, func, func->getArg(0)));

    verifyFunction(func);
    return func;
}

llvm::Function *LLVMCompilerContext::createCoroDestroyFunction()
{
    llvm::IRBuilder<> builder(*m_llvmCtx);

    // void coro_destroy(void *handle)
    llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getVoidTy(), builder.getVoidTy()->getPointerTo(), false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "coro_destroy", m_module.get());
    func->setComdat(m_module->getOrInsertComdat(func->getName()));
    func->setDSOLocal(true);
    func->addFnAttr(llvm::Attribute::NoInline);
    func->addFnAttr(llvm::Attribute::OptimizeNone);

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*m_llvmCtx, "entry", func);
    builder.SetInsertPoint(entry);
    builder.CreateCall(llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_destroy), { func->getArg(0) });
    builder.CreateRetVoid();

    verifyFunction(func);
    return func;
}

void LLVMCompilerContext::verifyFunction(llvm::Function *function)
{
    if (llvm::verifyFunction(*function, &llvm::errs())) {
        llvm::errs() << "error: " << function->getName() << "function verficiation failed!\n";
        llvm::errs() << "module name: " << m_module->getName() << "\n";
    }
}
