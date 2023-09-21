// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>

#include "motionblocks.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

std::string MotionBlocks::name() const
{
    return "Motion";
}

void MotionBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "motion_movesteps", &compileMoveSteps);
    engine->addCompileFunction(this, "motion_turnright", &compileTurnRight);

    // Inputs
    engine->addInput(this, "STEPS", STEPS);
    engine->addInput(this, "DEGREES", DEGREES);
}

void MotionBlocks::compileMoveSteps(Compiler *compiler)
{
    compiler->addInput(STEPS);
    compiler->addFunctionCall(&moveSteps);
}

void MotionBlocks::compileTurnRight(Compiler *compiler)
{
    compiler->addInput(DEGREES);
    compiler->addFunctionCall(&turnRight);
}

unsigned int MotionBlocks::moveSteps(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        double dir = sprite->direction();
        double steps = vm->getInput(0, 1)->toDouble();
        sprite->setX(sprite->x() + std::sin(dir * pi / 180) * steps);
        sprite->setY(sprite->y() + std::cos(dir * pi / 180) * steps);
    }

    return 1;
}

unsigned int MotionBlocks::turnRight(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setDirection(sprite->direction() + vm->getInput(0, 1)->toDouble());

    return 1;
}
