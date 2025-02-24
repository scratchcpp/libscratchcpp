// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/IRBuilder.h>

namespace libscratchcpp
{

class LLVMCoroutine
{
    public:
        LLVMCoroutine(llvm::Module *module, llvm::IRBuilder<> *builder, llvm::Function *func);
        LLVMCoroutine(const LLVMCoroutine &) = delete;

        llvm::Value *handle() const;

        llvm::BasicBlock *suspendBlock() const;
        llvm::BasicBlock *cleanupBlock() const;
        llvm::BasicBlock *freeMemRetBlock() const;

        llvm::Value *didSuspendVar() const;

        void createSuspend();
        llvm::Value *createResume(llvm::Function *function, llvm::Value *coroHandle);
        void end();

    private:
        llvm::Module *m_module = nullptr;
        llvm::IRBuilder<> *m_builder = nullptr;
        llvm::Function *m_function = nullptr;
        llvm::Value *m_handle = nullptr;
        llvm::BasicBlock *m_suspendBlock = nullptr;
        llvm::BasicBlock *m_cleanupBlock = nullptr;
        llvm::BasicBlock *m_freeMemRetBlock = nullptr;
        llvm::Value *m_didSuspendVar = nullptr;
};

} // namespace libscratchcpp
