// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/block.h>
#include <scratchcpp/scratchconfiguration.h>

#include "looksblocks.h"
#include "../engine/internal/randomgenerator.h"
#include "../engine/internal/clock.h"

using namespace libscratchcpp;

IRandomGenerator *LooksBlocks::rng = nullptr;
IClock *LooksBlocks::clock = nullptr;

// TODO: Use C++20
template<typename ContainerT, typename PredicateT>
void erase_if(ContainerT &items, const PredicateT &predicate)
{
    for (auto it = items.begin(); it != items.end();) {
        if (predicate(*it))
            it = items.erase(it);
        else
            ++it;
    }
}

std::string LooksBlocks::name() const
{
    return "Looks";
}

std::string LooksBlocks::description() const
{
    return name() + " blocks";
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "looks_sayforsecs", &compileSayForSecs);
    engine->addCompileFunction(this, "looks_say", &compileSay);
    engine->addCompileFunction(this, "looks_thinkforsecs", &compileThinkForSecs);
    engine->addCompileFunction(this, "looks_think", &compileThink);
    engine->addCompileFunction(this, "looks_show", &compileShow);
    engine->addCompileFunction(this, "looks_hide", &compileHide);
    engine->addCompileFunction(this, "looks_changeeffectby", &compileChangeEffectBy);
    engine->addCompileFunction(this, "looks_seteffectto", &compileSetEffectTo);
    engine->addCompileFunction(this, "looks_cleargraphiceffects", &compileClearGraphicEffects);
    engine->addCompileFunction(this, "looks_changesizeby", &compileChangeSizeBy);
    engine->addCompileFunction(this, "looks_setsizeto", &compileSetSizeTo);
    engine->addCompileFunction(this, "looks_size", &compileSize);
    engine->addCompileFunction(this, "looks_switchcostumeto", &compileSwitchCostumeTo);
    engine->addCompileFunction(this, "looks_nextcostume", &compileNextCostume);
    engine->addCompileFunction(this, "looks_switchbackdropto", &compileSwitchBackdropTo);
    engine->addCompileFunction(this, "looks_switchbackdroptoandwait", &compileSwitchBackdropToAndWait);
    engine->addCompileFunction(this, "looks_nextbackdrop", &compileNextBackdrop);
    engine->addCompileFunction(this, "looks_gotofrontback", &compileGoToFrontBack);
    engine->addCompileFunction(this, "looks_goforwardbackwardlayers", &compileGoForwardBackwardLayers);
    engine->addCompileFunction(this, "looks_costumenumbername", &compileCostumeNumberName);
    engine->addCompileFunction(this, "looks_backdropnumbername", &compileBackdropNumberName);

    // Monitor names
    engine->addMonitorNameFunction(this, "looks_costumenumbername", &costumeNumberNameMonitorName);
    engine->addMonitorNameFunction(this, "looks_backdropnumbername", &backdropNumberNameMonitorName);
    engine->addMonitorNameFunction(this, "looks_size", &sizeMonitorName);

    // Inputs
    engine->addInput(this, "MESSAGE", MESSAGE);
    engine->addInput(this, "SECS", SECS);
    engine->addInput(this, "CHANGE", CHANGE);
    engine->addInput(this, "SIZE", SIZE);
    engine->addInput(this, "COSTUME", COSTUME);
    engine->addInput(this, "BACKDROP", BACKDROP);
    engine->addInput(this, "VALUE", VALUE);

    // Fields
    engine->addField(this, "NUMBER_NAME", NUMBER_NAME);
    engine->addField(this, "EFFECT", EFFECT);
    engine->addField(this, "FRONT_BACK", FRONT_BACK);
    engine->addField(this, "FORWARD_BACKWARD", FORWARD_BACKWARD);

    // Field values
    engine->addFieldValue(this, "number", Number);
    engine->addFieldValue(this, "name", Name);
    engine->addFieldValue(this, "COLOR", ColorEffect);
    engine->addFieldValue(this, "FISHEYE", FisheyeEffect);
    engine->addFieldValue(this, "WHIRL", WhirlEffect);
    engine->addFieldValue(this, "PIXELATE", PixelateEffect);
    engine->addFieldValue(this, "MOSAIC", MosaicEffect);
    engine->addFieldValue(this, "BRIGHTNESS", BrightnessEffect);
    engine->addFieldValue(this, "GHOST", GhostEffect);
    engine->addFieldValue(this, "front", Front);
    engine->addFieldValue(this, "back", Back);
    engine->addFieldValue(this, "forward", Forward);
    engine->addFieldValue(this, "backward", Backward);
}

