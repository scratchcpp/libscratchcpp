// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executioncontext.h>

namespace libscratchcpp
{

class CppCompilerContext;

class CppExecutionContext : public ExecutionContext
{
    public:
        CppExecutionContext(CppCompilerContext *ctx, Thread *thread);

    private:
        CppCompilerContext *m_ctx = nullptr;
};

} // namespace libscratchcpp
