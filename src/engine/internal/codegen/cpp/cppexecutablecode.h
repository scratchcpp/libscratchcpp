// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executablecode.h>
#include <scratchcpp/global.h>
#include <vector>

#include "cppcoroutine.h"

namespace libscratchcpp
{

class Target;
class List;
class CppCompilerContext;
class CppExecutionContext;

/*!
 * Base class of generated C++ code.
 * Generated classes override runScript(), runReporter(), runPredicate() and runProcedure().
 */
class LIBSCRATCHCPP_EXPORT CppExecutableCode : public ExecutableCode
{
    public:
        CppExecutableCode(CppCompilerContext *ctx, const std::vector<void *> &pointers = {});

        virtual void run(ExecutionContext *context) override final;
        virtual ValueData runReporter(ExecutionContext *context) override final;
        virtual bool runPredicate(ExecutionContext *context) override final;
        virtual void kill(ExecutionContext *context) override final;
        virtual void reset(ExecutionContext *context) override final;

        virtual bool isFinished(ExecutionContext *context) const override final;

        virtual std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const override final;

        virtual CppCoroutine runProcedure(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, bool warp, ValueData *args);

    protected:
        virtual CppCoroutine runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);
        virtual ValueData runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);
        virtual bool runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists);

        CppCompilerContext *compilerContext() const;
        const std::vector<void *> &pointers() const;

    private:
        static CppExecutionContext *getContext(ExecutionContext *context);

        CppCompilerContext *m_ctx = nullptr;
        std::vector<void *> m_pointers;
};

} // namespace libscratchcpp
