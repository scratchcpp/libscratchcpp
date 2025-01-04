// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/Passes/PassBuilder.h>

#include <scratchcpp/target.h>
#include <iostream>

#include "llvmcompilercontext.h"

using namespace libscratchcpp;

LLVMCompilerContext::LLVMCompilerContext(IEngine *engine, Target *target) :
    CompilerContext(engine, target),
    m_llvmCtx(std::make_unique<llvm::LLVMContext>()),
    m_module(std::make_unique<llvm::Module>(target ? target->name() : "", *m_llvmCtx)),
    m_llvmCtxPtr(m_llvmCtx.get()),
    m_modulePtr(m_module.get()),
    m_jit((initTarget(), llvm::orc::LLJITBuilder().create()))
{
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

    m_jitInitialized = true;
    assert(m_llvmCtx);
    assert(m_module);

#ifdef PRINT_LLVM_IR
    std::cout << std::endl << "=== LLVM IR (" << m_module->getName().str() << ") ===" << std::endl;
    m_module->print(llvm::outs(), nullptr);
    std::cout << "==============" << std::endl << std::endl;
#endif

    // Optimize
    const auto &functions = m_module->getFunctionList();

    llvm::PassBuilder passBuilder;
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cGSCCAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cGSCCAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cGSCCAnalysisManager, moduleAnalysisManager);

    // Use O0 for the entire module
    llvm::ModulePassManager modulePassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O0);

    // Create a FunctionPassManager for O3 optimizations
    llvm::FunctionPassManager functionPassManager = passBuilder.buildFunctionSimplificationPipeline(llvm::OptimizationLevel::O3, llvm::ThinOrFullLTOPhase::None);

    // Run the O0 pipeline for the module
    modulePassManager.run(*m_module, moduleAnalysisManager);

    // Run the O3 pipeline for specific functions
    for (llvm::Function &func : m_module->functions()) {
        if (!func.isDeclaration()) {
            if (!func.hasFnAttribute(llvm::Attribute::OptimizeNone)) {
#ifndef NDEBUG
                std::cout << "debug: optimizing function: " << functionNames.back() << std::endl;
#endif
                functionPassManager.run(func, functionAnalysisManager);
            } else if (func.hasFnAttribute(llvm::Attribute::OptimizeNone)) {
                // Remove this attribute to avoid JIT hangs
                // TODO: Optimize all functions, maybe it doesn't take so long
                func.removeFnAttr(llvm::Attribute::OptimizeNone);
            }
        }
    }

    // Init JIT compiler
    std::string name = m_module->getName().str();
    auto err = m_jit->get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(m_module), std::move(m_llvmCtx)));

    if (err) {
        llvm::errs() << "error: failed to add module '" << name << "' to JIT: " << toString(std::move(err)) << "\n";
        return;
    }
}

bool LLVMCompilerContext::jitInitialized() const
{
    return m_jitInitialized;
}

void LLVMCompilerContext::initTarget()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
}
