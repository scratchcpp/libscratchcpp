// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/field.h>
#include <scratchcpp/target.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/value.h>
#include <scratchcpp/executioncontext.h>
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
    engine->addCompileFunction(this, "sound_stopallsounds", &compileStopAllSounds);
    engine->addCompileFunction(this, "sound_seteffectto", &compileSetEffectTo);
    engine->addCompileFunction(this, "sound_changeeffectby", &compileChangeEffectBy);
}

void SoundBlocks::onInit(IEngine *engine)
{
    engine->threadAboutToStop().connect([](Thread *thread) {
        Target *target = thread->target();
        const auto &sounds = target->sounds();

        for (auto sound : sounds) {
            if (sound->owner() == thread) {
                sound->stop();
                break;
            }
        }
    });
}

CompilerValue *SoundBlocks::compilePlay(Compiler *compiler)
{
    auto sound = compiler->addInput("SOUND_MENU");
    auto storeOwner = compiler->addConstValue(false);
    compiler->addFunctionCallWithCtx("sound_play", Compiler::StaticType::Pointer, { Compiler::StaticType::Unknown, Compiler::StaticType::Bool }, { sound, storeOwner });
    return nullptr;
}

CompilerValue *SoundBlocks::compilePlayUntilDone(Compiler *compiler)
{
    auto sound = compiler->addInput("SOUND_MENU");
    auto storeOwner = compiler->addConstValue(true);
    auto soundPtr = compiler->addFunctionCallWithCtx("sound_play", Compiler::StaticType::Pointer, { Compiler::StaticType::Unknown, Compiler::StaticType::Bool }, { sound, storeOwner });

    compiler->beginLoopCondition();
    auto waiting = compiler->addFunctionCallWithCtx("sound_is_waiting", Compiler::StaticType::Bool, { Compiler::StaticType::Pointer }, { soundPtr });
    compiler->beginWhileLoop(waiting);
    compiler->endLoop();

    return nullptr;
}

CompilerValue *SoundBlocks::compileStopAllSounds(Compiler *compiler)
{
    compiler->addFunctionCallWithCtx("sound_stopallsounds");
    return nullptr;
}

CompilerValue *SoundBlocks::compileSetEffectTo(Compiler *compiler)
{
    Field *field = compiler->field("EFFECT");

    if (!field)
        return nullptr;

    const std::string &option = field->value().toString();

    if (option == "PITCH") {
        auto value = compiler->addInput("VALUE");
        compiler->addTargetFunctionCall("sound_set_pitch_effect", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { value });
    } else if (option == "PAN") {
        auto value = compiler->addInput("VALUE");
        compiler->addTargetFunctionCall("sound_set_pan_effect", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { value });
    }

    return nullptr;
}

CompilerValue *SoundBlocks::compileChangeEffectBy(Compiler *compiler)
{
    Field *field = compiler->field("EFFECT");

    if (!field)
        return nullptr;

    const std::string &option = field->value().toString();

    if (option == "PITCH") {
        auto value = compiler->addInput("VALUE");
        compiler->addTargetFunctionCall("sound_change_pitch_effect", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { value });
    } else if (option == "PAN") {
        auto value = compiler->addInput("VALUE");
        compiler->addTargetFunctionCall("sound_change_pan_effect", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { value });
    }

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

extern "C" Sound *sound_play(ExecutionContext *ctx, const ValueData *soundName, bool storeOwner)
{
    Thread *thread = ctx->thread();
    Target *target = thread->target();
    int index = sound_get_index(target, soundName);
    auto sound = target->soundAt(index);

    if (sound) {
        sound->start(storeOwner ? thread : nullptr);
        return sound.get();
    }

    return nullptr;
}

extern "C" bool sound_is_waiting(ExecutionContext *ctx, Sound *sound)
{
    if (!sound)
        return false;

    return sound->owner() == ctx->thread() && sound->isPlaying();
}

extern "C" void sound_stopallsounds(ExecutionContext *ctx)
{
    ctx->engine()->stopSounds();
}

extern "C" void sound_set_pitch_effect(Target *target, double value)
{
    target->setSoundEffectValue(Sound::Effect::Pitch, value);
}

extern "C" void sound_set_pan_effect(Target *target, double value)
{
    target->setSoundEffectValue(Sound::Effect::Pan, value);
}

extern "C" void sound_change_pitch_effect(Target *target, double value)
{
    target->setSoundEffectValue(Sound::Effect::Pitch, target->soundEffectValue(Sound::Effect::Pitch) + value);
}

extern "C" void sound_change_pan_effect(Target *target, double value)
{
    target->setSoundEffectValue(Sound::Effect::Pan, target->soundEffectValue(Sound::Effect::Pan) + value);
}
