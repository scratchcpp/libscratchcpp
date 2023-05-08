// SPDX-License-Identifier: Apache-2.0

#include "listblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

ListBlocks::ListBlocks()
{
    // Blocks
    addCompileFunction("data_addtolist", &compileAddToList);
    addCompileFunction("data_deleteoflist", &compileDeleteFromList);
    addCompileFunction("data_deletealloflist", &compileDeleteAllOfList);
    addCompileFunction("data_insertatlist", &compileInsertToList);
    addCompileFunction("data_replaceitemoflist", &compileReplaceItemOfList);
    addCompileFunction("data_itemoflist", &compileItemOfList);
    addCompileFunction("data_itemnumoflist", &compileItemNumberInList);
    addCompileFunction("data_lengthoflist", &compileLengthOfList);
    addCompileFunction("data_listcontainsitem", &compileListContainsItem);

    // Inputs
    addInput("ITEM", ITEM);
    addInput("INDEX", INDEX);

    // Fields
    addField("LIST", LIST);
}

std::string ListBlocks::name() const
{
    return "Lists";
}

bool ListBlocks::categoryVisible() const
{
    return false;
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
    compiler->addInstruction(vm::OP_LIST_LENGTH, { compiler->listIndex(compiler->field(LIST)->valuePtr()) });
}
