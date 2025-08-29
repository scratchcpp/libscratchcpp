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
        impl->hatPredicateCode = impl->script->hatPredicateCode();

        if (impl->code)
            impl->executionContext = impl->code->createExecutionContext(this);

        if (impl->hatPredicateCode)
            impl->hatPredicateExecutionContext = impl->hatPredicateCode->createExecutionContext(this);
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

/*! Runs the reporter and returns its return value. */
ValueData Thread::runReporter()
{
    string_pool_set_thread(this);
    ValueData ret = impl->code->runReporter(impl->executionContext.get());
    string_pool_set_thread(nullptr);
    return ret;
}

/*! Runs the hat predicate and returns its return value. */
bool Thread::runPredicate()
{
    if (!impl->hatPredicateCode)
        return false;

    string_pool_set_thread(this);
    const bool ret = impl->hatPredicateCode->runPredicate(impl->hatPredicateExecutionContext.get());
    string_pool_set_thread(nullptr);
    return ret;
}

/*! Stops the script. */
void Thread::kill()
{
    impl->code->kill(impl->executionContext.get());
    string_pool_clear_thread(this);
}

/*! Resets the script to run from the start. */
void Thread::reset()
{
    impl->code->reset(impl->executionContext.get());
    string_pool_clear_thread(this);
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
