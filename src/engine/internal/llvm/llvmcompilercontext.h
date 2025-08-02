// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>

#include <scratchcpp/compilercontext.h>

namespace libscratchcpp
{

class ExecutionContext;
class ValueData;
class List;
class LLVMExecutableCode;

class LLVMCompilerContext : public CompilerContext
{
    public:
        LLVMCompilerContext(IEngine *engine, Target *target);

        void preoptimize() override;

        llvm::LLVMContext *llvmCtx();
        llvm::Module *module();

        void initJit();
        bool jitInitialized() const;

        llvm::Function *coroutineResumeFunction() const;
        void destroyCoroutine(void *handle);

        template<typename T>
        T lookupFunction(const std::string &name)
        {
            auto func = m_jit->get()->lookup(name);

            if (func)
                return (T)func->getValue();
            else {
                llvm::errs() << "error: failed to lookup LLVM function: " << toString(func.takeError()) << "\n";
                return nullptr;
            }
        }

    private:
        using ResumeCoroFuncType = bool (*)(void *);
        using DestroyCoroFuncType = void (*)(void *);

        void initTarget();
        llvm::Function *createCoroResumeFunction();
        llvm::Function *createCoroDestroyFunction();

        void verifyFunction(llvm::Function *function);

        std::unique_ptr<llvm::LLVMContext> m_llvmCtx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::LLVMContext *m_llvmCtxPtr = nullptr;
        llvm::Module *m_modulePtr = nullptr;
        llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> m_jit;
        bool m_jitInitialized = false;

        llvm::Function *m_llvmCoroResumeFunction = nullptr;

        llvm::Function *m_llvmCoroDestroyFunction = nullptr;
        DestroyCoroFuncType m_coroDestroyFunction = nullptr;
};

} // namespace libscratchcpp
