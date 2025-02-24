// SPDX-License-Identifier: Apache-2.0

#include "script_p.h"

using namespace libscratchcpp;

ScriptPrivate::ScriptPrivate(Target *target, Block *topBlock, IEngine *engine) :
    target(target),
    topBlock(topBlock),
    engine(engine)
{
}
