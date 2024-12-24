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

/*! Returns the stack timer of this context. Can be used for wait blocks. */
IStackTimer *ExecutionContext::stackTimer() const
{
    return impl->stackTimer;
}

/*! Sets a custom stack timer. */
void ExecutionContext::setStackTimer(IStackTimer *newStackTimer)
{
    impl->stackTimer = newStackTimer;
}

/*! Returns the random number generator of this context. */
IRandomGenerator *ExecutionContext::rng() const
{
    return impl->rng;
}

/*! Sets a custom random number generator. */
void ExecutionContext::setRng(IRandomGenerator *newRng)
{
    impl->rng = newRng;
}
