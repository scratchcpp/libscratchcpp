// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/promise.h>

#include "promise_p.h"

using namespace libscratchcpp;

/*! Constructs Promise. */
Promise::Promise() :
    impl(spimpl::make_unique_impl<PromisePrivate>())
{
}

/*! Returns true if the promise is resolved. */
bool Promise::isResolved() const
{
    return impl->isResolved;
}

/*! Marks the promise as resolved. */
void Promise::resolve()
{
    impl->isResolved = true;
}
