// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "../global.h"

namespace libscratchcpp
{

class ExecutionContext;
class Thread;

/*! \brief The ExecutableCode class represents the code of a compiled Scratch script. */
class LIBSCRATCHCPP_EXPORT ExecutableCode
{
    public:
        virtual ~ExecutableCode() { }

        /*! Runs the script until it finishes or yields. */
        virtual void run(ExecutionContext *context) = 0;

        /*! Stops the code. isFinished() will return true. */
        virtual void kill(ExecutionContext *context) = 0;

        /*! Resets the code to run from the start. */
        virtual void reset(ExecutionContext *context) = 0;

        /*! Returns true if the code is stopped or finished. */
        virtual bool isFinished(ExecutionContext *context) const = 0;

        /*! Creates an execution context for the given Target. */
        virtual std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const = 0;
};

} // namespace libscratchcpp
