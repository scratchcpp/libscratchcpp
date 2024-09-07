// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Compiler;
class VirtualMachine;
class IAudioInput;

/*! \brief The EventBlocks class contains the implementation of event blocks. */
class EventBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            TOUCHINGOBJECTMENU,
            BROADCAST_INPUT,
            VALUE
        };

        enum Fields
        {
            BROADCAST_OPTION,
            BACKDROP,
            WHENGREATERTHANMENU,
            KEY_OPTION
        };

        enum FieldValues
        {
            Loudness,
            Timer
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileWhenTouchingObjectPredicate(Compiler *compiler);
        static void compileWhenTouchingObject(Compiler *compiler);
        static void compileWhenFlagClicked(Compiler *compiler);
        static void compileWhenThisSpriteClicked(Compiler *compiler);
        static void compileWhenStageClicked(Compiler *compiler);
        static void compileBroadcast(Compiler *compiler);
        static void compileBroadcastAndWait(Compiler *compiler);
        static void compileWhenBroadcastReceived(Compiler *compiler);
        static void compileWhenBackdropSwitchesTo(Compiler *compiler);
        static void compileWhenGreaterThanPredicate(Compiler *compiler);
        static void compileWhenGreaterThan(Compiler *compiler);
        static void compileWhenKeyPressed(Compiler *compiler);

        static unsigned int whenTouchingObjectPredicate(VirtualMachine *vm);

        static unsigned int broadcast(VirtualMachine *vm);
        static unsigned int broadcastAndWait(VirtualMachine *vm);

        static unsigned int whenLoudnessGreaterThanPredicate(VirtualMachine *vm);
        static unsigned int whenTimerGreaterThanPredicate(VirtualMachine *vm);

        static IAudioInput *audioInput;
};

} // namespace libscratchcpp
