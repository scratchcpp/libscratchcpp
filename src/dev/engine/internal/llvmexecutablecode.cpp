// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/value_functions.h>
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
}

void LLVMExecutableCode::run(ExecutionContext *context)
{
    LLVMExecutionContext *ctx = getContext(context);

    if (!ctx->finished) {
        m_mainFunction(context->target());
        ctx->finished = true;
    }
}

void LLVMExecutableCode::kill(ExecutionContext *context)
{
    getContext(context)->finished = true;
}

void LLVMExecutableCode::reset(ExecutionContext *context)
{
    getContext(context)->finished = false;
}

bool LLVMExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->finished;
}

void LLVMExecutableCode::promise()
{
}

void LLVMExecutableCode::resolvePromise()
{
}

std::shared_ptr<ExecutionContext> LLVMExecutableCode::createExecutionContext(Target *target) const
{
    return std::make_shared<LLVMExecutionContext>(target);
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
