// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/executablecode.h>
#include <scratchcpp/valuedata.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>

namespace libscratchcpp
{

class Target;
class LLVMExecutionContext;

class LLVMExecutableCode : public ExecutableCode
{
    public:
        LLVMExecutableCode(std::unique_ptr<llvm::Module> module, std::vector<std::unique_ptr<libscratchcpp::ValueData>> &constValues);
        ~LLVMExecutableCode();

        void run(ExecutionContext *context) override;
        void kill(libscratchcpp::ExecutionContext *context) override;
        void reset(ExecutionContext *context) override;

        bool isFinished(ExecutionContext *context) const override;

        void promise() override;
        void resolvePromise() override;

        std::shared_ptr<ExecutionContext> createExecutionContext(Target *target) const override;

    private:
        using FunctionType = size_t (*)(Target *);

        static LLVMExecutionContext *getContext(ExecutionContext *context);

        std::unique_ptr<llvm::LLVMContext> m_ctx;
        llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> m_jit;

        std::vector<FunctionType> m_functions;
        std::vector<std::unique_ptr<ValueData>> m_constValues;
};

} // namespace libscratchcpp
