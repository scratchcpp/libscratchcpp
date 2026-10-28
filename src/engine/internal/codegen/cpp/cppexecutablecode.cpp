// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/valuedata.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/target.h>
#include <cassert>

#include "cppexecutablecode.h"
#include "cppcompilercontext.h"
#include "cppexecutioncontext.h"

using namespace libscratchcpp;

CppExecutableCode::CppExecutableCode(CppCompilerContext *ctx, const std::vector<void *> &pointers) :
    m_ctx(ctx),
    m_pointers(pointers)
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

    CppCoroutine &coroutine = ctx->coroutine();

    if (!coroutine.done()) {
        if (coroutine.resume()) {
            ctx->setCoroutine(CppCoroutine());
            ctx->setFinished(true);
        }
    } else {
        Target *target = ctx->thread()->target();
        CppCoroutine ret = runScript(context, target, target->variableData(), target->listData());

        // NOTE: The end thread flag only stops the code (procedures propagate it)
        if (ret.done())
            ctx->setFinished(true);
        else
            ctx->setCoroutine(std::move(ret));
    }
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
    // NOTE: Do not destroy coroutine, it will be destroyed later
    // (really bad things happen when a coroutine destroys itself...)
    // The code is finished and the coroutine won't be used again until reset() is called
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void CppExecutableCode::reset(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);
    ctx->setCoroutine(CppCoroutine());
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

CppCoroutine CppExecutableCode::runProcedure(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, bool warp, ValueData *args)
{
    // Generated code overrides this
    return CppCoroutine(false);
}

CppCoroutine CppExecutableCode::runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
    return CppCoroutine(false);
}

ValueData CppExecutableCode::runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
    ValueData ret;
    value_init(&ret);
    return ret;
}

bool CppExecutableCode::runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists)
{
    // Generated code overrides this
    return false;
}

CppCompilerContext *CppExecutableCode::compilerContext() const
{
    return m_ctx;
}

const std::vector<void *> &CppExecutableCode::pointers() const
{
    return m_pointers;
}

CppExecutionContext *CppExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<CppExecutionContext *>(context));
    return static_cast<CppExecutionContext *>(context);
}
