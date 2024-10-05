// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/executioncontext.h>

namespace libscratchcpp
{

class LLVMExecutionContext : public ExecutionContext
{
    public:
        LLVMExecutionContext(Target *target);

        size_t pos() const;
        void setPos(size_t newPos);

    private:
        size_t m_pos = 0;
};

} // namespace libscratchcpp
