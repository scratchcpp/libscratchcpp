// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/executioncontext.h>
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
    size_t i = 0;

    while (true) {
        auto func = m_jit->get()->lookup("f" + std::to_string(i));

        if (func)
            m_functions.push_back((FunctionType)(func->getValue()));
        else {
            // Ignore error
            llvm::consumeError(func.takeError());
            break;
        }

        i++;
    }
}

void LLVMExecutableCode::run(ExecutionContext *context)
{
    LLVMExecutionContext *ctx = getContext(context);

    if (ctx->pos() < m_functions.size())
        ctx->setPos(m_functions[ctx->pos()](context->target()));
}

void LLVMExecutableCode::kill(ExecutionContext *context)
{
    getContext(context)->setPos(m_functions.size());
}

void LLVMExecutableCode::reset(ExecutionContext *context)
{
    getContext(context)->setPos(0);
}

bool LLVMExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->pos() >= m_functions.size();
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

LLVMExecutionContext *LLVMExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<LLVMExecutionContext *>(context));
    return static_cast<LLVMExecutionContext *>(context);
}
