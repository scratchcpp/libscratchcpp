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

/*! Returns the script promise. */
std::shared_ptr<Promise> ExecutionContext::promise() const
{
    return impl->promise;
}

/*! Sets the script promise (yields until the promise is resolved). */
void ExecutionContext::setPromise(std::shared_ptr<Promise> promise)
{
    impl->promise = promise;
}
