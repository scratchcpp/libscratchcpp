// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../global.h"
#include "../spimpl.h"

namespace libscratchcpp
{

class Thread;
class IEngine;
class Promise;
class ExecutionContextPrivate;

/*! \brief The ExecutionContext represents the execution context of a target (can be a clone) with variables, lists, etc. */
class LIBSCRATCHCPP_EXPORT ExecutionContext
{
    public:
        ExecutionContext(Thread *thread);
        ExecutionContext(const ExecutionContext &) = delete;
        virtual ~ExecutionContext() { }

        Thread *thread() const;
        IEngine *engine() const;

        std::shared_ptr<Promise> promise() const;
        void setPromise(std::shared_ptr<Promise> promise);

    private:
        spimpl::unique_impl_ptr<ExecutionContextPrivate> impl;
};

} // namespace libscratchcpp
