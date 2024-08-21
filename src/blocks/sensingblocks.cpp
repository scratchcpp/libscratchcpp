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
#include <scratchcpp/variable.h>
#include <scratchcpp/block.h>
#include "sensingblocks.h"

#include "../engine/internal/clock.h"
#include "audio/audioinput.h"
#include "audio/iaudioloudness.h"

using namespace libscratchcpp;

IClock *SensingBlocks::clock = nullptr;
IAudioInput *SensingBlocks::audioInput = nullptr;

std::string SensingBlocks::name() const
{
    return "Sensing";
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sensing_touchingobject", &compileTouchingObject);
    engine->addCompileFunction(this, "sensing_touchingcolor", &compileTouchingColor);
    engine->addCompileFunction(this, "sensing_distanceto", &compileDistanceTo);
    engine->addCompileFunction(this, "sensing_askandwait", &compileAskAndWait);
    engine->addCompileFunction(this, "sensing_answer", &compileAnswer);
    engine->addCompileFunction(this, "sensing_keypressed", &compileKeyPressed);
    engine->addCompileFunction(this, "sensing_mousedown", &compileMouseDown);
    engine->addCompileFunction(this, "sensing_mousex", &compileMouseX);
    engine->addCompileFunction(this, "sensing_mousey", &compileMouseY);
    engine->addCompileFunction(this, "sensing_setdragmode", &compileSetDragMode);
    engine->addCompileFunction(this, "sensing_loudness", &compileLoudness);
    engine->addCompileFunction(this, "sensing_loud", &compileLoud);
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
    engine->addCompileFunction(this, "sensing_resettimer", &compileResetTimer);
    engine->addCompileFunction(this, "sensing_of", &compileOf);
    engine->addCompileFunction(this, "sensing_current", &compileCurrent);
    engine->addCompileFunction(this, "sensing_dayssince2000", &compileDaysSince2000);

    // Monitor names
    engine->addMonitorNameFunction(this, "sensing_mousedown", &mouseDownMonitorName);
    engine->addMonitorNameFunction(this, "sensing_mousex", &mouseXMonitorName);
    engine->addMonitorNameFunction(this, "sensing_mousey", &mouseYMonitorName);
    engine->addMonitorNameFunction(this, "sensing_loudness", &loudnessMonitorName);
    engine->addMonitorNameFunction(this, "sensing_timer", &timerMonitorName);
    engine->addMonitorNameFunction(this, "sensing_current", &currentMonitorName);
    engine->addMonitorNameFunction(this, "sensing_dayssince2000", &daysSince2000MonitorName);

    // Inputs
    engine->addInput(this, "TOUCHINGOBJECTMENU", TOUCHINGOBJECTMENU);
    engine->addInput(this, "COLOR", COLOR);
    engine->addInput(this, "DISTANCETOMENU", DISTANCETOMENU);
    engine->addInput(this, "QUESTION", QUESTION);
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

    // Callbacks
    engine->questionAnswered().connect(&onAnswer);
}

void SensingBlocks::onInit(IEngine *engine)
{
    engine->threadAboutToStop().connect([engine](VirtualMachine *thread) {
        if (!m_questionList.empty()) {
            // Abort the question of this thread if it's currently being displayed
            if (m_questionList.front()->vm == thread) {
                thread->target()->setBubbleText("");
                engine->questionAborted()();
            }

            m_questionList
                .erase(std::remove_if(m_questionList.begin(), m_questionList.end(), [thread](const std::unique_ptr<Question> &question) { return question->vm == thread; }), m_questionList.end());
        }
    });
}

void SensingBlocks::compileTouchingObject(Compiler *compiler)
{
    Input *input = compiler->input(TOUCHINGOBJECTMENU);

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addFunctionCall(&touchingMousePointer);
        else if (value == "_edge_")
            compiler->addFunctionCall(&touchingEdge);
        else {
            int index = compiler->engine()->findTarget(value);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&touchingObjectByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&touchingObject);
    }
}

void SensingBlocks::compileTouchingColor(Compiler *compiler)
{
    compiler->addInput(COLOR);
    compiler->addFunctionCall(&touchingColor);
}

