// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/executioncontext.h>

namespace libscratchcpp
{

class LLVMExecutionContext : public ExecutionContext
{
    public:
        LLVMExecutionContext(Thread *thread);

        void *coroutineHandle() const;
        void setCoroutineHandle(void *newCoroutineHandle);

        bool finished() const;
        void setFinished(bool newFinished);

    private:
        void *m_coroutineHandle = nullptr;
        bool m_finished = false;
};

} // namespace libscratchcpp
