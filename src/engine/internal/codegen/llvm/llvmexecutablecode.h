// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executablecode.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/valuedata.h>
#include <llvm/IR/LLVMContext.h>

#include "llvmcompilercontext.h"
#include "test_export.h"

namespace libscratchcpp
{

class LLVMExecutionContext;

class LIBSCRATCHCPP_TEST_EXPORT LLVMExecutableCode : public ExecutableCode
{
    public:
        LLVMExecutableCode(
            LLVMCompilerContext *ctx,
            function_id_t functionId,
            const std::string &mainFunctionName,
            const std::string &resumeFunctionName,
            size_t stringCount,
            Compiler::CodeType codeType);

        void run(ExecutionContext *context) override;
        ValueData runReporter(ExecutionContext *context) override;
        bool runPredicate(ExecutionContext *context) override;
        void kill(libscratchcpp::ExecutionContext *context) override;
        void reset(ExecutionContext *context) override;

        bool isFinished(ExecutionContext *context) const override;

        std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const override;

        function_id_t functionId() const;
        size_t stringCount() const;

    private:
        using MainFunctionType = void *(*)(ExecutionContext *, Target *, ValueData **, List **);
        using ReporterFunctionType = ValueData (*)(ExecutionContext *, Target *, ValueData **, List **);
        using PredicateFunctionType = bool (*)(ExecutionContext *, Target *, ValueData **, List **);
        using ResumeFunctionType = bool (*)(void *);

        static LLVMExecutionContext *getContext(ExecutionContext *context);

        LLVMCompilerContext *m_ctx = nullptr;
        function_id_t m_functionId = 0;
        std::string m_mainFunctionName;
        std::string m_predicateFunctionName;
        std::string m_resumeFunctionName;
        size_t m_stringCount = 0;
        Compiler::CodeType m_codeType;

        mutable std::variant<MainFunctionType, ReporterFunctionType, PredicateFunctionType> m_mainFunction;
        mutable ResumeFunctionType m_resumeFunction = nullptr;
};

} // namespace libscratchcpp
