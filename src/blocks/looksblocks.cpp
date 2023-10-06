// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/field.h>
#include <scratchcpp/costume.h>

#include "looksblocks.h"

using namespace libscratchcpp;

std::string LooksBlocks::name() const
{
    return "Looks";
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "looks_show", &compileShow);
    engine->addCompileFunction(this, "looks_hide", &compileHide);
    engine->addCompileFunction(this, "looks_changesizeby", &compileChangeSizeBy);
    engine->addCompileFunction(this, "looks_setsizeto", &compileSetSizeTo);
    engine->addCompileFunction(this, "looks_size", &compileSize);
    engine->addCompileFunction(this, "looks_costumenumbername", &compileCostumeNumberName);

    // Inputs
    engine->addInput(this, "CHANGE", CHANGE);
    engine->addInput(this, "SIZE", SIZE);

    // Fields
    engine->addField(this, "NUMBER_NAME", NUMBER_NAME);

    // Field values
    engine->addFieldValue(this, "number", CostumeNumber);
    engine->addFieldValue(this, "name", CostumeName);
}

void LooksBlocks::compileShow(Compiler *compiler)
{
    compiler->addFunctionCall(&show);
}

void LooksBlocks::compileHide(Compiler *compiler)
{
    compiler->addFunctionCall(&hide);
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

void LooksBlocks::compileCostumeNumberName(Compiler *compiler)
{
    int option = compiler->field(NUMBER_NAME)->specialValueId();

    switch (option) {
        case CostumeNumber:
            compiler->addFunctionCall(&costumeNumber);
            break;

        case CostumeName:
            compiler->addFunctionCall(&costumeName);
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

unsigned int LooksBlocks::costumeNumber(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        vm->addReturnValue(target->currentCostume());
    else
        vm->addReturnValue(0);

    return 0;
}

unsigned int LooksBlocks::costumeName(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        auto costume = target->costumeAt(target->currentCostume() - 1);

        if (costume)
            vm->addReturnValue(costume->name());
        else
            vm->addReturnValue("");
    } else
        vm->addReturnValue("");

    return 0;
}
