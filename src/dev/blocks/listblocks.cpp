// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>

#include "listblocks.h"

using namespace libscratchcpp;

std::string ListBlocks::name() const
{
    return "Lists";
}

std::string ListBlocks::description() const
{
    return "List blocks";
}

void ListBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "data_addtolist", &compileAddToList);
}

CompilerValue *ListBlocks::compileAddToList(Compiler *compiler)
{
    auto list = compiler->field("LIST")->valuePtr();
    assert(list);

    if (list)
        compiler->createListAppend(static_cast<List *>(list.get()), compiler->addInput("ITEM"));

    return nullptr;
}
