// SPDX-License-Identifier: Apache-2.0

#include "script_p.h"

using namespace libscratchcpp;

ScriptPrivate::ScriptPrivate(Target *target, IEngine *engine) :
    target(target),
    engine(engine)
{
}
