// SPDX-License-Identifier: Apache-2.0

#include "standardblocks.h"
#include "controlblocks.h"
#include "eventblocks.h"
#include "motionblocks.h"

using namespace libscratchcpp;

std::string StandardBlocks::name() const
{
    return "Standard Blocks";
}

std::string StandardBlocks::description() const
{
    return "Default blocks included with Scratch";
}

bool StandardBlocks::includeByDefault() const
{
    return true;
}

void StandardBlocks::registerSections(Engine *engine)
{
    engine->registerSection(std::make_shared<MotionBlocks>());
    engine->registerSection(std::make_shared<EventBlocks>());
    engine->registerSection(std::make_shared<ControlBlocks>());
}
