// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/valuedata.h>

#include "cppexecutablecode.h"

using namespace libscratchcpp;

CppExecutableCode::CppExecutableCode(CppCompilerContext *ctx) :
    m_ctx(ctx)
{
}

void CppExecutableCode::run(ExecutionContext *context)
{
}

ValueData CppExecutableCode::runReporter(ExecutionContext *context)
{
    return ValueData();
}

bool CppExecutableCode::runPredicate(ExecutionContext *context)
{
    return false;
}

void CppExecutableCode::kill(ExecutionContext *context)
{
}

void CppExecutableCode::reset(ExecutionContext *context)
{
}

bool CppExecutableCode::isFinished(ExecutionContext *context) const
{
    return true;
}

std::shared_ptr<ExecutionContext> CppExecutableCode::createExecutionContext(Thread *thread) const
{
    return nullptr;
}