void LooksBlocks::onInit(IEngine *engine)
{
    engine->threadAboutToStop().connect([](Thread *thread) {
        m_timeMap.erase(thread->vm());
        erase_if(m_waitingBubbles, [thread](const std::pair<Target *, VirtualMachine *> &pair) { return pair.second == thread->vm(); });
    });

    engine->stopped().connect([engine]() {
        const auto &targets = engine->targets();

        for (auto target : targets) {
            target->setBubbleText("");
            target->clearGraphicsEffects();
        }
    });
}

void LooksBlocks::compileSayForSecs(Compiler *compiler)
{
    compiler->addInput(MESSAGE);
    compiler->addInput(SECS);
    compiler->addFunctionCall(&startSayForSecs);
    compiler->addFunctionCall(&sayForSecs);
}

void LooksBlocks::compileSay(Compiler *compiler)
{
    compiler->addInput(MESSAGE);
    compiler->addFunctionCall(&say);
}

void LooksBlocks::compileThinkForSecs(Compiler *compiler)
{
    compiler->addInput(MESSAGE);
    compiler->addInput(SECS);
    compiler->addFunctionCall(&startThinkForSecs);
    compiler->addFunctionCall(&thinkForSecs);
}

void LooksBlocks::compileThink(Compiler *compiler)
{
    compiler->addInput(MESSAGE);
    compiler->addFunctionCall(&think);
}

void LooksBlocks::compileShow(Compiler *compiler)
{
    compiler->addFunctionCall(&show);
}

void LooksBlocks::compileHide(Compiler *compiler)
{
    compiler->addFunctionCall(&hide);
}

void LooksBlocks::compileChangeEffectBy(Compiler *compiler)
{
    int option = compiler->field(EFFECT)->specialValueId();

    switch (option) {
        case ColorEffect:
            if (!m_colorEffect)
                m_colorEffect = ScratchConfiguration::getGraphicsEffect("color");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeColorEffectBy);
            break;

        case FisheyeEffect:
            if (!m_fisheyeEffect)
                m_fisheyeEffect = ScratchConfiguration::getGraphicsEffect("fisheye");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeFisheyeEffectBy);
            break;

        case WhirlEffect:
            if (!m_whirlEffect)
                m_whirlEffect = ScratchConfiguration::getGraphicsEffect("whirl");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeWhirlEffectBy);
            break;

        case PixelateEffect:
            if (!m_pixelateEffect)
                m_pixelateEffect = ScratchConfiguration::getGraphicsEffect("pixelate");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changePixelateEffectBy);
            break;

        case MosaicEffect:
            if (!m_mosaicEffect)
                m_mosaicEffect = ScratchConfiguration::getGraphicsEffect("mosaic");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeMosaicEffectBy);
            break;

        case BrightnessEffect:
            if (!m_brightnessEffect)
                m_brightnessEffect = ScratchConfiguration::getGraphicsEffect("brightness");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeBrightnessEffectBy);
            break;

        case GhostEffect:
            if (!m_ghostEffect)
                m_ghostEffect = ScratchConfiguration::getGraphicsEffect("ghost");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&changeGhostEffectBy);
            break;

        default:
            IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect(compiler->field(EFFECT)->value().toString());

            if (effect) {
                auto it = std::find(m_customGraphicsEffects.begin(), m_customGraphicsEffects.end(), effect);
                size_t index;

                if (it == m_customGraphicsEffects.end()) {
                    index = m_customGraphicsEffects.size();
                    m_customGraphicsEffects.push_back(effect);
                } else
                    index = it - m_customGraphicsEffects.begin();

                compiler->addConstValue(index);
                compiler->addInput(CHANGE);
                compiler->addFunctionCall(&changeEffectBy);
            }

            break;
    }
}

