// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/OptimizationLevel.h>

#include <scratchcpp/compilercontext.h>

namespace libscratchcpp
{

class ExecutionContext;
class ValueData;
class List;
class LLVMExecutableCode;

using function_id_t = unsigned int;

class LLVMCompilerContext : public CompilerContext
{
    public:
        LLVMCompilerContext(IEngine *engine, Target *target);

        void preoptimize() override;

        llvm::LLVMContext *llvmCtx();
        llvm::Module *module();

        function_id_t getNextFunctionId();

        void initJit();
        bool jitInitialized() const;

        llvm::Function *coroutineResumeFunction() const;
        void destroyCoroutine(void *handle);

        llvm::StructType *valueDataType() const;
        llvm::StructType *stringPtrType() const;

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
        void createTargetMachine();
        void optimize(llvm::OptimizationLevel optLevel);

        llvm::Function *createCoroResumeFunction();
        llvm::Function *createCoroDestroyFunction();

        void verifyFunction(llvm::Function *function);

        std::unique_ptr<llvm::LLVMContext> m_llvmCtx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::LLVMContext *m_llvmCtxPtr = nullptr;
        llvm::Module *m_modulePtr = nullptr;
        std::unique_ptr<llvm::TargetMachine> m_targetMachine;
        llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> m_jit;
        bool m_jitInitialized = false;

        function_id_t m_nextFunctionId = 0;

        llvm::Function *m_llvmCoroResumeFunction = nullptr;

        llvm::Function *m_llvmCoroDestroyFunction = nullptr;
        DestroyCoroFuncType m_coroDestroyFunction = nullptr;

        llvm::StructType *m_valueDataType = nullptr;
        llvm::StructType *m_stringPtrType = nullptr;
};

} // namespace libscratchcpp
