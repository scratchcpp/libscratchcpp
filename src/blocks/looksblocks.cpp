// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>

#include "looksblocks.h"
#include "../engine/internal/randomgenerator.h"

using namespace libscratchcpp;

IRandomGenerator *LooksBlocks::rng = nullptr;

std::string LooksBlocks::name() const
{
    return "Looks";
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
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

    // Inputs
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

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setColorEffectTo);
            break;

        case FisheyeEffect:
            if (!m_fisheyeEffect)
                m_fisheyeEffect = ScratchConfiguration::getGraphicsEffect("fisheye");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setFisheyeEffectTo);
            break;

        case WhirlEffect:
            if (!m_whirlEffect)
                m_whirlEffect = ScratchConfiguration::getGraphicsEffect("whirl");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setWhirlEffectTo);
            break;

        case PixelateEffect:
            if (!m_pixelateEffect)
                m_pixelateEffect = ScratchConfiguration::getGraphicsEffect("pixelate");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setPixelateEffectTo);
            break;

        case MosaicEffect:
            if (!m_mosaicEffect)
                m_mosaicEffect = ScratchConfiguration::getGraphicsEffect("mosaic");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setMosaicEffectTo);
            break;

        case BrightnessEffect:
            if (!m_brightnessEffect)
                m_brightnessEffect = ScratchConfiguration::getGraphicsEffect("brightness");

            compiler->addInput(CHANGE);
            compiler->addFunctionCall(&setBrightnessEffectTo);
            break;

        case GhostEffect:
            if (!m_ghostEffect)
                m_ghostEffect = ScratchConfiguration::getGraphicsEffect("ghost");

            compiler->addInput(CHANGE);
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
                compiler->addInput(CHANGE);
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

    Input *input = compiler->input(COSTUME);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();
        int index = target->findCostume(value);

        if (index == -1) {
            if (value == "next costume")
                compiler->addFunctionCall(&nextCostume);
            else if (value == "previous costume")
                compiler->addFunctionCall(&previousCostume);
            else {
                Value v(value);

                if (v.type() == Value::Type::Integer) {
                    compiler->addConstValue(v.toLong() - 1);
                    compiler->addFunctionCall(&switchCostumeToByIndex);
                }
            }
        } else {
            compiler->addConstValue(index);
            compiler->addFunctionCall(&switchCostumeToByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&switchCostumeTo);
    }
}

void LooksBlocks::compileNextCostume(Compiler *compiler)
{
    compiler->addFunctionCall(&nextCostume);
}

void LooksBlocks::compileSwitchBackdropTo(Compiler *compiler)
{
    Stage *stage = compiler->engine()->stage();

    if (!stage)
        return;

    Input *input = compiler->input(BACKDROP);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();
        int index = stage->findCostume(value);

        if (index == -1) {
            if (value == "next backdrop")
                compiler->addFunctionCall(&nextBackdrop);
            else if (value == "previous backdrop")
                compiler->addFunctionCall(&previousBackdrop);
            else if (value == "random backdrop")
                compiler->addFunctionCall(&randomBackdrop);
            else {
                Value v(value);

                if (v.type() == Value::Type::Integer) {
                    compiler->addConstValue(v.toLong() - 1);
                    compiler->addFunctionCall(&switchBackdropToByIndex);
                }
            }
        } else {
            compiler->addConstValue(index);
            compiler->addFunctionCall(&switchBackdropToByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&switchBackdropTo);
    }
}

void LooksBlocks::compileSwitchBackdropToAndWait(Compiler *compiler)
{
    Stage *stage = compiler->engine()->stage();

    if (!stage)
        return;

    Input *input = compiler->input(BACKDROP);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();
        int index = stage->findCostume(value);

        if (index == -1) {
            if (value == "next backdrop")
                compiler->addFunctionCall(&nextBackdropAndWait);
            else if (value == "previous backdrop")
                compiler->addFunctionCall(&previousBackdropAndWait);
            else if (value == "random backdrop")
                compiler->addFunctionCall(&randomBackdropAndWait);
            else {
                Value v(value);

                if (v.type() == Value::Type::Integer) {
                    compiler->addConstValue(v.toLong() - 1);
                    compiler->addFunctionCall(&switchBackdropToByIndexAndWait);
                }
            }
        } else {
            compiler->addConstValue(index);
            compiler->addFunctionCall(&switchBackdropToByIndexAndWait);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&switchBackdropToAndWait);
    }

    compiler->addFunctionCall(&backdropNumber);
    compiler->addFunctionCall(&checkBackdropScripts);
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
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite) {
        IGraphicsEffect *effect = m_customGraphicsEffects[vm->getInput(0, 2)->toLong()];
        sprite->setGraphicsEffectValue(effect, sprite->graphicsEffectValue(effect) + vm->getInput(1, 2)->toDouble());
    }

    return 2;
}

