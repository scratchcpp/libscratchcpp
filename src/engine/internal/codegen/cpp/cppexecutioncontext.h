// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executioncontext.h>

#include "test_export.h"

namespace libscratchcpp
{

class CppCompilerContext;

class LIBSCRATCHCPP_TEST_EXPORT CppExecutionContext : public ExecutionContext
{
    public:
        CppExecutionContext(CppCompilerContext *ctx, Thread *thread);

        bool finished() const;
        void setFinished(bool finished);

    private:
        CppCompilerContext *m_ctx = nullptr;
        bool m_finished = false;
};

} // namespace libscratchcpp