void LooksBlocks::compileSetEffectTo(Compiler *compiler)
{
    int option = compiler->field(EFFECT)->specialValueId();

    switch (option) {
        case ColorEffect:
            if (!m_colorEffect)
                m_colorEffect = ScratchConfiguration::getGraphicsEffect("color");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setColorEffectTo);
            break;

        case FisheyeEffect:
            if (!m_fisheyeEffect)
                m_fisheyeEffect = ScratchConfiguration::getGraphicsEffect("fisheye");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setFisheyeEffectTo);
            break;

        case WhirlEffect:
            if (!m_whirlEffect)
                m_whirlEffect = ScratchConfiguration::getGraphicsEffect("whirl");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setWhirlEffectTo);
            break;

        case PixelateEffect:
            if (!m_pixelateEffect)
                m_pixelateEffect = ScratchConfiguration::getGraphicsEffect("pixelate");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setPixelateEffectTo);
            break;

        case MosaicEffect:
            if (!m_mosaicEffect)
                m_mosaicEffect = ScratchConfiguration::getGraphicsEffect("mosaic");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setMosaicEffectTo);
            break;

        case BrightnessEffect:
            if (!m_brightnessEffect)
                m_brightnessEffect = ScratchConfiguration::getGraphicsEffect("brightness");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setBrightnessEffectTo);
            break;

        case GhostEffect:
            if (!m_ghostEffect)
                m_ghostEffect = ScratchConfiguration::getGraphicsEffect("ghost");

            compiler->addInput(VALUE);
            compiler->addFunctionCall(&setGhostEffectTo);
            break;

        default:
            IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect(compiler->field(EFFECT)->value().toString());

            if (effect) {
                auto it = std::find(m_customGraphicsEffects.begin(), m_customGraphicsEffects.end(), effect);
                size_t index;

                if (it == m_customGraphicsEffects.end()) {
                    index = m_customGraphicsEffects.size();
                    m_customGraphicsEffects.push_back(effect);
                } else
                    index = it - m_customGraphicsEffects.begin();

                compiler->addConstValue(index);
                compiler->addInput(VALUE);
                compiler->addFunctionCall(&setEffectTo);
            }

            break;
    }
}

void LooksBlocks::compileClearGraphicEffects(Compiler *compiler)
{
    compiler->addFunctionCall(&clearGraphicEffects);
}

void LooksBlocks::compileChangeSizeBy(Compiler *compiler)
{
    compiler->addInput(CHANGE);
    compiler->addFunctionCall(&changeSizeBy);
}

void LooksBlocks::compileSetSizeTo(Compiler *compiler)
{
    compiler->addInput(SIZE);
    compiler->addFunctionCall(&setSizeTo);
}

void LooksBlocks::compileSize(Compiler *compiler)
{
    compiler->addFunctionCall(&size);
}

void LooksBlocks::compileSwitchCostumeTo(Compiler *compiler)
{
    Target *target = compiler->target();

    if (!target)
        return;

    compiler->addInput(COSTUME);
    compiler->addFunctionCall(&switchCostumeTo);
}

void LooksBlocks::compileNextCostume(Compiler *compiler)
{
    compiler->addFunctionCall(&nextCostume);
}

void LooksBlocks::compileSwitchBackdropTo(Compiler *compiler)
{
    compiler->addInput(BACKDROP);
    compiler->addFunctionCall(&switchBackdropTo);
}

void LooksBlocks::compileSwitchBackdropToAndWait(Compiler *compiler)
{
    compiler->addInput(BACKDROP);
    compiler->addFunctionCall(&switchBackdropToAndWait);
}

void LooksBlocks::compileNextBackdrop(Compiler *compiler)
{
    compiler->addFunctionCall(&nextBackdrop);
}

void LooksBlocks::compileGoToFrontBack(Compiler *compiler)
{
    int option = compiler->field(FRONT_BACK)->specialValueId();

    switch (option) {
        case Front:
            compiler->addFunctionCall(&goToFront);
            break;

        case Back:
            compiler->addFunctionCall(&goToBack);
            break;
    }
}

void LooksBlocks::compileGoForwardBackwardLayers(Compiler *compiler)
{
    int option = compiler->field(FORWARD_BACKWARD)->specialValueId();

    switch (option) {
        case Forward:
            compiler->addInput(NUM);
            compiler->addFunctionCall(&goForwardLayers);
            break;

        case Backward:
            compiler->addInput(NUM);
            compiler->addFunctionCall(&goBackwardLayers);
            break;
    }
}

