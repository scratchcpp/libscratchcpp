// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/itimer.h>
#include "sensingblocks.h"

#include "../engine/internal/clock.h"

using namespace libscratchcpp;

IClock *SensingBlocks::clock = Clock::instance().get();

std::string SensingBlocks::name() const
{
    return "Sensing";
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
    engine->addCompileFunction(this, "sensing_resettimer", &compileResetTimer);
    engine->addCompileFunction(this, "sensing_dayssince2000", &compileDaysSince2000);
}

void SensingBlocks::compileTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&timer);
}

void SensingBlocks::compileResetTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&resetTimer);
}

void SensingBlocks::compileDaysSince2000(Compiler *compiler)
{
    compiler->addFunctionCall(&daysSince2000);
}

unsigned int SensingBlocks::timer(VirtualMachine *vm)
{
    vm->addReturnValue(vm->engine()->timer()->value());
    return 0;
}

unsigned int SensingBlocks::resetTimer(VirtualMachine *vm)
{
    vm->engine()->timer()->reset();
    return 0;
}

unsigned int SensingBlocks::daysSince2000(VirtualMachine *vm)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock->currentSystemTime().time_since_epoch()).count();
    vm->addReturnValue(ms / 86400000.0 - 10957);

    return 0;
}
