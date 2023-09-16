// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/itimer.h>
#include "sensingblocks.h"

using namespace libscratchcpp;

std::string SensingBlocks::name() const
{
    return "Sensing";
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
}

void SensingBlocks::compileTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&timer);
}

unsigned int SensingBlocks::timer(VirtualMachine *vm)
{
    vm->addReturnValue(vm->engine()->timer()->value());
    return 0;
}
