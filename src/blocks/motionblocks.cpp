// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/input.h>

#include "motionblocks.h"
#include "../engine/internal/randomgenerator.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

IRandomGenerator *MotionBlocks::rng = nullptr;

std::string MotionBlocks::name() const
{
    return "Motion";
}

void MotionBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "motion_movesteps", &compileMoveSteps);
    engine->addCompileFunction(this, "motion_turnright", &compileTurnRight);
    engine->addCompileFunction(this, "motion_turnleft", &compileTurnLeft);
    engine->addCompileFunction(this, "motion_pointindirection", &compilePointInDirection);
    engine->addCompileFunction(this, "motion_pointtowards", &compilePointTowards);
    engine->addCompileFunction(this, "motion_gotoxy", &compileGoToXY);

    // Inputs
    engine->addInput(this, "STEPS", STEPS);
    engine->addInput(this, "DEGREES", DEGREES);
    engine->addInput(this, "DIRECTION", DIRECTION);
    engine->addInput(this, "TOWARDS", TOWARDS);
    engine->addInput(this, "X", X);
    engine->addInput(this, "Y", Y);
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

void MotionBlocks::compileTurnLeft(Compiler *compiler)
{
    compiler->addInput(DEGREES);
    compiler->addFunctionCall(&turnLeft);
}

void MotionBlocks::compilePointInDirection(Compiler *compiler)
{
    compiler->addInput(DIRECTION);
    compiler->addFunctionCall(&pointInDirection);
}

void MotionBlocks::compilePointTowards(Compiler *compiler)
{
    Input *input = compiler->input(TOWARDS);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addFunctionCall(&pointTowardsMousePointer);
        else if (value == "_random_")
            compiler->addFunctionCall(&pointTowardsRandomPosition);
        else {
            int index = compiler->engine()->findTarget(value);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&pointTowardsByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&pointTowards);
    }
}

void MotionBlocks::compileGoToXY(Compiler *compiler)
{
    compiler->addInput(X);
    compiler->addInput(Y);
    compiler->addFunctionCall(&goToXY);
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

unsigned int MotionBlocks::turnLeft(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setDirection(sprite->direction() - vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::pointInDirection(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setDirection(vm->getInput(0, 1)->toDouble());

    return 1;
}

void MotionBlocks::pointTowardsPos(Sprite *sprite, double x, double y)
{
    if (!sprite)
        return;

    // https://en.scratch-wiki.info/wiki/Point_Towards_()_(block)#Workaround
    double deltaX = x - sprite->x();
    double deltaY = y - sprite->y();

    if (deltaY == 0) {
        if (deltaX < 0)
            sprite->setDirection(-90);
        else
            sprite->setDirection(90);
    } else if (deltaY < 0)
        sprite->setDirection(180 + (180 / pi) * std::atan(deltaX / deltaY));
    else
        sprite->setDirection((180 / pi) * std::atan(deltaX / deltaY));
}

unsigned int MotionBlocks::pointTowards(VirtualMachine *vm)
{
    std::string value = vm->getInput(0, 1)->toString();
    Target *target;

    if (value == "_mouse_")
        pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), vm->engine()->mouseX(), vm->engine()->mouseY());
    else if (value == "_random_") {
        // TODO: Read stage size from engine (#224)
        static const unsigned int stageWidth = 480;
        static const unsigned int stageHeight = 360;

        if (!rng)
            rng = RandomGenerator::instance().get();

        pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2), rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2));
    } else {
        target = vm->engine()->targetAt(vm->engine()->findTarget(value));
        Sprite *sprite = dynamic_cast<Sprite *>(target);

        if (sprite)
            pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), sprite->x(), sprite->y());
    }

    return 1;
}

unsigned int MotionBlocks::pointTowardsByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), sprite->x(), sprite->y());

    return 1;
}

unsigned int MotionBlocks::pointTowardsMousePointer(VirtualMachine *vm)
{
    pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), vm->engine()->mouseX(), vm->engine()->mouseY());
    return 0;
}

unsigned int MotionBlocks::pointTowardsRandomPosition(VirtualMachine *vm)
{
    // TODO: Read stage size from engine (#224)
    static const unsigned int stageWidth = 480;
    static const unsigned int stageHeight = 360;

    if (!rng)
        rng = RandomGenerator::instance().get();

    pointTowardsPos(dynamic_cast<Sprite *>(vm->target()), rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2), rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2));

    return 0;
}

unsigned int MotionBlocks::goToXY(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        sprite->setX(vm->getInput(0, 2)->toDouble());
        sprite->setY(vm->getInput(1, 2)->toDouble());
    }

    return 2;
}
