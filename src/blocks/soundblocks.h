// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Sound;
class Target;

/*! \brief The SoundBlocks class contains the implementation of sound blocks. */
class SoundBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            SOUND_MENU,
            VOLUME
        };

        enum Fields
        {

        };

        enum FieldValues
        {

        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compilePlay(Compiler *compiler);
        static void compileChangeVolumeBy(Compiler *compiler);
        static void compileSetVolumeTo(Compiler *compiler);
        static void compileVolume(Compiler *compiler);

        static Sound *getSoundByIndex(Target *target, long index);
        static unsigned int play(VirtualMachine *vm);
        static unsigned int playByIndex(VirtualMachine *vm);

        static unsigned int changeVolumeBy(VirtualMachine *vm);
        static unsigned int setVolumeTo(VirtualMachine *vm);
        static unsigned int volume(VirtualMachine *vm);
};

} // namespace libscratchcpp
