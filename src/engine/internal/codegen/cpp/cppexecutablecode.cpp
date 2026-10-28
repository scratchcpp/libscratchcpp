// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/valuedata.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/target.h>
#include <cassert>

#include "cppexecutablecode.h"
#include "cppcompilercontext.h"
#include "cppexecutioncontext.h"

using namespace libscratchcpp;

CppExecutableCode::CppExecutableCode(CppCompilerContext *ctx) :
    m_ctx(ctx)
{
    assert(m_ctx);
}

void CppExecutableCode::run(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);

    if (ctx->finished())
        return;

    auto promise = ctx->promise();

    if (promise) {
        if (promise->isResolved())
            ctx->setPromise(nullptr);
        else
            return;
    }

    // TODO: Add support for coroutines
    // TODO: Add support for end thread sentinel value
    Target *target = ctx->thread()->target();
    runScript(context, target, target->variableData(), target->listData());
    ctx->setFinished(true);
}

ValueData CppExecutableCode::runReporter(ExecutionContext *context)
{
    Target *target = context->thread()->target();
    return runReporter(context, target, target->variableData(), target->listData());
}

bool CppExecutableCode::runPredicate(ExecutionContext *context)
{
    Target *target = context->thread()->target();
    return runPredicate(context, target, target->variableData(), target->listData());
}

void CppExecutableCode::kill(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);
    // TODO: Add support for coroutines
    // NOTE: Do not destroy coroutine, it will be destroyed later
    // (really bad things happen when a coroutine destroys itself...)
    // The code is finished and the coroutine won't be used again until reset() is called
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void CppExecutableCode::reset(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);

    // TODO: Add support for coroutines
    ctx->setFinished(false);
    ctx->setPromise(nullptr);
}

bool CppExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->finished();
}

std::shared_ptr<ExecutionContext> CppExecutableCode::createExecutionContext(Thread *thread) const
{
    return std::make_shared<CppExecutionContext>(m_ctx, thread);
}

void CppExecutableCode::runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
}

ValueData CppExecutableCode::runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
    return ValueData();
}

bool CppExecutableCode::runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
    return false;
}

CppExecutionContext *CppExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<CppExecutionContext *>(context));
    return static_cast<CppExecutionContext *>(context);
}
