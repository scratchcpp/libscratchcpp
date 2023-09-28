// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>

#include "motionblocks.h"
#include "../engine/internal/randomgenerator.h"
#include "../engine/internal/clock.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

IRandomGenerator *MotionBlocks::rng = nullptr;
IClock *MotionBlocks::clock = nullptr;

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
    engine->addCompileFunction(this, "motion_goto", &compileGoTo);
    engine->addCompileFunction(this, "motion_glidesecstoxy", &compileGlideSecsToXY);
    engine->addCompileFunction(this, "motion_glideto", &compileGlideTo);
    engine->addCompileFunction(this, "motion_changexby", &compileChangeXBy);
    engine->addCompileFunction(this, "motion_setx", &compileSetX);
    engine->addCompileFunction(this, "motion_changeyby", &compileChangeYBy);
    engine->addCompileFunction(this, "motion_sety", &compileSetY);
    engine->addCompileFunction(this, "motion_setrotationstyle", &compileSetRotationStyle);
    engine->addCompileFunction(this, "motion_xposition", &compileXPosition);

    // Inputs
    engine->addInput(this, "STEPS", STEPS);
    engine->addInput(this, "DEGREES", DEGREES);
    engine->addInput(this, "DIRECTION", DIRECTION);
    engine->addInput(this, "TOWARDS", TOWARDS);
    engine->addInput(this, "X", X);
    engine->addInput(this, "Y", Y);
    engine->addInput(this, "TO", TO);
    engine->addInput(this, "SECS", SECS);
    engine->addInput(this, "DX", DX);
    engine->addInput(this, "DY", DY);

    // Fields
    engine->addField(this, "STYLE", STYLE);
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

void MotionBlocks::compileGoTo(Compiler *compiler)
{
    Input *input = compiler->input(TO);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addFunctionCall(&goToMousePointer);
        else if (value == "_random_")
            compiler->addFunctionCall(&goToRandomPosition);
        else {
            int index = compiler->engine()->findTarget(value);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&goToByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&goTo);
    }
}

void MotionBlocks::compileGlideSecsToXY(Compiler *compiler)
{
    compiler->addInput(SECS);
    compiler->addInput(X);
    compiler->addInput(Y);
    compiler->addFunctionCall(&startGlideSecsTo);
    compiler->addFunctionCall(&glideSecsTo);
}

void MotionBlocks::compileGlideTo(Compiler *compiler)
{
    compiler->addInput(SECS);

    Input *input = compiler->input(TO);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addFunctionCall(&startGlideToMousePointer);
        else if (value == "_random_")
            compiler->addFunctionCall(&startGlideToRandomPosition);
        else {
            int index = compiler->engine()->findTarget(value);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&startGlideToByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&startGlideTo);
    }

    compiler->addFunctionCall(&glideSecsTo);
}

void MotionBlocks::compileChangeXBy(Compiler *compiler)
{
    compiler->addInput(DX);
    compiler->addFunctionCall(&changeXBy);
}

void MotionBlocks::compileSetX(Compiler *compiler)
{
    compiler->addInput(X);
    compiler->addFunctionCall(&setX);
}

void MotionBlocks::compileChangeYBy(Compiler *compiler)
{
    compiler->addInput(DY);
    compiler->addFunctionCall(&changeYBy);
}

void MotionBlocks::compileSetY(Compiler *compiler)
{
    compiler->addInput(Y);
    compiler->addFunctionCall(&setY);
}

void MotionBlocks::compileSetRotationStyle(Compiler *compiler)
{
    int option = compiler->field(STYLE)->specialValueId();

    switch (option) {
        case LeftRight:
            compiler->addFunctionCall(&setLeftRightRotationStyle);
            break;

        case DoNotRotate:
            compiler->addFunctionCall(&setDoNotRotateRotationStyle);
            break;

        case AllAround:
            compiler->addFunctionCall(&setAllAroundRotationStyle);
            break;

        default:
            break;
    }
}

void MotionBlocks::compileXPosition(Compiler *compiler)
{
    compiler->addFunctionCall(&xPosition);
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
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));
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

unsigned int MotionBlocks::goTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (!sprite)
        return 1;

    std::string value = vm->getInput(0, 1)->toString();

    if (value == "_mouse_") {
        sprite->setX(vm->engine()->mouseX());
        sprite->setY(vm->engine()->mouseY());
    } else if (value == "_random_") {
        // TODO: Read stage size from engine (#224)
        static const unsigned int stageWidth = 480;
        static const unsigned int stageHeight = 360;

        if (!rng)
            rng = RandomGenerator::instance().get();

        sprite->setX(rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2));
        sprite->setY(rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2));
    } else {
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));
        Sprite *targetSprite = dynamic_cast<Sprite *>(target);

        if (targetSprite) {
            sprite->setX(targetSprite->x());
            sprite->setY(targetSprite->y());
        }
    }

    return 1;
}

unsigned int MotionBlocks::goToByIndex(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *targetSprite = dynamic_cast<Sprite *>(target);

    if (sprite && targetSprite) {
        sprite->setX(targetSprite->x());
        sprite->setY(targetSprite->y());
    }

    return 1;
}