void LooksBlocks::compileCostumeNumberName(Compiler *compiler)
{
    int option = compiler->field(NUMBER_NAME)->specialValueId();

    switch (option) {
        case Number:
            compiler->addFunctionCall(&costumeNumber);
            break;

        case Name:
            compiler->addFunctionCall(&costumeName);
            break;
    }
}

void LooksBlocks::compileBackdropNumberName(Compiler *compiler)
{
    int option = compiler->field(NUMBER_NAME)->specialValueId();

    switch (option) {
        case Number:
            compiler->addFunctionCall(&backdropNumber);
            break;

        case Name:
            compiler->addFunctionCall(&backdropName);
            break;
    }
}

const std::string &LooksBlocks::costumeNumberNameMonitorName(Block *block)
{
    int option = block->findFieldById(NUMBER_NAME)->specialValueId();

    switch (option) {
        case Number: {
            static const std::string name = "costume number";
            return name;
        }

        case Name: {
            static const std::string name = "costume name";
            return name;
        }

        default: {
            static const std::string name = "";
            return name;
        }
    }
}

const std::string &LooksBlocks::backdropNumberNameMonitorName(Block *block)
{
    int option = block->findFieldById(NUMBER_NAME)->specialValueId();

    switch (option) {
        case Number: {
            static const std::string name = "backdrop number";
            return name;
        }

        case Name: {
            static const std::string name = "backdrop name";
            return name;
        }

        default: {
            static const std::string name = "";
            return name;
        }
    }
}

const std::string &LooksBlocks::sizeMonitorName(Block *block)
{
    static const std::string name = "size";
    return name;
}

void LooksBlocks::startWait(VirtualMachine *vm, double secs)
{
    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    m_timeMap[vm] = { currentTime, secs * 1000 };
    vm->engine()->requestRedraw();
}

bool LooksBlocks::wait(VirtualMachine *vm)
{
    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    assert(m_timeMap.count(vm) == 1);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_timeMap[vm].first).count() >= m_timeMap[vm].second) {
        m_timeMap.erase(vm);
        vm->stop(true, true, false);
        return true;
    } else {
        vm->stop(true, true, true);
        return false;
    }
}

void LooksBlocks::showBubble(VirtualMachine *vm, Target::BubbleType type, const std::string &text)
{
    Target *target = vm->target();

    if (target) {
        target->setBubbleType(type);
        target->setBubbleText(text);
        m_waitingBubbles.erase(target);
    }
}

void LooksBlocks::hideBubble(Target *target)
{
    if (!target)
        return;

    target->setBubbleText("");
    m_waitingBubbles.erase(target);
}

unsigned int LooksBlocks::startSayForSecs(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target) {
        showBubble(vm, Target::BubbleType::Say, vm->getInput(0, 2)->toString());
        m_waitingBubbles[target] = vm;
        startWait(vm, vm->getInput(1, 2)->toDouble());
    }

    return 2;
}

unsigned int LooksBlocks::sayForSecs(VirtualMachine *vm)
{
    if (wait(vm)) {
        Target *target = vm->target();

        if (target) {
            auto it = m_waitingBubbles.find(target);

            // Clear bubble if it hasn't been changed
            if (it != m_waitingBubbles.cend() && it->second == vm)
                hideBubble(vm->target());
        }
    }

    return 0;
}

unsigned int LooksBlocks::say(VirtualMachine *vm)
{
    showBubble(vm, Target::BubbleType::Say, vm->getInput(0, 1)->toString());
    return 1;
}

unsigned int LooksBlocks::startThinkForSecs(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target) {
        showBubble(vm, Target::BubbleType::Think, vm->getInput(0, 2)->toString());
        m_waitingBubbles[target] = vm;
        startWait(vm, vm->getInput(1, 2)->toDouble());
    }

    return 2;
}

unsigned int LooksBlocks::thinkForSecs(VirtualMachine *vm)
{
    return sayForSecs(vm); // there isn't any difference
}

unsigned int LooksBlocks::think(VirtualMachine *vm)
{
    showBubble(vm, Target::BubbleType::Think, vm->getInput(0, 1)->toString());
    return 1;
}

unsigned int LooksBlocks::show(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setVisible(true);

    return 0;
}

