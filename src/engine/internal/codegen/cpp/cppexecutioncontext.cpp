// SPDX-License-Identifier: Apache-2.0

#include "cppexecutioncontext.h"

using namespace libscratchcpp;

CppExecutionContext::CppExecutionContext(CppCompilerContext *ctx, Thread *thread) :
    ExecutionContext(thread),
    m_ctx(ctx)
{
}

bool CppExecutionContext::finished() const
{
    return m_finished;
}

void CppExecutionContext::setFinished(bool finished)
{
    m_finished = finished;
}
