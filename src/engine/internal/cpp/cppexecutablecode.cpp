// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/target.h>
#include <scratchcpp/promise.h>
#include <iostream>
#include <cassert>

#include "cppexecutablecode.h"
#include "cppexecutioncontext.h"
#include "cppcompilercontext.h"
#include "cppruntime.h"
#include "cppprelude.h"

using namespace libscratchcpp;

CppExecutableCode::CppExecutableCode(CppCompilerContext *ctx, const std::string &functionName, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_functionName(functionName),
    m_codeType(codeType)
{
    assert(m_ctx);
}

void CppExecutableCode::run(ExecutionContext *context)
{
    assert(m_codeType == Compiler::CodeType::Script);
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

    if (ctx->frame()) {
        bool done = cpp::resume(ctx->frame());

        if (done)
            ctx->setFrame(nullptr);

        ctx->setFinished(done);
    } else {
        resolve();
        Target *target = ctx->thread()->target();
        MainFunctionType f = reinterpret_cast<MainFunctionType>(m_function);
        void *handle = f(context, target, target->variableData(), target->listData());

        if (!handle || handle == cpp::END_THREAD_SENTINEL) {
            ctx->setFinished(true);
            ctx->setFrame(nullptr);
        } else
            ctx->setFrame(handle);
    }
}

ValueData CppExecutableCode::runReporter(ExecutionContext *context)
{
    assert(m_codeType == Compiler::CodeType::Reporter);
    resolve();
    Target *target = context->thread()->target();
    ReporterFunctionType f = reinterpret_cast<ReporterFunctionType>(m_function);
    return f(context, target, target->variableData(), target->listData());
}

bool CppExecutableCode::runPredicate(ExecutionContext *context)
{
    assert(m_codeType == Compiler::CodeType::HatPredicate);
    resolve();
    Target *target = context->thread()->target();
    PredicateFunctionType f = reinterpret_cast<PredicateFunctionType>(m_function);
    return f(context, target, target->variableData(), target->listData());
}

void CppExecutableCode::kill(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void CppExecutableCode::reset(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);

    if (ctx->frame()) {
        cpp::destroy(ctx->frame());
        ctx->setFrame(nullptr);
    }

    ctx->setFinished(false);
    ctx->setPromise(nullptr);
}

bool CppExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->finished();
}

std::shared_ptr<ExecutionContext> CppExecutableCode::createExecutionContext(Thread *thread) const
{
    resolve();
    return std::make_shared<CppExecutionContext>(thread);
}

/*! Returns the name of the generated function. */
const std::string &CppExecutableCode::functionName() const
{
    return m_functionName;
}

Compiler::CodeType CppExecutableCode::codeType() const
{
    return m_codeType;
}

/*! Returns true if the compiled function has been found. */
bool CppExecutableCode::isResolved() const
{
    if (!m_function)
        m_function = cppLookupFunction(m_ctx->moduleKey(), m_functionName);

    return m_function;
}

void CppExecutableCode::resolve() const
{
    if (!isResolved()) {
        std::cerr << "error: C++ function " << m_functionName << " (module " << std::hex << m_ctx->moduleKey() << std::dec << ") is not compiled into the application" << std::endl;
        std::abort();
    }
}

CppExecutionContext *CppExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<CppExecutionContext *>(context));
    return static_cast<CppExecutionContext *>(context);
}
