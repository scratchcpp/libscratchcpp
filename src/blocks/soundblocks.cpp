// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/target.h>
#include <algorithm>

#include "soundblocks.h"

using namespace libscratchcpp;

std::string SoundBlocks::name() const
{
    return "Sound";
}

std::string SoundBlocks::description() const
{
    return name() + " blocks";
}

Rgb SoundBlocks::color() const
{
    return rgb(207, 99, 207);
}

void SoundBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "sound_play", &compilePlay);
    engine->addCompileFunction(this, "sound_playuntildone", &compilePlayUntilDone);
}

CompilerValue *SoundBlocks::compilePlay(Compiler *compiler)
{
    auto sound = compiler->addInput("SOUND_MENU");
    compiler->addTargetFunctionCall("sound_play", Compiler::StaticType::Pointer, { Compiler::StaticType::Unknown }, { sound });
    return nullptr;
}

CompilerValue *SoundBlocks::compilePlayUntilDone(Compiler *compiler)
{
    auto sound = compiler->addInput("SOUND_MENU");
    auto soundPtr = compiler->addTargetFunctionCall("sound_play", Compiler::StaticType::Pointer, { Compiler::StaticType::Unknown }, { sound });

    compiler->beginLoopCondition();
    auto numType = Compiler::StaticType::Number;
    auto playing = compiler->addFunctionCall("sound_is_playing", Compiler::StaticType::Bool, { Compiler::StaticType::Pointer }, { soundPtr });
    compiler->beginWhileLoop(playing);
    compiler->endLoop();

    return nullptr;
}

int sound_wrap_clamp_index(Target *target, int index)
{
    const long soundCount = target->sounds().size();

    if (index < 0)
        return (soundCount + index % (-soundCount)) % soundCount;
    else if (index >= soundCount)
        return index % soundCount;
    else
        return index;
}

int sound_get_index(Target *target, const ValueData *sound)
{
    if (!value_isString(sound)) {
        // Numbers should be treated as sound indices
        if (value_isNaN(sound) || value_isInfinity(sound) || value_isNegativeInfinity(sound))
            return -1;
        else
            return sound_wrap_clamp_index(target, value_toLong(sound) - 1);
    } else {
        // Strings should be treated as sound names, where possible
        // TODO: Use UTF-16 in Target
        // StringPtr *nameStr = value_toStringPtr(sound);
        std::string nameStr;
        value_toString(sound, &nameStr);
        const int soundIndex = target->findSound(nameStr);

        auto it = std::find_if(nameStr.begin(), nameStr.end(), [](char c) { return !std::isspace(c); });
        bool isWhiteSpace = (it == nameStr.end());

        if (soundIndex != -1) {
            return soundIndex;
            // Try to cast the string to a number (and treat it as a costume index)
            // Pure whitespace should not be treated as a number
        } else if (value_isValidNumber(sound) && !isWhiteSpace)
            return sound_wrap_clamp_index(target, value_toLong(sound) - 1);
    }

    return -1;
}

extern "C" Sound *sound_play(Target *target, const ValueData *soundName)
{
    int index = sound_get_index(target, soundName);
    auto sound = target->soundAt(index);

    if (sound) {
        sound->start();
        return sound.get();
    }

    return nullptr;
}

extern "C" bool sound_is_playing(Sound *sound)
{
    if (!sound)
        return false;

    return sound->isPlaying();
}
