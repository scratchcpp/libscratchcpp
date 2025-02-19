// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/executioncontext.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <iostream>

#include <llvm/Support/Error.h>

#include "llvmexecutablecode.h"
#include "llvmcompilercontext.h"
#include "llvmexecutioncontext.h"

using namespace libscratchcpp;

LLVMExecutableCode::LLVMExecutableCode(LLVMCompilerContext *ctx, const std::string &mainFunctionName, const std::string &resumeFunctionName, bool isPredicate) :
    m_ctx(ctx),
    m_mainFunctionName(mainFunctionName),
    m_resumeFunctionName(resumeFunctionName),
    m_isPredicate(isPredicate)
{
    assert(m_ctx);

    if (m_ctx->jitInitialized()) {
        std::cerr << "error: cannot create LLVM code after JIT compiler had been initialized" << std::endl;
        assert(false);
    }
}

void LLVMExecutableCode::run(ExecutionContext *context)
{
    assert(std::holds_alternative<MainFunctionType>(m_mainFunction));
    assert(m_resumeFunction);
    LLVMExecutionContext *ctx = getContext(context);

    if (ctx->finished())
        return;

    auto promise = ctx->promise();

    if (promise) {
        if (promise->isResolved())
            ctx->setPromise(nullptr);
        else
            return;
    }

    if (ctx->coroutineHandle()) {
        bool done = m_resumeFunction(ctx->coroutineHandle());

        if (done)
            ctx->setCoroutineHandle(nullptr);

        ctx->setFinished(done);
    } else {
        Target *target = ctx->thread()->target();
        MainFunctionType f = std::get<MainFunctionType>(m_mainFunction);
        void *handle = f(context, target, target->variableData(), target->listData());

        if (!handle)
            ctx->setFinished(true);

        ctx->setCoroutineHandle(handle);
    }
}

bool LLVMExecutableCode::runPredicate(ExecutionContext *context)
{
    assert(std::holds_alternative<PredicateFunctionType>(m_mainFunction));
    Target *target = context->thread()->target();
    PredicateFunctionType f = std::get<PredicateFunctionType>(m_mainFunction);
    return f(context, target, target->variableData(), target->listData());
}

void LLVMExecutableCode::kill(ExecutionContext *context)
{
    LLVMExecutionContext *ctx = getContext(context);
    ctx->setCoroutineHandle(nullptr);
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void LLVMExecutableCode::reset(ExecutionContext *context)
{
    LLVMExecutionContext *ctx = getContext(context);
    ctx->setCoroutineHandle(nullptr);
    ctx->setFinished(false);
    ctx->setPromise(nullptr);
}

bool LLVMExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->finished();
}

std::shared_ptr<ExecutionContext> LLVMExecutableCode::createExecutionContext(Thread *thread) const
{
    if (!m_ctx->jitInitialized())
        m_ctx->initJit();

    if (m_isPredicate)
        m_mainFunction = m_ctx->lookupFunction<PredicateFunctionType>(m_mainFunctionName);
    else
        m_mainFunction = m_ctx->lookupFunction<MainFunctionType>(m_mainFunctionName);

    m_resumeFunction = m_ctx->lookupFunction<ResumeFunctionType>(m_resumeFunctionName);
    return std::make_shared<LLVMExecutionContext>(thread);
}

LLVMExecutionContext *LLVMExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<LLVMExecutionContext *>(context));
    return static_cast<LLVMExecutionContext *>(context);
}
