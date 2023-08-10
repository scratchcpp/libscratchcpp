// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/field.h>

#include "listblocks.h"

using namespace libscratchcpp;

std::string ListBlocks::name() const
{
    return "Lists";
}

bool ListBlocks::categoryVisible() const
{
    return false;
}

void ListBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "data_addtolist", &compileAddToList);
    engine->addCompileFunction(this, "data_deleteoflist", &compileDeleteFromList);
    engine->addCompileFunction(this, "data_deletealloflist", &compileDeleteAllOfList);
    engine->addCompileFunction(this, "data_insertatlist", &compileInsertToList);
    engine->addCompileFunction(this, "data_replaceitemoflist", &compileReplaceItemOfList);
    engine->addCompileFunction(this, "data_itemoflist", &compileItemOfList);
    engine->addCompileFunction(this, "data_itemnumoflist", &compileItemNumberInList);
    engine->addCompileFunction(this, "data_lengthoflist", &compileLengthOfList);
    engine->addCompileFunction(this, "data_listcontainsitem", &compileListContainsItem);

    // Inputs
    engine->addInput(this, "ITEM", ITEM);
    engine->addInput(this, "INDEX", INDEX);

    // Fields
    engine->addField(this, "LIST", LIST);
}

void ListBlocks::compileAddToList(Compiler *compiler)
{
    compiler->addInput(ITEM);
    compiler->addInstruction(vm::OP_LIST_APPEND, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileDeleteFromList(Compiler *compiler)
{
    compiler->addInput(INDEX);
    compiler->addInstruction(vm::OP_LIST_DEL, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileDeleteAllOfList(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_LIST_DEL_ALL, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileInsertToList(Compiler *compiler)
{
    compiler->addInput(ITEM);
    compiler->addInput(INDEX);
    compiler->addInstruction(vm::OP_LIST_INSERT, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileReplaceItemOfList(Compiler *compiler)
{
    compiler->addInput(INDEX);
    compiler->addInput(ITEM);
    compiler->addInstruction(vm::OP_LIST_REPLACE, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileItemOfList(Compiler *compiler)
{
    compiler->addInput(INDEX);
    compiler->addInstruction(vm::OP_LIST_GET_ITEM, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileItemNumberInList(Compiler *compiler)
{
    compiler->addInput(ITEM);
    compiler->addInstruction(vm::OP_LIST_INDEX_OF, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileLengthOfList(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_LIST_LENGTH, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}

void ListBlocks::compileListContainsItem(Compiler *compiler)
{
    compiler->addInput(ITEM);
    compiler->addInstruction(vm::OP_LIST_CONTAINS, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}
