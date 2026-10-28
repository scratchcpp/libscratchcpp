// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executablecode.h>

#include "test_export.h"

namespace libscratchcpp
{

class Target;
class List;
class CppCompilerContext;
class CppExecutionContext;

class LIBSCRATCHCPP_TEST_EXPORT CppExecutableCode : public ExecutableCode
{
    public:
        CppExecutableCode(CppCompilerContext *ctx);

        virtual void run(ExecutionContext *context) override final;
        virtual ValueData runReporter(ExecutionContext *context) override final;
        virtual bool runPredicate(ExecutionContext *context) override final;
        virtual void kill(ExecutionContext *context) override final;
        virtual void reset(ExecutionContext *context) override final;

        virtual bool isFinished(ExecutionContext *context) const override final;

        virtual std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const override final;

    protected:
        virtual void runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);
        virtual ValueData runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);
        virtual bool runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);

    private:
        static CppExecutionContext *getContext(ExecutionContext *context);

        CppCompilerContext *m_ctx = nullptr;
};

} // namespace libscratchcpp
