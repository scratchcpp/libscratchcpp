// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/executablecode.h>
#include <scratchcpp/valuedata.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>

namespace libscratchcpp
{

class Target;
class List;
class LLVMExecutionContext;

class LLVMExecutableCode : public ExecutableCode
{
    public:
        LLVMExecutableCode(std::unique_ptr<llvm::Module> module);

        void run(ExecutionContext *context) override;
        void kill(libscratchcpp::ExecutionContext *context) override;
        void reset(ExecutionContext *context) override;

        bool isFinished(ExecutionContext *context) const override;

        std::shared_ptr<ExecutionContext> createExecutionContext(Target *target) const override;

    private:
        uint64_t lookupFunction(const std::string &name);

        using MainFunctionType = void *(*)(ExecutionContext *, Target *, ValueData **, List **);
        using ResumeFunctionType = bool (*)(void *);

        static LLVMExecutionContext *getContext(ExecutionContext *context);

        std::unique_ptr<llvm::LLVMContext> m_ctx;
        llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> m_jit;

        MainFunctionType m_mainFunction;
        ResumeFunctionType m_resumeFunction;
};

} // namespace libscratchcpp
