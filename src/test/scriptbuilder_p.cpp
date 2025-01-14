// SPDX-License-Identifier: Apache-2.0

#include "scriptbuilder_p.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

ScriptBuilderPrivate::ScriptBuilderPrivate(IEngine *engine, std::shared_ptr<Target> target) :
    engine(engine),
    target(target)
{
}
