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

LLVMExecutableCode::LLVMExecutableCode(LLVMCompilerContext *ctx, const std::string &mainFunctionName, const std::string &resumeFunctionName, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_mainFunctionName(mainFunctionName),
    m_resumeFunctionName(resumeFunctionName),
    m_codeType(codeType)
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

ValueData LLVMExecutableCode::runReporter(ExecutionContext *context)
{
    assert(std::holds_alternative<ReporterFunctionType>(m_mainFunction));
    Target *target = context->thread()->target();
    ReporterFunctionType f = std::get<ReporterFunctionType>(m_mainFunction);
    return f(context, target, target->variableData(), target->listData());
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
    // NOTE: Do not destroy coroutine, it will be destroyed later
    // (really bad things happen when a coroutine destroys itself...)
    // The code is finished and the coroutine won't be used again until reset() is called
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void LLVMExecutableCode::reset(ExecutionContext *context)
{
    LLVMExecutionContext *ctx = getContext(context);

    if (ctx->coroutineHandle()) {
        // TODO: Can a script reset itself? (really bad things happen when a coroutine destroys itself...)
        m_ctx->destroyCoroutine(ctx->coroutineHandle());
        ctx->setCoroutineHandle(nullptr);
    }

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

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            m_mainFunction = m_ctx->lookupFunction<MainFunctionType>(m_mainFunctionName);
            break;

        case Compiler::CodeType::Reporter:
            m_mainFunction = m_ctx->lookupFunction<ReporterFunctionType>(m_mainFunctionName);
            break;

        case Compiler::CodeType::HatPredicate:
            m_mainFunction = m_ctx->lookupFunction<PredicateFunctionType>(m_mainFunctionName);
            break;
    }

    m_resumeFunction = m_ctx->lookupFunction<ResumeFunctionType>(m_resumeFunctionName);
    return std::make_shared<LLVMExecutionContext>(m_ctx, thread);
}

LLVMExecutionContext *LLVMExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<LLVMExecutionContext *>(context));
    return static_cast<LLVMExecutionContext *>(context);
}
