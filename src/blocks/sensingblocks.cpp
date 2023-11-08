// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/itimer.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include "sensingblocks.h"

#include "../engine/internal/clock.h"

using namespace libscratchcpp;

IClock *SensingBlocks::clock = nullptr;

std::string SensingBlocks::name() const
{
    return "Sensing";
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sensing_distanceto", &compileDistanceTo);
    engine->addCompileFunction(this, "sensing_keypressed", &compileKeyPressed);
    engine->addCompileFunction(this, "sensing_mousedown", &compileMouseDown);
    engine->addCompileFunction(this, "sensing_mousex", &compileMouseX);
    engine->addCompileFunction(this, "sensing_mousey", &compileMouseY);
    engine->addCompileFunction(this, "sensing_setdragmode", &compileSetDragMode);
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
    engine->addCompileFunction(this, "sensing_resettimer", &compileResetTimer);
    engine->addCompileFunction(this, "sensing_of", &compileOf);
    engine->addCompileFunction(this, "sensing_current", &compileCurrent);
    engine->addCompileFunction(this, "sensing_dayssince2000", &compileDaysSince2000);

    // Inputs
    engine->addInput(this, "DISTANCETOMENU", DISTANCETOMENU);
    engine->addInput(this, "KEY_OPTION", KEY_OPTION);
    engine->addInput(this, "OBJECT", OBJECT);

    // Fields
    engine->addField(this, "CURRENTMENU", CURRENTMENU);
    engine->addField(this, "DRAG_MODE", DRAG_MODE);
    engine->addField(this, "PROPERTY", PROPERTY);

    // Field values
    engine->addFieldValue(this, "YEAR", YEAR);
    engine->addFieldValue(this, "MONTH", MONTH);
    engine->addFieldValue(this, "DATE", DATE);
    engine->addFieldValue(this, "DAYOFWEEK", DAYOFWEEK);
    engine->addFieldValue(this, "HOUR", HOUR);
    engine->addFieldValue(this, "MINUTE", MINUTE);
    engine->addFieldValue(this, "SECOND", SECOND);
    engine->addFieldValue(this, "draggable", Draggable);
    engine->addFieldValue(this, "not draggable", NotDraggable);
    engine->addFieldValue(this, "x position", XPosition);
    engine->addFieldValue(this, "y position", YPosition);
    engine->addFieldValue(this, "direction", Direction);
    engine->addFieldValue(this, "costume #", CostumeNumber);
    engine->addFieldValue(this, "costume name", CostumeName);
    engine->addFieldValue(this, "size", Size);
    engine->addFieldValue(this, "volume", Volume);
    engine->addFieldValue(this, "background #", BackdropNumber); // Scratch 1.4 support
    engine->addFieldValue(this, "backdrop #", BackdropNumber);
    engine->addFieldValue(this, "backdrop name", BackdropName);
}

void SensingBlocks::compileDistanceTo(Compiler *compiler)
{
    Input *input = compiler->input(DISTANCETOMENU);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addFunctionCall(&distanceToMousePointer);
        else {
            int index = compiler->engine()->findTarget(value);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&distanceToByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&distanceTo);
    }
}

void SensingBlocks::compileKeyPressed(Compiler *compiler)
{
    compiler->addInput(KEY_OPTION);
    compiler->addFunctionCall(&keyPressed);
}

void SensingBlocks::compileMouseDown(Compiler *compiler)
{
    compiler->addFunctionCall(&mouseDown);
}

void SensingBlocks::compileMouseX(Compiler *compiler)
{
    compiler->addFunctionCall(&mouseX);
}

void SensingBlocks::compileMouseY(Compiler *compiler)
{
    compiler->addFunctionCall(&mouseY);
}

void SensingBlocks::compileSetDragMode(Compiler *compiler)
{
    int option = compiler->field(DRAG_MODE)->specialValueId();

    switch (option) {
        case Draggable:
            compiler->addFunctionCall(&setDraggableMode);
            break;

        case NotDraggable:
            compiler->addFunctionCall(&setNotDraggableMode);
            break;

        default:
            break;
    }
}

