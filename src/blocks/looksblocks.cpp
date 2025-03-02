// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/target.h>
#include <scratchcpp/textbubble.h>
#include "looksblocks.h"

using namespace libscratchcpp;

std::string LooksBlocks::name() const
{
    return "Looks";
}

std::string LooksBlocks::description() const
{
    return name() + " blocks";
}

Rgb LooksBlocks::color() const
{
    return rgb(153, 102, 255);
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
}

void LooksBlocks::onInit(IEngine *engine)
{
    engine->stopped().connect([engine]() {
        const auto &targets = engine->targets();

        for (auto target : targets) {
            target->bubble()->setText("");
            target->clearGraphicsEffects();
        }
    });
}
