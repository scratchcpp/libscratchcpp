// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/itimer.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
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
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
    engine->addCompileFunction(this, "sensing_resettimer", &compileResetTimer);
    engine->addCompileFunction(this, "sensing_current", &compileCurrent);
    engine->addCompileFunction(this, "sensing_dayssince2000", &compileDaysSince2000);

    // Inputs
    engine->addInput(this, "DISTANCETOMENU", DISTANCETOMENU);
    engine->addInput(this, "KEY_OPTION", KEY_OPTION);

    // Fields
    engine->addField(this, "CURRENTMENU", CURRENTMENU);

    // Field values
    engine->addFieldValue(this, "YEAR", YEAR);
    engine->addFieldValue(this, "MONTH", MONTH);
    engine->addFieldValue(this, "DATE", DATE);
    engine->addFieldValue(this, "DAYOFWEEK", DAYOFWEEK);
    engine->addFieldValue(this, "HOUR", HOUR);
    engine->addFieldValue(this, "MINUTE", MINUTE);
    engine->addFieldValue(this, "SECOND", SECOND);
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

void SensingBlocks::compileTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&timer);
}

void SensingBlocks::compileResetTimer(Compiler *compiler)
{
    compiler->addFunctionCall(&resetTimer);
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
