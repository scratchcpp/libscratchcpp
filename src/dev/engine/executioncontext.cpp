// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/executioncontext.h>

#include "executioncontext_p.h"

using namespace libscratchcpp;

/*! Constructs ExecutionContext. */
ExecutionContext::ExecutionContext(Target *target) :
    impl(spimpl::make_unique_impl<ExecutionContextPrivate>(target))
{
}

/*! Returns the Target of this context. */
Target *ExecutionContext::target() const
{
    return impl->target;
}
