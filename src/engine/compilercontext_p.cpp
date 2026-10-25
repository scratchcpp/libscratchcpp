// SPDX-License-Identifier: Apache-2.0

#include "compilercontext_p.h"

using namespace libscratchcpp;

CompilerContextPrivate::CompilerContextPrivate(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants) :
    engine(engine),
    target(target),
    unsafeConstants(unsafeConstants)
{
}
