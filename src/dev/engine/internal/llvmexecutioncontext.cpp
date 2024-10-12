// SPDX-License-Identifier: Apache-2.0

#include "llvmexecutioncontext.h"

using namespace libscratchcpp;

LLVMExecutionContext::LLVMExecutionContext(Target *target) :
    ExecutionContext(target)
{
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
