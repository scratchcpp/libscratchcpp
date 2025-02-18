// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/script.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/thread.h>
#include <iostream>

#include "script_p.h"

using namespace libscratchcpp;

/*! Constructs Script. */
Script::Script(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine) :
    impl(spimpl::make_unique_impl<ScriptPrivate>(target, topBlock, engine))
{
}

/*! Returns the Target. */
Target *Script::target() const
{
    return impl->target;
}

std::shared_ptr<Block> Script::topBlock() const
{
    return impl->topBlock;
}

/*! Returns the executable code of the script. */
ExecutableCode *Script::code() const
{
    return impl->code.get();
}

/*! Sets the executable code of the script. */
void Script::setCode(std::shared_ptr<ExecutableCode> code)
{
    impl->code = code;
}

/*! Returns the executable code of the hat predicate. */
ExecutableCode *Script::hatPredicateCode() const
{
    return impl->hatPredicateCode.get();
}

/*! Sets the executable code of the hat predicate. */
void Script::setHatPredicateCode(std::shared_ptr<ExecutableCode> code)
{
    impl->hatPredicateCode = code;
}

/*!
 * Runs the edge-activated hat predicate as the given target and returns the reported value.
 * \note If there isn't any predicate, nothing will happen and the returned value will be false.
 */
bool Script::runHatPredicate(Target *target)
{
    if (!target || !impl->engine)
        return false;

    // TODO: Implement this
    // auto thread = std::make_shared<Thread>(target, impl->engine, this);

    return false;
}

/*! Starts the script (creates a thread). */
std::shared_ptr<Thread> Script::start()
{
    return start(impl->target);
}

/*! Starts the script (creates a thread) as the given target (might be a clone). */
std::shared_ptr<Thread> Script::start(Target *target)
{
    auto thread = std::make_shared<Thread>(target, impl->engine, this);
    return thread;
}
