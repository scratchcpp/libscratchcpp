// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/virtualmachine.h>
#ifdef USE_LLVM
#include <scratchcpp/script.h>
#include <scratchcpp/dev/executablecode.h>
#endif

#include "thread_p.h"

using namespace libscratchcpp;

/*! Constructs Thread. */
Thread::Thread(Target *target, IEngine *engine, Script *script) :
    impl(spimpl::make_unique_impl<ThreadPrivate>(target, engine, script))
{
    impl->vm = std::make_unique<VirtualMachine>(target, engine, script, this);
#ifdef USE_LLVM
    impl->code = impl->script->code();
    impl->executionContext = impl->code->createExecutionContext(target);
#endif
}

/*! Returns the virtual machine of this thread. */
VirtualMachine *Thread::vm() const
{
    // TODO: Remove this method
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
#ifdef USE_LLVM
    impl->code->run(impl->executionContext.get());
#else
    impl->vm->run();
#endif
}

/*! Stops the script. */
void Thread::kill()
{
#ifdef USE_LLVM
    impl->code->kill(impl->executionContext.get());
#else
    impl->vm->kill();
#endif
}

/*! Resets the script to run from the start. */
void Thread::reset()
{
#ifdef USE_LLVM
    impl->code->reset(impl->executionContext.get());
#else
    impl->vm->reset();
#endif
}

/*! Returns true if the script is stopped or finished. */
bool Thread::isFinished() const
{
#ifdef USE_LLVM
    return impl->code->isFinished(impl->executionContext.get());
#else
    return impl->vm->atEnd();
#endif
}

/*! Pauses the script (when it's executed using run() again) until resolvePromise() is called. */
void Thread::promise()
{
#ifdef USE_LLVM
    impl->code->promise();
#else
    impl->vm->promise();
#endif
}

/*! Resolves the promise and resumes the script. */
void Thread::resolvePromise()
{
#ifdef USE_LLVM
    impl->code->resolvePromise();
#else
    impl->vm->resolvePromise();
#endif
}
