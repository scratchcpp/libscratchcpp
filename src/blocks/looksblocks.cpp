// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>

#include "looksblocks.h"

using namespace libscratchcpp;

std::string LooksBlocks::name() const
{
    return "Looks";
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "looks_show", &compileShow);
    engine->addCompileFunction(this, "looks_hide", &compileHide);
}

void LooksBlocks::compileShow(Compiler *compiler)
{
    compiler->addFunctionCall(&show);
}

void LooksBlocks::compileHide(Compiler *compiler)
{
    compiler->addFunctionCall(&hide);
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
