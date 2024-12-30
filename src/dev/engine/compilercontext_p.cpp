// SPDX-License-Identifier: Apache-2.0

#include "compilercontext_p.h"

using namespace libscratchcpp;

CompilerContextPrivate::CompilerContextPrivate(IEngine *engine, Target *target) :
    engine(engine),
    target(target)
{
}
