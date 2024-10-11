// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/executioncontext.h>

namespace libscratchcpp
{

class LLVMExecutionContext : public ExecutionContext
{
    public:
        LLVMExecutionContext(Target *target);

        bool finished = false; // TODO: Remove this
};

} // namespace libscratchcpp
