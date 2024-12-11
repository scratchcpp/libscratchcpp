// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/thread.h>

#include "executioncontext_p.h"

using namespace libscratchcpp;

/*! Constructs ExecutionContext. */
ExecutionContext::ExecutionContext(Thread *thread) :
    impl(spimpl::make_unique_impl<ExecutionContextPrivate>(thread))
{
}

/*! Returns the thread of this context. */
Thread *ExecutionContext::thread() const
{
    return impl->thread;
}

/*! Returns the engine of the project. */
IEngine *ExecutionContext::engine() const
{
    return impl->thread->engine();
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