unsigned int MotionBlocks::goToMousePointer(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        sprite->setX(vm->engine()->mouseX());
        sprite->setY(vm->engine()->mouseY());
    }

    return 0;
}

unsigned int MotionBlocks::goToRandomPosition(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        // TODO: Read stage size from engine (#224)
        static const unsigned int stageWidth = 480;
        static const unsigned int stageHeight = 360;

        if (!rng)
            rng = RandomGenerator::instance().get();

        sprite->setX(rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2));
        sprite->setY(rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2));
    }

    return 0;
}

void MotionBlocks::startGlidingToPos(VirtualMachine *vm, double x, double y, double secs)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (!sprite)
        return;

    if (secs <= 0) {
        if (sprite) {
            sprite->setX(x);
            sprite->setY(y);
        }

        return;
    }

    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    m_timeMap[vm] = { currentTime, secs * 1000 };
    m_glideMap[vm] = { { sprite->x(), sprite->y() }, { x, y } };
}

void MotionBlocks::continueGliding(VirtualMachine *vm)
{
    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_timeMap[vm].first).count();
    auto maxTime = m_timeMap[vm].second;
    assert(m_timeMap.count(vm) == 1);

    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());
    double x = m_glideMap[vm].second.first;
    double y = m_glideMap[vm].second.second;

    if (elapsedTime >= maxTime) {
        if (sprite) {
            sprite->setX(x);
            sprite->setY(y);
        }

        m_timeMap.erase(vm);
        m_glideMap.erase(vm);
    } else {
        if (sprite) {
            double startX = m_glideMap[vm].first.first;
            double startY = m_glideMap[vm].first.second;
            double factor = elapsedTime / static_cast<double>(maxTime);
            assert(factor >= 0 && factor < 1);

            sprite->setX(startX + (x - startX) * factor);
            sprite->setY(startY + (y - startY) * factor);
        }

        vm->stop(true, true, true);
        vm->engine()->lockFrame();
    }
}

unsigned int MotionBlocks::startGlideSecsTo(VirtualMachine *vm)
{
    startGlidingToPos(vm, vm->getInput(1, 3)->toDouble(), vm->getInput(2, 3)->toDouble(), vm->getInput(0, 3)->toDouble());
    return 3;
}

unsigned int MotionBlocks::glideSecsTo(VirtualMachine *vm)
{
    if (m_timeMap.find(vm) != m_timeMap.cend()) {
        assert(m_glideMap.find(vm) != m_glideMap.cend());
        continueGliding(vm);
    }

    return 0;
}

unsigned int MotionBlocks::startGlideTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (!sprite)
        return 1;

    std::string value = vm->getInput(1, 2)->toString();

    if (value == "_mouse_")
        startGlidingToPos(vm, vm->engine()->mouseX(), vm->engine()->mouseY(), vm->getInput(0, 2)->toDouble());
    else if (value == "_random_") {
        // TODO: Read stage size from engine (#224)
        static const unsigned int stageWidth = 480;
        static const unsigned int stageHeight = 360;

        if (!rng)
            rng = RandomGenerator::instance().get();

        startGlidingToPos(vm, rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2), rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2), vm->getInput(0, 2)->toDouble());
    } else {
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));
        Sprite *targetSprite = dynamic_cast<Sprite *>(target);

        if (targetSprite)
            startGlidingToPos(vm, targetSprite->x(), targetSprite->y(), vm->getInput(0, 2)->toDouble());
    }

    return 2;
}

unsigned int MotionBlocks::startGlideToByIndex(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());
    Target *target = vm->engine()->targetAt(vm->getInput(1, 2)->toInt());
    Sprite *targetSprite = dynamic_cast<Sprite *>(target);

    if (sprite && targetSprite)
        startGlidingToPos(vm, targetSprite->x(), targetSprite->y(), vm->getInput(0, 2)->toDouble());

    return 2;
}

unsigned int MotionBlocks::startGlideToMousePointer(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        startGlidingToPos(vm, vm->engine()->mouseX(), vm->engine()->mouseY(), vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::startGlideToRandomPosition(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        // TODO: Read stage size from engine (#224)
        static const unsigned int stageWidth = 480;
        static const unsigned int stageHeight = 360;

        if (!rng)
            rng = RandomGenerator::instance().get();

        startGlidingToPos(vm, rng->randint(-static_cast<int>(stageWidth / 2), stageWidth / 2), rng->randint(-static_cast<int>(stageHeight / 2), stageHeight / 2), vm->getInput(0, 1)->toDouble());
    }

    return 1;
}

unsigned int MotionBlocks::changeXBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setX(sprite->x() + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::setX(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setX(vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::changeYBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setY(sprite->y() + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::setY(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setY(vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int MotionBlocks::setLeftRightRotationStyle(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setRotationStyle(Sprite::RotationStyle::LeftRight);

    return 0;
}

unsigned int MotionBlocks::setDoNotRotateRotationStyle(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setRotationStyle(Sprite::RotationStyle::DoNotRotate);

    return 0;
}

unsigned int MotionBlocks::setAllAroundRotationStyle(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setRotationStyle(Sprite::RotationStyle::AllAround);

    return 0;
}

unsigned int MotionBlocks::xPosition(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->addReturnValue(sprite->x());
    else
        vm->addReturnValue(0);

    return 0;
}