unsigned int LooksBlocks::changeColorEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_colorEffect, sprite->graphicsEffectValue(m_colorEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeFisheyeEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_fisheyeEffect, sprite->graphicsEffectValue(m_fisheyeEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeWhirlEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_whirlEffect, sprite->graphicsEffectValue(m_whirlEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changePixelateEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_pixelateEffect, sprite->graphicsEffectValue(m_pixelateEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeMosaicEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_mosaicEffect, sprite->graphicsEffectValue(m_mosaicEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeBrightnessEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_brightnessEffect, sprite->graphicsEffectValue(m_brightnessEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::changeGhostEffectBy(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_ghostEffect, sprite->graphicsEffectValue(m_ghostEffect) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_customGraphicsEffects[vm->getInput(0, 2)->toLong()], vm->getInput(1, 2)->toDouble());

    return 2;
}

unsigned int LooksBlocks::setColorEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_colorEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setFisheyeEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_fisheyeEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setWhirlEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_whirlEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setPixelateEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_pixelateEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setMosaicEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_mosaicEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setBrightnessEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_brightnessEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::setGhostEffectTo(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setGraphicsEffectValue(m_ghostEffect, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int LooksBlocks::clearGraphicEffects(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->clearGraphicsEffects();

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
    // TODO: Remove this (#248)
    std::size_t costumeCount = target->costumes().size();
    if (index < 0 || index >= costumeCount) {
        if (index < 0)
            index = std::fmod(costumeCount + std::fmod(index, -costumeCount), costumeCount);
        else
            index = std::fmod(index, costumeCount);
    }

    target->setCostumeIndex(index);
}

unsigned int LooksBlocks::switchCostumeToByIndex(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        setCostumeByIndex(target, vm->getInput(0, 1)->toLong());

    return 1;
}

unsigned int LooksBlocks::switchCostumeTo(VirtualMachine *vm)
{
    Target *target = vm->target();

    if (!target)
        return 1;

    const Value *name = vm->getInput(0, 1);
    std::string nameStr = name->toString();
    int index = target->findCostume(nameStr);

    if (index == -1) {
        if (nameStr == "next costume")
            nextCostume(vm);
        else if (nameStr == "previous costume")
            previousCostume(vm);
        else {
            if (name->type() == Value::Type::Integer)
                setCostumeByIndex(target, name->toLong() - 1);
        }
    } else
        setCostumeByIndex(target, index);

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
            vm->engine()->startBackdropScripts(stage->currentCostume()->broadcast());
    }
}

void LooksBlocks::switchBackdropToByIndexImpl(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage())
        setCostumeByIndex(stage, vm->getInput(0, 1)->toLong());
}

void LooksBlocks::switchBackdropToImpl(VirtualMachine *vm)
{
    Stage *stage = vm->engine()->stage();

    if (!stage)
        return;

    const Value *name = vm->getInput(0, 1);
    std::string nameStr = name->toString();
    int index = stage->findCostume(nameStr);

    if (index == -1) {
        if (nameStr == "next backdrop")
            nextBackdropImpl(vm);
        else if (nameStr == "previous backdrop")
            previousBackdropImpl(vm);
        else if (nameStr == "random backdrop") {
            randomBackdropImpl(vm);
        } else {
            if (name->type() == Value::Type::Integer)
                setCostumeByIndex(stage, name->toLong() - 1);
        }
    } else
        setCostumeByIndex(stage, index);
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

unsigned int LooksBlocks::switchBackdropToByIndex(VirtualMachine *vm)
{
    switchBackdropToByIndexImpl(vm);
    startBackdropScripts(vm, false);

    return 1;
}

unsigned int LooksBlocks::switchBackdropTo(VirtualMachine *vm)
{
    switchBackdropToImpl(vm);
    startBackdropScripts(vm, false);

    return 1;
}

unsigned int LooksBlocks::switchBackdropToByIndexAndWait(VirtualMachine *vm)
{
    switchBackdropToByIndexImpl(vm);
    startBackdropScripts(vm, true);

    return 1;
}

unsigned int LooksBlocks::switchBackdropToAndWait(VirtualMachine *vm)
{
    switchBackdropToImpl(vm);
    startBackdropScripts(vm, true);

    return 1;
}

unsigned int LooksBlocks::nextBackdrop(VirtualMachine *vm)
{
    nextBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::nextBackdropAndWait(VirtualMachine *vm)
{
    nextBackdropImpl(vm);
    startBackdropScripts(vm, true);

    return 0;
}

unsigned int LooksBlocks::previousBackdrop(VirtualMachine *vm)
{
    previousBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::previousBackdropAndWait(VirtualMachine *vm)
{
    previousBackdropImpl(vm);
    startBackdropScripts(vm, true);

    return 0;
}

unsigned int LooksBlocks::randomBackdrop(VirtualMachine *vm)
{
    randomBackdropImpl(vm);
    startBackdropScripts(vm, false);

    return 0;
}

unsigned int LooksBlocks::randomBackdropAndWait(VirtualMachine *vm)
{
    randomBackdropImpl(vm);
    startBackdropScripts(vm, true);

    return 0;
}

unsigned int LooksBlocks::checkBackdropScripts(VirtualMachine *vm)
{
    if (Stage *stage = vm->engine()->stage()) {
        long index = vm->getInput(0, 1)->toLong() - 1;
        assert(stage->costumes().size() == 0 || index >= 0);

        if ((stage->costumes().size() > 0) && vm->engine()->broadcastByPtrRunning(stage->costumeAt(index)->broadcast()))
            vm->stop(true, true, true);
    }

    return 1;
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
