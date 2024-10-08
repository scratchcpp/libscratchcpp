// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../global.h"
#include "../spimpl.h"

namespace libscratchcpp
{

class Target;
class ExecutionContextPrivate;

/*! \brief The ExecutionContext represents the execution context of a target (can be a clone) with variables, lists, etc. */
class LIBSCRATCHCPP_EXPORT ExecutionContext
{
    public:
        ExecutionContext(Target *target);
        ExecutionContext(const ExecutionContext &) = delete;
        virtual ~ExecutionContext() { }

        Target *target() const;

    private:
        spimpl::unique_impl_ptr<ExecutionContextPrivate> impl;
};

} // namespace libscratchcpp