void SensingBlocks::compileTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&timer);
}

void SensingBlocks::compileResetTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&resetTimer);
}

void SensingBlocks::compileOf(Compiler *compiler)
{
    int option = compiler->field(PROPERTY)->specialValueId();
    Input *input = compiler->input(OBJECT);
    BlockFunc f = nullptr;

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        int index = compiler->engine()->findTarget(value);

        switch (option) {
            case XPosition:
                f = &xPositionOfSpriteByIndex;
                break;

            case YPosition:
                f = &yPositionOfSpriteByIndex;
                break;

            case Direction:
                f = &directionOfSpriteByIndex;
                break;

            case CostumeNumber:
                f = &costumeNumberOfSpriteByIndex;
                break;

            case CostumeName:
                f = &costumeNameOfSpriteByIndex;
                break;

            case Size:
                f = &sizeOfSpriteByIndex;
                break;

            case Volume:
                f = &volumeOfTargetByIndex;
                break;

            case BackdropNumber:
                f = &backdropNumberOfStageByIndex;
                break;

            case BackdropName:
                f = &backdropNameOfStageByIndex;
                break;

            default:
                break;
        }

        if (f)
            compiler->addConstValue(index);
    } else {
        switch (option) {
            case XPosition:
                f = &xPositionOfSprite;
                break;

            case YPosition:
                f = &yPositionOfSprite;
                break;

            case Direction:
                f = &directionOfSprite;
                break;

            case CostumeNumber:
                f = &costumeNumberOfSprite;
                break;

            case CostumeName:
                f = &costumeNameOfSprite;
                break;

            case Size:
                f = &sizeOfSprite;
                break;

            case Volume:
                f = &volumeOfTarget;
                break;

            case BackdropNumber:
                f = &backdropNumberOfStage;
                break;

            case BackdropName:
                f = &backdropNameOfStage;
                break;

            default:
                break;
        }

        if (f)
            compiler->addInput(input);
    }

    if (f)
        compiler->addFunctionCall(f);
}

void SensingBlocks::compileCurrent(Compiler *compiler)
{
    int id = compiler->field(CURRENTMENU)->specialValueId();

    switch (id) {
        case YEAR:
            compiler->addFunctionCall(&currentYear);
            break;

        case MONTH:
            compiler->addFunctionCall(&currentMonth);
            break;

        case DATE:
            compiler->addFunctionCall(&currentDate);
            break;

        case DAYOFWEEK:
            compiler->addFunctionCall(&currentDayOfWeek);
            break;

        case HOUR:
            compiler->addFunctionCall(&currentHour);
            break;

        case MINUTE:
            compiler->addFunctionCall(&currentMinute);
            break;

        case SECOND:
            compiler->addFunctionCall(&currentSecond);
            break;

        default:
            break;
    }
}

void SensingBlocks::compileDaysSince2000(Compiler *compiler)
{
    compiler->addFunctionCall(&daysSince2000);
}

unsigned int SensingBlocks::keyPressed(VirtualMachine *vm)
{
    vm->replaceReturnValue(vm->engine()->keyPressed(vm->getInput(0, 1)->toString()), 1);
    return 0;
}

unsigned int SensingBlocks::mouseDown(VirtualMachine *vm)
{
    vm->addReturnValue(vm->engine()->mousePressed());
    return 0;
}

unsigned int SensingBlocks::mouseX(VirtualMachine *vm)
{
    vm->addReturnValue(vm->engine()->mouseX());
    return 0;
}

unsigned int SensingBlocks::mouseY(VirtualMachine *vm)
{
    vm->addReturnValue(vm->engine()->mouseY());
    return 0;
}

unsigned int SensingBlocks::setDraggableMode(VirtualMachine *vm)
{
    if (Sprite *sprite = dynamic_cast<Sprite *>(vm->target()))
        sprite->setDraggable(true);

    return 0;
}

unsigned int SensingBlocks::setNotDraggableMode(VirtualMachine *vm)
{
    if (Sprite *sprite = dynamic_cast<Sprite *>(vm->target()))
        sprite->setDraggable(false);

    return 0;
}

