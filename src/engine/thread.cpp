// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/virtualmachine.h>

#include "thread_p.h"

using namespace libscratchcpp;

/*! Constructs Thread. */
Thread::Thread(Target *target, IEngine *engine, Script *script) :
    impl(spimpl::make_unique_impl<ThreadPrivate>(target, engine, script))
{
    impl->vm = std::make_unique<VirtualMachine>(target, engine, script, this);
}

/*! Returns the virtual machine of this thread. */
VirtualMachine *Thread::vm() const
{
    return impl->vm.get();
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
    impl->vm->run();
}

/*! Stops the script. */
void Thread::kill()
{
    impl->vm->kill();
}

/*! Resets the script to run from the start. */
void Thread::reset()
{
    impl->vm->reset();
}

/*! Returns true if the script is stopped or finished. */
bool Thread::isFinished() const
{
    return impl->vm->atEnd();
}

/*! Pauses the script (when it's executed using run() again) until resolvePromise() is called. */
void Thread::promise()
{
    impl->vm->promise();
}

/*! Resolves the promise and resumes the script. */
void Thread::resolvePromise()
{
    impl->vm->resolvePromise();
}
