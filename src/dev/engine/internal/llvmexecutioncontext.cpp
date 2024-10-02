// SPDX-License-Identifier: Apache-2.0

#include "llvmexecutioncontext.h"

using namespace libscratchcpp;

LLVMExecutionContext::LLVMExecutionContext(Target *target) :
    ExecutionContext(target)
{
}

size_t LLVMExecutionContext::pos() const
{
    return m_pos;
}

void LLVMExecutionContext::setPos(size_t newPos)
{
    m_pos = newPos;
}