unsigned int SensingBlocks::distanceTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (!sprite) {
        vm->replaceReturnValue(10000, 1);
        return 0;
    }

    std::string value = vm->getInput(0, 1)->toString();

    if (value == "_mouse_")
        vm->replaceReturnValue(std::sqrt(std::pow(sprite->x() - vm->engine()->mouseX(), 2) + std::pow(sprite->y() - vm->engine()->mouseY(), 2)), 1);
    else {
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));
        Sprite *targetSprite = dynamic_cast<Sprite *>(target);

        if (targetSprite)
            vm->replaceReturnValue(std::sqrt(std::pow(sprite->x() - targetSprite->x(), 2) + std::pow(sprite->y() - targetSprite->y(), 2)), 1);
        else
            vm->replaceReturnValue(10000, 1);
    }

    return 0;
}

unsigned int SensingBlocks::distanceToByIndex(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *targetSprite = dynamic_cast<Sprite *>(target);

    if (sprite && targetSprite)
        vm->replaceReturnValue(std::sqrt(std::pow(sprite->x() - targetSprite->x(), 2) + std::pow(sprite->y() - targetSprite->y(), 2)), 1);
    else
        vm->replaceReturnValue(10000, 1);

    return 0;
}

unsigned int SensingBlocks::distanceToMousePointer(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->addReturnValue(std::sqrt(std::pow(sprite->x() - vm->engine()->mouseX(), 2) + std::pow(sprite->y() - vm->engine()->mouseY(), 2)));
    else
        vm->addReturnValue(10000);

    return 0;
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

unsigned int SensingBlocks::xPositionOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->x(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::xPositionOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->x(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::yPositionOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->y(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::yPositionOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->y(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::directionOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->direction(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::directionOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->direction(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::costumeNumberOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->costumeIndex() + 1, 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::costumeNumberOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->costumeIndex() + 1, 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::costumeNameOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->currentCostume()->name(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::costumeNameOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->currentCostume()->name(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::sizeOfSprite(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->size(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::sizeOfSpriteByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        vm->replaceReturnValue(sprite->size(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::volumeOfTarget(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));

    if (target)
        vm->replaceReturnValue(target->volume(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::volumeOfTargetByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());

    if (target)
        vm->replaceReturnValue(target->volume(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::backdropNumberOfStage(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Stage *stage = dynamic_cast<Stage *>(target);

    if (stage)
        vm->replaceReturnValue(stage->costumeIndex() + 1, 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::backdropNumberOfStageByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Stage *stage = dynamic_cast<Stage *>(target);

    if (stage)
        vm->replaceReturnValue(stage->costumeIndex() + 1, 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::backdropNameOfStage(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(0, 1)->toString()));
    Stage *stage = dynamic_cast<Stage *>(target);

    if (stage)
        vm->replaceReturnValue(stage->currentCostume()->name(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::backdropNameOfStageByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Stage *stage = dynamic_cast<Stage *>(target);

    if (stage)
        vm->replaceReturnValue(stage->currentCostume()->name(), 1);
    else
        vm->replaceReturnValue(0, 1);

    return 0;
}

unsigned int SensingBlocks::currentYear(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_year + 1900);

    return 0;
}

unsigned int SensingBlocks::currentMonth(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_mon + 1);

    return 0;
}

unsigned int SensingBlocks::currentDate(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_mday);

    return 0;
}

unsigned int SensingBlocks::currentDayOfWeek(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_wday + 1);

    return 0;
}

unsigned int SensingBlocks::currentHour(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_hour);

    return 0;
}

unsigned int SensingBlocks::currentMinute(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_min);

    return 0;
}

unsigned int SensingBlocks::currentSecond(VirtualMachine *vm)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vm->addReturnValue(ltm->tm_sec);

    return 0;
}

unsigned int SensingBlocks::daysSince2000(VirtualMachine *vm)
{
    if (!clock)
        clock = Clock::instance().get();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock->currentSystemTime().time_since_epoch()).count();
    vm->addReturnValue(ms / 86400000.0 - 10957);

    return 0;
}
