// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executioncontext.h>

#include "../../../test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT CppExecutionContext : public ExecutionContext
{
    public:
        CppExecutionContext(Thread *thread);
        ~CppExecutionContext();

        void *frame() const;
        void setFrame(void *newFrame);

        bool finished() const;
        void setFinished(bool newFinished);

    private:
        void *m_frame = nullptr;
        bool m_finished = false;
};

} // namespace libscratchcpp