unsigned int LooksBlocks::hide(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setVisible(false);

    return 0;
}

unsigned int LooksBlocks::changeEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target) {
        IGraphicsEffect *effect = m_customGraphicsEffects[vm->getInput(0, 2)->toLong()];
        target->setGraphicsEffectValue(effect, target->graphicsEffectValue(effect) + vm->getInput(1, 2)->toDouble());
    }

    return 2;
}

unsigned int LooksBlocks::changeColorEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_colorEffect, target->graphicsEffectValue(m_colorEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeFisheyeEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_fisheyeEffect, target->graphicsEffectValue(m_fisheyeEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeWhirlEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_whirlEffect, target->graphicsEffectValue(m_whirlEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changePixelateEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_pixelateEffect, target->graphicsEffectValue(m_pixelateEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeMosaicEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_mosaicEffect, target->graphicsEffectValue(m_mosaicEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeBrightnessEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_brightnessEffect, target->graphicsEffectValue(m_brightnessEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeGhostEffectBy(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_ghostEffect, target->graphicsEffectValue(m_ghostEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_customGraphicsEffects[vm->getInput(0, 2)->toLong()], vm->getInput(1, 2)->toDouble());

    return 2;
}

unsigned int LooksBlocks::setColorEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_colorEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setFisheyeEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_fisheyeEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setWhirlEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_whirlEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setPixelateEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_pixelateEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setMosaicEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_mosaicEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setBrightnessEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_brightnessEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setGhostEffectTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->setGraphicsEffectValue(m_ghostEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::clearGraphicEffects(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (target)
        target->clearGraphicsEffects();

    return 0;
}

unsigned int LooksBlocks::changeSizeBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setSize(sprite->size() + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setSizeTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setSize(vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::size(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->addReturnValue(sprite->size());
    else
        vm->addReturnValue(0);

    return 0;
}

void LooksBlocks::setCostumeByIndex(Target *target, long index)
{
    long costumeCount = target->costumes().size();

    if (index < 0 || index >= costumeCount) {
        if (index < 0)
            index = std::fmod(costumeCount + std::fmod(index, -costumeCount), costumeCount);
        else
            index = std::fmod(index, costumeCount);
    }

    target->setCostumeIndex(index);
}

unsigned int LooksBlocks::switchCostumeTo(VirtualMachine *vm)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/blocks/scratch3_looks.js#L389-L413
    Target *target = vm->target();

    if (!target)
        return 1;

    const Value *name = vm->getInput(0, 1);

    if (!name->isString()) {
        // Numbers should be treated as costume indices, always
        if (name->isNaN() || name->isInfinity() || name->isNegativeInfinity())
            target->setCostumeIndex(0);
        else
            setCostumeByIndex(target, name->toLong() - 1);
    } else {
        // Strings should be treated as costume names, where possible
        const int costumeIndex = target->findCostume(name->toString());
        std::string nameStr = name->toString();

        auto it = std::find_if(nameStr.begin(), nameStr.end(), [](char c) { return !std::isspace(c); });
        bool isWhiteSpace = (it == nameStr.end());

        if (costumeIndex != -1) {
            setCostumeByIndex(target, costumeIndex);
        } else if (nameStr == "next costume") {
            nextCostume(vm);
        } else if (nameStr == "previous costume") {
            previousCostume(vm);
            // Try to cast the string to a number (and treat it as a costume index)
            // Pure whitespace should not be treated as a number
            // Note: isNaN will cast the string to a number before checking if it's NaN
        } else if (!(name->isNaN() || isWhiteSpace)) {
            target->setCostumeIndex(name->toInt() - 1);
        }
    }

    return 1;
}

unsigned int LooksBlocks::nextCostume(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        setCostumeByIndex(target, target->costumeIndex() + 1);

    return 0;
}

unsigned int LooksBlocks::previousCostume(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        setCostumeByIndex(target, target->costumeIndex() - 1);

    return 0;
}

void LooksBlocks::startBackdropScripts(VirtualMachine *vm, bool wait)
{
    if (Stage *stage = vm->engine()->stage()) {
        if (stage->costumes().size() > 0)
            vm->engine()->startBackdropScripts(stage->currentCostume()->broadcast(), vm->thread(), wait);
    }
}

void LooksBlocks::switchBackdropToImpl(VirtualMachine *vm)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/blocks/scratch3_looks.js#L423-L462
    Stage *stage = vm->engine()->stage();

    if (!stage)
        return;

    const Value *name = vm->getInput(0, 1);

    if (!name->isString()) {
        // Numbers should be treated as costume indices, always
        if (name->isNaN() || name->isInfinity() || name->isNegativeInfinity())
            stage->setCostumeIndex(0);
        else
            setCostumeByIndex(stage, name->toLong() - 1);
    } else {
        // Strings should be treated as costume names, where possible
        const int costumeIndex = stage->findCostume(name->toString());
        std::string nameStr = name->toString();

        auto it = std::find_if(nameStr.begin(), nameStr.end(), [](char c) { return !std::isspace(c); });
        bool isWhiteSpace = (it == nameStr.end());

        if (costumeIndex != -1) {
            setCostumeByIndex(stage, costumeIndex);
        } else if (nameStr == "next backdrop") {
            nextBackdropImpl(vm);
        } else if (nameStr == "previous backdrop") {
            previousBackdropImpl(vm);
        } else if (nameStr == "random backdrop") {
            randomBackdropImpl(vm);
            // Try to cast the string to a number (and treat it as a costume index)
            // Pure whitespace should not be treated as a number
            // Note: isNaN will cast the string to a number before checking if it's NaN
        } else if (!(name->isNaN() || isWhiteSpace)) {
            stage->setCostumeIndex(name->toInt() - 1);
        }
    }
}

void LooksBlocks::nextBackdropImpl(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage())
        setCostumeByIndex(stage, stage->costumeIndex() + 1);
}

void LooksBlocks::previousBackdropImpl(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage())
        setCostumeByIndex(stage, stage->costumeIndex() - 1);
}

void LooksBlocks::randomBackdropImpl(VirtualMachine *vm)
{
    if (!rng)
        rng = RandomGenerator::instance().get();

    if (Stage *stage = vm->engine()->stage()) {
        std::size_t count = stage->costumes().size();

        if (count > 0)
            stage->setCostumeIndex(rng->randint(0, count - 1));
    }
}

unsigned int LooksBlocks::switchBackdropTo(VirtualMachine *vm)
{
    switchBackdropToImpl(vm);
    startBackdropScripts(vm, false);

    return 1;
}

unsigned int LooksBlocks::switchBackdropToAndWait(VirtualMachine *vm)
{
    switchBackdropToImpl(vm);
    startBackdropScripts(vm, true);
    vm->promise();

    return 1;
}

unsigned int LooksBlocks::nextBackdrop(VirtualMachine *vm)
{
    nextBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::previousBackdrop(VirtualMachine *vm)
{
    previousBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::randomBackdrop(VirtualMachine *vm)
{
    randomBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::goToFront(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->engine()->moveSpriteToFront(sprite);

    return 0;
}

unsigned int LooksBlocks::goToBack(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->engine()->moveSpriteToBack(sprite);

    return 0;
}

unsigned int LooksBlocks::goForwardLayers(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->engine()->moveSpriteForwardLayers(sprite, vm->getInput(0, 1)->toInt());

    return 1;
}

unsigned int LooksBlocks::goBackwardLayers(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        vm->engine()->moveSpriteBackwardLayers(sprite, vm->getInput(0, 1)->toInt());

    return 1;
}

unsigned int LooksBlocks::costumeNumber(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        vm->addReturnValue(target->costumeIndex() + 1);
    else
        vm->addReturnValue(0);

    return 0;
}

unsigned int LooksBlocks::costumeName(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        auto costume = target->currentCostume();

        if (costume)
            vm->addReturnValue(costume->name());
        else
            vm->addReturnValue("");
    } else
        vm->addReturnValue("");

    return 0;
}

unsigned int LooksBlocks::backdropNumber(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage())
        vm->addReturnValue(stage->costumeIndex() + 1);
    else
        vm->addReturnValue(0);

    return 0;
}

unsigned int LooksBlocks::backdropName(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage()) {
        auto costume = stage->currentCostume();

        if (costume)
            vm->addReturnValue(costume->name());
        else
            vm->addReturnValue("");
    } else
        vm->addReturnValue("");

    return 0;
}
