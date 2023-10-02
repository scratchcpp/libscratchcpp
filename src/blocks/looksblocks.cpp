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
}

void LooksBlocks::compileShow(Compiler *compiler)
{
    compiler->addFunctionCall(&show);
}

unsigned int LooksBlocks::show(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->setVisible(true);

    return 0;
}
