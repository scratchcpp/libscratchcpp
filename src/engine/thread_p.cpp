// SPDX-License-Identifier: Apache-2.0

#include "thread_p.h"

using namespace libscratchcpp;

ThreadPrivate::ThreadPrivate(Target *target, IEngine *engine, Script *script) :
    target(target),
    engine(engine),
    script(script)
{
}
