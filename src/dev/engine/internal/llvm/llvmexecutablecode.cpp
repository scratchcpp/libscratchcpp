// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/promise.h>
#include <scratchcpp/thread.h>
#include <llvm/Support/Error.h>
#include <iostream>

#include "llvmexecutablecode.h"
#include "llvmexecutioncontext.h"

using namespace libscratchcpp;

LLVMExecutableCode::LLVMExecutableCode(std::unique_ptr<llvm::Module> module) :
    m_ctx(std::make_unique<llvm::LLVMContext>()),
    m_jit(llvm::orc::LLJITBuilder().create())
{
    if (!m_jit) {
        llvm::errs() << "error: failed to create JIT: " << toString(m_jit.takeError()) << "\n";
        return;
    }

    if (!module)
        return;

    std::string name = module->getName().str();
    auto err = m_jit->get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(m_ctx)));

    if (err) {
        llvm::errs() << "error: failed to add module '" << name << "' to JIT: " << toString(std::move(err)) << "\n";
        return;
    }

    // Lookup functions
    m_mainFunction = (MainFunctionType)lookupFunction("f");
    assert(m_mainFunction);
    m_resumeFunction = (ResumeFunctionType)lookupFunction("resume");
    assert(m_resumeFunction);
}

void LLVMExecutableCode::run(ExecutionContext *context)
{
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
        void *handle = m_mainFunction(context, target, target->variableData(), target->listData());

        if (!handle)
            ctx->setFinished(true);

        ctx->setCoroutineHandle(handle);
    }
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
    return std::make_shared<LLVMExecutionContext>(thread);
}

uint64_t LLVMExecutableCode::lookupFunction(const std::string &name)
{
    auto func = m_jit->get()->lookup(name);

    if (func)
        return func->getValue();
    else {
        llvm::errs() << "error: failed to lookup LLVM function: " << toString(func.takeError()) << "\n";
        return 0;
    }
}

LLVMExecutionContext *LLVMExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<LLVMExecutionContext *>(context));
    return static_cast<LLVMExecutionContext *>(context);
}
