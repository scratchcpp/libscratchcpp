// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executioncontext.h>

#include "llvmcompilercontext.h"
#include "test_export.h"

namespace libscratchcpp
{

struct StringPtr;

class LIBSCRATCHCPP_TEST_EXPORT LLVMExecutionContext : public ExecutionContext
{
    public:
        LLVMExecutionContext(LLVMCompilerContext *compilerCtx, Thread *thread);
        ~LLVMExecutionContext();

        void *coroutineHandle() const;
        void setCoroutineHandle(void *newCoroutineHandle);

        bool finished() const;
        void setFinished(bool newFinished);

    private:
        LLVMCompilerContext *m_compilerCtx = nullptr;
        void *m_coroutineHandle = nullptr;
        bool m_finished = false;
};

} // namespace libscratchcpp
