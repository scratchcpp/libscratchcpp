// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>

#include "eventblocks.h"

using namespace libscratchcpp;

std::string EventBlocks::name() const
{
    return "Events";
}

std::string libscratchcpp::EventBlocks::description() const
{
    return "Event blocks";
}

void EventBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObject);
}

CompilerValue *EventBlocks::compileWhenTouchingObject(Compiler *compiler)
{
    compiler->engine()->addWhenTouchingObjectScript(compiler->block());
    return nullptr;
}
