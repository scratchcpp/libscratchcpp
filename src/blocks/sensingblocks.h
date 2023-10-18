// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include "../engine/internal/clock.h"

namespace libscratchcpp
{

/*! \brief The SensingBlocks class contains the implementation of sensing blocks. */
class SensingBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            DISTANCETOMENU,
            KEY_OPTION
        };

        enum Fields
        {
            CURRENTMENU,
            DRAG_MODE
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
            NotDraggable
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileDistanceTo(Compiler *compiler);
        static void compileKeyPressed(Compiler *compiler);
        static void compileMouseDown(Compiler *compiler);
        static void compileMouseX(Compiler *compiler);
        static void compileMouseY(Compiler *compiler);
        static void compileSetDragMode(Compiler *compiler);
        static void compileTimer(Compiler *compiler);
        static void compileResetTimer(Compiler *compiler);
        static void compileCurrent(Compiler *compiler);
        static void compileDaysSince2000(Compiler *compiler);

        static unsigned int keyPressed(VirtualMachine *vm);
        static unsigned int mouseDown(VirtualMachine *vm);
        static unsigned int mouseX(VirtualMachine *vm);
        static unsigned int mouseY(VirtualMachine *vm);

        static unsigned int setDraggableMode(VirtualMachine *vm);
        static unsigned int setNotDraggableMode(VirtualMachine *vm);

        static unsigned int distanceTo(VirtualMachine *vm);
        static unsigned int distanceToByIndex(VirtualMachine *vm);
        static unsigned int distanceToMousePointer(VirtualMachine *vm);

        static unsigned int timer(VirtualMachine *vm);
        static unsigned int resetTimer(VirtualMachine *vm);
        static unsigned int currentYear(VirtualMachine *vm);
        static unsigned int currentMonth(VirtualMachine *vm);
        static unsigned int currentDate(VirtualMachine *vm);
        static unsigned int currentDayOfWeek(VirtualMachine *vm);
        static unsigned int currentHour(VirtualMachine *vm);
        static unsigned int currentMinute(VirtualMachine *vm);
        static unsigned int currentSecond(VirtualMachine *vm);
        static unsigned int daysSince2000(VirtualMachine *vm);

        static IClock *clock;
};

} // namespace libscratchcpp