void SensingBlocks::compileDistanceTo(Compiler *compiler)
{
    Input *input = compiler->input(DISTANCETOMENU);

    if (input->pointsToDropdownMenu()) {
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

void SensingBlocks::compileAskAndWait(Compiler *compiler)
{
    compiler->addInput(QUESTION);
    compiler->addFunctionCall(&askAndWait);
}

void SensingBlocks::compileAnswer(Compiler *compiler)
{
    compiler->addFunctionCall(&answer);
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

void SensingBlocks::compileLoudness(Compiler *compiler)
{
    compiler->addFunctionCall(&loudness);
}

void SensingBlocks::compileLoud(Compiler *compiler)
{
    compiler->addFunctionCall(&loud);
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
    Field *property = compiler->field(PROPERTY);
    assert(property);
    int option = property->specialValueId();
    Input *input = compiler->input(OBJECT);
    assert(input);
    BlockFunc f = nullptr;

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        IEngine *engine = compiler->engine();
        assert(engine);
        int index = engine->findTarget(value);

        if (index == -1) {
            compiler->addInstruction(vm::OP_NULL);
            return;
        }

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

            default: {
                // Variable
                f = &variableOfTargetByIndex;
                Target *target = engine->targetAt(index);
                auto varIndex = target->findVariable(property->value().toString());

                if (varIndex == -1)
                    compiler->addInstruction(vm::OP_NULL);
                else {
                    // NOTE: The OP_READ_VAR instruction can't be used for this (see #548)
                    compiler->addConstValue(varIndex);
                }

                break;
            }
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
                f = &variableOfTarget;
                compiler->addConstValue(property->value().toString());
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

const std::string &SensingBlocks::mouseDownMonitorName(Block *block)
{
    static const std::string name = "mouse down?";
    return name;
}

const std::string &SensingBlocks::mouseXMonitorName(Block *block)
{
    static const std::string name = "mouse x";
    return name;
}

const std::string &SensingBlocks::mouseYMonitorName(Block *block)
{
    static const std::string name = "mouse y";
    return name;
}

const std::string &SensingBlocks::loudnessMonitorName(Block *block)
{
    static const std::string name = "loudness";
    return name;
}

const std::string &SensingBlocks::timerMonitorName(Block *block)
{
    static const std::string name = "timer";
    return name;
}

const std::string &SensingBlocks::currentMonitorName(Block *block)
{
    int id = block->findFieldById(CURRENTMENU)->specialValueId();

    switch (id) {
        case YEAR: {
            static const std::string name = "year";
            return name;
        }

        case MONTH: {
            static const std::string name = "month";
            return name;
        }

        case DATE: {
            static const std::string name = "date";
            return name;
        }

        case DAYOFWEEK: {
            static const std::string name = "day of week";
            return name;
        }

        case HOUR: {
            static const std::string name = "hour";
            return name;
        }

        case MINUTE: {
            static const std::string name = "minute";
            return name;
        }

        case SECOND: {
            static const std::string name = "second";
            return name;
        }

        default: {
            static const std::string name = "";
            return name;
        }
    }
}

const std::string &SensingBlocks::daysSince2000MonitorName(Block *block)
{
    static const std::string name = "days since 2000";
    return name;
}

unsigned int SensingBlocks::touchingObject(VirtualMachine *vm)
{
    std::string value = vm->getInput(0, 1)->toString();

    if (value == "_mouse_")
        vm->replaceReturnValue(vm->target()->touchingPoint(vm->engine()->mouseX(), vm->engine()->mouseY()), 1);
    else if (value == "_edge_")
        vm->replaceReturnValue(vm->target()->touchingEdge(), 1);
    else {
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));
        vm->replaceReturnValue(touchingObjectCommon(vm->target(), target), 1);
    }

    return 0;
}

unsigned int SensingBlocks::touchingObjectByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    vm->replaceReturnValue(touchingObjectCommon(vm->target(), target), 1);
    return 0;
}

unsigned int SensingBlocks::touchingMousePointer(VirtualMachine *vm)
{
    vm->addReturnValue(vm->target()->touchingPoint(vm->engine()->mouseX(), vm->engine()->mouseY()));
    return 0;
}

unsigned int SensingBlocks::touchingEdge(VirtualMachine *vm)
{
    vm->addReturnValue(vm->target()->touchingEdge());
    return 0;
}

unsigned int SensingBlocks::touchingColor(VirtualMachine *vm)
{
    vm->replaceReturnValue(vm->target()->touchingColor(*vm->getInput(0, 1)), 1);
    return 0;
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

unsigned int SensingBlocks::loudness(VirtualMachine *vm)
{
    if (!audioInput)
        audioInput = AudioInput::instance().get();

    auto audioLoudness = audioInput->audioLoudness();
    vm->addReturnValue(audioLoudness->getLoudness());
    return 0;
}

unsigned int SensingBlocks::loud(VirtualMachine *vm)
{
    if (!audioInput)
        audioInput = AudioInput::instance().get();

    auto audioLoudness = audioInput->audioLoudness();
    vm->addReturnValue(audioLoudness->getLoudness() > 10.0);
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

void SensingBlocks::onAnswer(const std::string &answer)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L99-L115
    m_answer = answer;

    if (!m_questionList.empty()) {
        Question *question = m_questionList.front().get();
        VirtualMachine *vm = question->vm;
        assert(vm);
        assert(vm->target());

        // If the target was visible when asked, hide the say bubble unless the target was the stage
        if (question->wasVisible && !question->wasStage)
            vm->target()->setBubbleText("");

        m_questionList.erase(m_questionList.begin());
        vm->resolvePromise();
        askNextQuestion();
    }
}

unsigned int SensingBlocks::askAndWait(VirtualMachine *vm)
{
    const bool isQuestionAsked = !m_questionList.empty();
    enqueueAsk(vm->getInput(0, 1)->toString(), vm);

    if (!isQuestionAsked)
        askNextQuestion();

    vm->promise();
    return 1;
}

unsigned int SensingBlocks::answer(VirtualMachine *vm)
{
    vm->addReturnValue(m_answer);
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

unsigned int SensingBlocks::variableOfTarget(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->engine()->findTarget(vm->getInput(1, 2)->toString()));

    if (target) {
        auto varIndex = target->findVariable(vm->getInput(0, 2)->toString());

        if (varIndex == -1)
            vm->replaceReturnValue(0, 2);
        else
            vm->replaceReturnValue(target->variableAt(varIndex)->value(), 2);
    } else
        vm->replaceReturnValue(0, 2);

    return 1;
}

unsigned int SensingBlocks::variableOfTargetByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());

    if (target) {
        const int varIndex = vm->getInput(0, 2)->toInt();
        vm->replaceReturnValue(target->variableAt(varIndex)->value(), 2);
    } else
        vm->replaceReturnValue(0, 2);

    return 1;
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

bool SensingBlocks::touchingObjectCommon(Target *source, Target *target)
{
    if (source && target && !target->isStage())
        return source->touchingSprite(static_cast<Sprite *>(target));

    return false;
}

void SensingBlocks::enqueueAsk(const std::string &question, VirtualMachine *vm)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L117-L119
    assert(vm);
    Target *target = vm->target();
    assert(target);
    bool visible = true;
    bool isStage = target->isStage();

    if (!isStage) {
        Sprite *sprite = static_cast<Sprite *>(target);
        visible = sprite->visible();
    }

    m_questionList.push_back(std::make_unique<Question>(question, vm, visible, isStage));
}

void SensingBlocks::askNextQuestion()
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L121-L133
    if (m_questionList.empty())
        return;

    Question *question = m_questionList.front().get();
    Target *target = question->vm->target();
    IEngine *engine = question->vm->engine();

    // If the target is visible, emit a blank question and show
    // a bubble unless the target was the stage
    if (question->wasVisible && !question->wasStage) {
        target->setBubbleType(Target::BubbleType::Say);
        target->setBubbleText(question->question);

        engine->questionAsked()("");
    } else {
        engine->questionAsked()(question->question);
    }
}
