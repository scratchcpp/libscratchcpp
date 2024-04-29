// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include <scratchcpp/value.h>
#include <vector>

#include "../engine/internal/clock.h"

namespace libscratchcpp
{

class Target;
class IAudioInput;

/*! \brief The SensingBlocks class contains the implementation of sensing blocks. */
class SensingBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            TOUCHINGOBJECTMENU,
            COLOR,
            DISTANCETOMENU,
            QUESTION,
            KEY_OPTION,
            OBJECT
        };

        enum Fields
        {
            CURRENTMENU,
            DRAG_MODE,
            PROPERTY
        };

        enum FieldValues
        {
            YEAR,
            MONTH,
            DATE,
            DAYOFWEEK,
            HOUR,
            MINUTE,
            SECOND,
            Draggable,
            NotDraggable,
            XPosition,
            YPosition,
            Direction,
            CostumeNumber,
            CostumeName,
            Size,
            Volume,
            BackdropNumber,
            BackdropName
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileTouchingObject(Compiler *compiler);
        static void compileTouchingColor(Compiler *compiler);
        static void compileDistanceTo(Compiler *compiler);
        static void compileAskAndWait(Compiler *compiler);
        static void compileAnswer(Compiler *compiler);
        static void compileKeyPressed(Compiler *compiler);
        static void compileMouseDown(Compiler *compiler);
        static void compileMouseX(Compiler *compiler);
        static void compileMouseY(Compiler *compiler);
        static void compileSetDragMode(Compiler *compiler);
        static void compileLoudness(Compiler *compiler);
        static void compileLoud(Compiler *compiler);
        static void compileTimer(Compiler *compiler);
        static void compileResetTimer(Compiler *compiler);
        static void compileOf(Compiler *compiler);
        static void compileCurrent(Compiler *compiler);
        static void compileDaysSince2000(Compiler *compiler);

        static const std::string &mouseDownMonitorName(Block *block);
        static const std::string &mouseXMonitorName(Block *block);
        static const std::string &mouseYMonitorName(Block *block);
        static const std::string &loudnessMonitorName(Block *block);
        static const std::string &timerMonitorName(Block *block);
        static const std::string &currentMonitorName(Block *block);
        static const std::string &daysSince2000MonitorName(Block *block);

        static unsigned int touchingObject(VirtualMachine *vm);
        static unsigned int touchingObjectByIndex(VirtualMachine *vm);
        static unsigned int touchingMousePointer(VirtualMachine *vm);
        static unsigned int touchingEdge(VirtualMachine *vm);

        static unsigned int touchingColor(VirtualMachine *vm);

        static unsigned int keyPressed(VirtualMachine *vm);
        static unsigned int mouseDown(VirtualMachine *vm);
        static unsigned int mouseX(VirtualMachine *vm);
        static unsigned int mouseY(VirtualMachine *vm);

        static unsigned int setDraggableMode(VirtualMachine *vm);
        static unsigned int setNotDraggableMode(VirtualMachine *vm);

        static unsigned int loudness(VirtualMachine *vm);
        static unsigned int loud(VirtualMachine *vm);

        static unsigned int distanceTo(VirtualMachine *vm);
        static unsigned int distanceToByIndex(VirtualMachine *vm);
        static unsigned int distanceToMousePointer(VirtualMachine *vm);

        static void onAnswer(const std::string &answer);
        static unsigned int askAndWait(VirtualMachine *vm);
        static unsigned int answer(VirtualMachine *vm);

        static unsigned int timer(VirtualMachine *vm);
        static unsigned int resetTimer(VirtualMachine *vm);

        static unsigned int xPositionOfSprite(VirtualMachine *vm);
        static unsigned int xPositionOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int yPositionOfSprite(VirtualMachine *vm);
        static unsigned int yPositionOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int directionOfSprite(VirtualMachine *vm);
        static unsigned int directionOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int costumeNumberOfSprite(VirtualMachine *vm);
        static unsigned int costumeNumberOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int costumeNameOfSprite(VirtualMachine *vm);
        static unsigned int costumeNameOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int sizeOfSprite(VirtualMachine *vm);
        static unsigned int sizeOfSpriteByIndex(VirtualMachine *vm);
        static unsigned int volumeOfTarget(VirtualMachine *vm);
        static unsigned int volumeOfTargetByIndex(VirtualMachine *vm);
        static unsigned int variableOfTarget(VirtualMachine *vm);
        static unsigned int variableOfTargetByIndex(VirtualMachine *vm);
        static unsigned int backdropNumberOfStage(VirtualMachine *vm);
        static unsigned int backdropNumberOfStageByIndex(VirtualMachine *vm);
        static unsigned int backdropNameOfStage(VirtualMachine *vm);
        static unsigned int backdropNameOfStageByIndex(VirtualMachine *vm);

        static unsigned int currentYear(VirtualMachine *vm);
        static unsigned int currentMonth(VirtualMachine *vm);
        static unsigned int currentDate(VirtualMachine *vm);
        static unsigned int currentDayOfWeek(VirtualMachine *vm);
        static unsigned int currentHour(VirtualMachine *vm);
        static unsigned int currentMinute(VirtualMachine *vm);
        static unsigned int currentSecond(VirtualMachine *vm);
        static unsigned int daysSince2000(VirtualMachine *vm);

        static IClock *clock;
        static IAudioInput *audioInput;

    private:
        struct Question
        {
                Question(const std::string &question, VirtualMachine *vm, bool wasVisible, bool wasStage) :
                    question(question),
                    vm(vm),
                    wasVisible(wasVisible),
                    wasStage(wasStage)
                {
                }

                std::string question;
                VirtualMachine *vm = nullptr;
                bool wasVisible = false;
                bool wasStage = false;
        };

        static bool touchingObjectCommon(Target *source, Target *target);

        static void enqueueAsk(const std::string &question, VirtualMachine *vm);
        static void askNextQuestion();

        static inline std::vector<std::unique_ptr<Question>> m_questionList;
        static inline Value m_answer;
};

} // namespace libscratchcpp
