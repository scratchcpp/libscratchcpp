// SPDX-License-Identifier: Apache-2.0

#include <llvm/IR/Module.h>

#include "llvmcoroutine.h"

using namespace libscratchcpp;

LLVMCoroutine::LLVMCoroutine(llvm::Module *module, llvm::IRBuilder<> *builder, llvm::Function *func) :
    m_module(module),
    m_builder(builder),
    m_function(func)
{
    llvm::LLVMContext &ctx = builder->getContext();

    // Set presplitcoroutine attribute
    func->setPresplitCoroutine();

    // Coroutine intrinsics
    llvm::Function *coroId = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::coro_id);
    llvm::Function *coroSize = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::coro_size, builder->getInt64Ty());
    llvm::Function *coroBegin = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::coro_begin);
    llvm::Function *coroEnd = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::coro_end);
    llvm::Function *coroFree = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::coro_free);

    // Init coroutine
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    llvm::Constant *nullPointer = llvm::ConstantPointerNull::get(pointerType);
    llvm::Value *coroIdRet = builder->CreateCall(coroId, { builder->getInt32(8), nullPointer, nullPointer, nullPointer });

    // Allocate memory
    llvm::Value *coroSizeRet = builder->CreateCall(coroSize, std::nullopt, "size");
    llvm::FunctionCallee mallocFunc = module->getOrInsertFunction("malloc", llvm::FunctionType::get(pointerType, { builder->getInt64Ty() }, false));
    llvm::Value *alloc = builder->CreateCall(mallocFunc, coroSizeRet, "mem");

    // Begin
    m_handle = builder->CreateCall(coroBegin, { coroIdRet, alloc });
    m_didSuspendVar = builder->CreateAlloca(builder->getInt1Ty(), nullptr, "didSuspend");
    builder->CreateStore(builder->getInt1(false), m_didSuspendVar);
    llvm::BasicBlock *entry = builder->GetInsertBlock();

    // Create suspend branch
    m_suspendBlock = llvm::BasicBlock::Create(ctx, "suspend", func);
    builder->SetInsertPoint(m_suspendBlock);
    builder->CreateCall(coroEnd, { m_handle, builder->getInt1(false), llvm::ConstantTokenNone::get(ctx) });
    builder->CreateRet(m_handle);

    // Create free branches
    m_freeMemRetBlock = llvm::BasicBlock::Create(ctx, "freeMemRet", func);
    builder->SetInsertPoint(m_freeMemRetBlock);
    builder->CreateFree(alloc);
    builder->CreateRet(llvm::ConstantPointerNull::get(pointerType));

    llvm::BasicBlock *freeBranch = llvm::BasicBlock::Create(ctx, "free", func);
    builder->SetInsertPoint(freeBranch);
    builder->CreateFree(alloc);
    builder->CreateBr(m_suspendBlock);

    // Create cleanup branch
    m_cleanupBlock = llvm::BasicBlock::Create(ctx, "cleanup", func);
    builder->SetInsertPoint(m_cleanupBlock);
    llvm::Value *mem = builder->CreateCall(coroFree, { coroIdRet, m_handle });
    llvm::Value *needFree = builder->CreateIsNotNull(mem);
    builder->CreateCondBr(needFree, freeBranch, m_suspendBlock);

    builder->SetInsertPoint(entry);
}

llvm::Value *LLVMCoroutine::handle() const
{
    return m_handle;
}

llvm::BasicBlock *LLVMCoroutine::suspendBlock() const
{
    return m_suspendBlock;
}

llvm::BasicBlock *LLVMCoroutine::cleanupBlock() const
{
    return m_cleanupBlock;
}

llvm::BasicBlock *LLVMCoroutine::freeMemRetBlock() const
{
    return m_freeMemRetBlock;
}

llvm::Value *LLVMCoroutine::didSuspendVar() const
{
    return m_didSuspendVar;
}

void LLVMCoroutine::createSuspend()
{
    llvm::LLVMContext &ctx = m_builder->getContext();

    m_builder->CreateStore(m_builder->getInt1(true), m_didSuspendVar);
    llvm::BasicBlock *resumeBranch = llvm::BasicBlock::Create(ctx, "", m_function);
    llvm::Value *noneToken = llvm::ConstantTokenNone::get(ctx);
    llvm::Value *suspendResult = m_builder->CreateCall(llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::coro_suspend), { noneToken, m_builder->getInt1(false) });
    llvm::SwitchInst *sw = m_builder->CreateSwitch(suspendResult, m_suspendBlock, 2);
    sw->addCase(m_builder->getInt8(0), resumeBranch);
    sw->addCase(m_builder->getInt8(1), m_cleanupBlock);
    m_builder->SetInsertPoint(resumeBranch);
}

llvm::Value *LLVMCoroutine::createResume(llvm::Value *coroHandle)
{
    m_builder->CreateCall(llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::coro_resume), { coroHandle });
    return m_builder->CreateCall(llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::coro_done), { coroHandle });
}

void LLVMCoroutine::end()
{
    llvm::LLVMContext &ctx = m_builder->getContext();

    // Add final suspend point
    llvm::BasicBlock *endBranch = llvm::BasicBlock::Create(ctx, "end", m_function);
    llvm::BasicBlock *finalSuspendBranch = llvm::BasicBlock::Create(ctx, "finalSuspend", m_function);
    m_builder->CreateCondBr(m_builder->CreateLoad(m_builder->getInt1Ty(), m_didSuspendVar), finalSuspendBranch, endBranch);

    m_builder->SetInsertPoint(finalSuspendBranch);
    llvm::Value *suspendResult = m_builder->CreateCall(llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::coro_suspend), { llvm::ConstantTokenNone::get(ctx), m_builder->getInt1(true) });
    llvm::SwitchInst *sw = m_builder->CreateSwitch(suspendResult, m_suspendBlock, 2);
    sw->addCase(m_builder->getInt8(0), endBranch); // unreachable
    sw->addCase(m_builder->getInt8(1), m_cleanupBlock);

    // Jump to "free and return" branch
    m_builder->SetInsertPoint(endBranch);
    m_builder->CreateBr(m_freeMemRetBlock);
}
