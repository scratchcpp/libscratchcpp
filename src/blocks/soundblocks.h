// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <unordered_set>

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class Sound;
class Target;

/*! \brief The SoundBlocks class contains the implementation of sound blocks. */
class SoundBlocks : public IExtension
{
    public:
        enum Inputs
        {
            SOUND_MENU,
            VALUE,
            VOLUME
        };

        enum Fields
        {
            EFFECT
        };

        enum FieldValues
        {
            PITCH,
            PAN
        };

        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

        static bool compilePlayCommon(Compiler *compiler, bool untilDone, bool *byIndex = nullptr);
        static void compilePlay(Compiler *compiler);
        static void compilePlayUntilDone(Compiler *compiler);
        static void compileStopAllSounds(Compiler *compiler);
        static void compileSetEffectTo(Compiler *compiler);
        static void compileChangeEffectBy(Compiler *compiler);
        static void compileClearEffects(Compiler *compiler);
        static void compileChangeVolumeBy(Compiler *compiler);
        static void compileSetVolumeTo(Compiler *compiler);
        static void compileVolume(Compiler *compiler);

        static const std::string &volumeMonitorName(Block *block);

        static Sound *getSoundByIndex(Target *target, long index);
        static Sound *playCommon(VirtualMachine *vm);
        static Sound *playByIndexCommon(VirtualMachine *vm);

        static unsigned int play(VirtualMachine *vm);
        static unsigned int playByIndex(VirtualMachine *vm);

        static unsigned int playUntilDone(VirtualMachine *vm);
        static unsigned int playByIndexUntilDone(VirtualMachine *vm);
        static unsigned int checkSound(VirtualMachine *vm);
        static unsigned int checkSoundByIndex(VirtualMachine *vm);

        static unsigned int stopAllSounds(VirtualMachine *vm);

        static unsigned int setPitchEffectTo(VirtualMachine *vm);
        static unsigned int setPanEffectTo(VirtualMachine *vm);
        static unsigned int changePitchEffectBy(VirtualMachine *vm);
        static unsigned int changePanEffectBy(VirtualMachine *vm);
        static unsigned int clearEffects(VirtualMachine *vm);

        static unsigned int changeVolumeBy(VirtualMachine *vm);
        static unsigned int setVolumeTo(VirtualMachine *vm);
        static unsigned int volume(VirtualMachine *vm);

        static inline std::unordered_map<Sound *, VirtualMachine *> m_waitingSounds;
};

} // namespace libscratchcpp
