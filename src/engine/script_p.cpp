// SPDX-License-Identifier: Apache-2.0

#include "script_p.h"

using namespace libscratchcpp;

ScriptPrivate::ScriptPrivate(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine) :
    target(target),
    topBlock(topBlock),
    engine(engine)
{
}
