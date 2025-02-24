// SPDX-License-Identifier: Apache-2.0

#include "llvmexecutioncontext.h"
#include "llvmcompilercontext.h"

using namespace libscratchcpp;

LLVMExecutionContext::LLVMExecutionContext(LLVMCompilerContext *compilerCtx, Thread *thread) :
    ExecutionContext(thread),
    m_compilerCtx(compilerCtx)
{
}

LLVMExecutionContext::~LLVMExecutionContext()
{
    if (m_coroutineHandle) {
        if (m_compilerCtx) {
            m_compilerCtx->destroyCoroutine(m_coroutineHandle);
            m_coroutineHandle = nullptr;
        } else
            assert(false);
    }
}

void *LLVMExecutionContext::coroutineHandle() const
{
    return m_coroutineHandle;
}

void LLVMExecutionContext::setCoroutineHandle(void *newCoroutineHandle)
{
    m_coroutineHandle = newCoroutineHandle;
}

bool LLVMExecutionContext::finished() const
{
    return m_finished;
}

void LLVMExecutionContext::setFinished(bool newFinished)
{
    m_finished = newFinished;
}
