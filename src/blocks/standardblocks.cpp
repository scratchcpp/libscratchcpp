// SPDX-License-Identifier: Apache-2.0

#include "standardblocks.h"
#include "controlblocks.h"
#include "eventblocks.h"
#include "motionblocks.h"
#include "looksblocks.h"
#include "soundblocks.h"
#include "sensingblocks.h"
#include "operatorblocks.h"
#include "variableblocks.h"
#include "listblocks.h"

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
    engine->registerSection(std::make_shared<LooksBlocks>());
    engine->registerSection(std::make_shared<SoundBlocks>());
    engine->registerSection(std::make_shared<EventBlocks>());
    engine->registerSection(std::make_shared<ControlBlocks>());
    engine->registerSection(std::make_shared<SensingBlocks>());
    engine->registerSection(std::make_shared<OperatorBlocks>());
    engine->registerSection(std::make_shared<VariableBlocks>());
    engine->registerSection(std::make_shared<ListBlocks>());
}
