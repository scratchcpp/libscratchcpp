// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executablecode.h>

namespace libscratchcpp
{

class CppCompilerContext;

class CppExecutableCode : public ExecutableCode
{
    public:
        CppExecutableCode(CppCompilerContext *ctx);

        void run(ExecutionContext *context) override;
        ValueData runReporter(ExecutionContext *context) override;
        bool runPredicate(ExecutionContext *context) override;
        void kill(ExecutionContext *context) override;
        void reset(ExecutionContext *context) override;

        bool isFinished(ExecutionContext *context) const override;

        std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const override;

    private:
        CppCompilerContext *m_ctx = nullptr;
};

} // namespace libscratchcpp
