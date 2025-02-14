// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>

#include "thread_p.h"
#include "scratch/string_pool_p.h"

using namespace libscratchcpp;

/*! Constructs Thread. */
Thread::Thread(Target *target, IEngine *engine, Script *script) :
    impl(spimpl::make_unique_impl<ThreadPrivate>(target, engine, script))
{
    string_pool_add_thread(this);

    if (impl->script) {
        impl->code = impl->script->code();

        if (impl->code)
            impl->executionContext = impl->code->createExecutionContext(this);
    }
}

/*! Destroys Thread. */
Thread::~Thread()
{
    string_pool_remove_thread(this);
}

/*! Returns the Target of the script. */
Target *Thread::target() const
{
    return impl->target;
}

/*! Returns the engine of the project. */
IEngine *Thread::engine() const
{
    return impl->engine;
}

/*! Returns the Script. */
Script *Thread::script() const
{
    return impl->script;
}

/*! Runs the script until it finishes or yields. */
void Thread::run()
{
    string_pool_set_thread(this);
    impl->code->run(impl->executionContext.get());
    string_pool_set_thread(nullptr);
}

/*! Stops the script. */
void Thread::kill()
{
    impl->code->kill(impl->executionContext.get());
}

/*! Resets the script to run from the start. */
void Thread::reset()
{
    impl->code->reset(impl->executionContext.get());
}

/*! Returns true if the script is stopped or finished. */
bool Thread::isFinished() const
{
    return impl->code->isFinished(impl->executionContext.get());
}

/*! Returns the script promise. */
std::shared_ptr<Promise> Thread::promise() const
{
    return impl->executionContext->promise();
}

/*! Sets the script promise (yields until the promise is resolved). */
void Thread::setPromise(std::shared_ptr<Promise> promise)
{
    impl->executionContext->setPromise(promise);
}
