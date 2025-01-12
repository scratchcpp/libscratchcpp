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
    engine->addCompileFunction(this, "data_deleteoflist", &compileDeleteOfList);
    engine->addCompileFunction(this, "data_deletealloflist", &compileDeleteAllOfList);
    engine->addCompileFunction(this, "data_insertatlist", &compileInsertAtList);
    engine->addCompileFunction(this, "data_replaceitemoflist", &compileReplaceItemOfList);
    engine->addCompileFunction(this, "data_itemoflist", &compileItemOfList);
    engine->addCompileFunction(this, "data_itemnumoflist", &compileItemNumOfList);
    engine->addCompileFunction(this, "data_lengthoflist", &compileLengthOfList);
    engine->addCompileFunction(this, "data_listcontainsitem", &compileListContainsItem);
}

CompilerValue *ListBlocks::compileAddToList(Compiler *compiler)
{
    auto list = compiler->field("LIST")->valuePtr();
    assert(list);

    if (list)
        compiler->createListAppend(static_cast<List *>(list.get()), compiler->addInput("ITEM"));

    return nullptr;
}

CompilerValue *ListBlocks::getListIndex(Compiler *compiler, CompilerValue *input, List *list, CompilerValue *listSize)
{
    CompilerLocalVariable *ret = compiler->createLocalVariable(Compiler::StaticType::Number);

    CompilerValue *isRandom1 = compiler->createCmpEQ(input, compiler->addConstValue("random"));
    CompilerValue *isRandom2 = compiler->createCmpEQ(input, compiler->addConstValue("any"));
    CompilerValue *isRandom = compiler->createOr(isRandom1, isRandom2);

    compiler->beginIfStatement(isRandom);
    {
        CompilerValue *random = compiler->createRandomInt(compiler->addConstValue(1), listSize);
        compiler->createLocalVariableWrite(ret, random);
    }
    compiler->beginElseBranch();
    {
        CompilerValue *isLast = compiler->createCmpEQ(input, compiler->addConstValue("last"));
        compiler->createLocalVariableWrite(ret, compiler->createSelect(isLast, listSize, input, Compiler::StaticType::Number));
    }
    compiler->endIf();

    return compiler->addLocalVariableValue(ret);
}

CompilerValue *ListBlocks::compileDeleteOfList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *index = compiler->addInput("INDEX");
        CompilerValue *cond = compiler->createCmpEQ(index, compiler->addConstValue("all"));
        compiler->beginIfStatement(cond);
        {
            compiler->createListClear(list);
        }
        compiler->beginElseBranch();
        {
            CompilerValue *max = compiler->addListSize(list);
            index = getListIndex(compiler, index, list, max);
            index = compiler->createSub(index, compiler->addConstValue(1));
            compiler->createListRemove(list, index);
        }
        compiler->endIf();
    }

    return nullptr;
}

CompilerValue *ListBlocks::compileDeleteAllOfList(Compiler *compiler)
{
    auto list = compiler->field("LIST")->valuePtr();
    assert(list);

    if (list)
        compiler->createListClear(static_cast<List *>(list.get()));

    return nullptr;
}

CompilerValue *ListBlocks::compileInsertAtList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *index = compiler->addInput("INDEX");
        CompilerValue *max = compiler->createAdd(compiler->addListSize(list), compiler->addConstValue(1));
        index = getListIndex(compiler, index, list, max);
        index = compiler->createSub(index, compiler->addConstValue(1));
        CompilerValue *item = compiler->addInput("ITEM");
        compiler->createListInsert(list, index, item);
    }

    return nullptr;
}

CompilerValue *ListBlocks::compileReplaceItemOfList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *index = compiler->addInput("INDEX");
        CompilerValue *max = compiler->addListSize(list);
        index = getListIndex(compiler, index, list, max);
        index = compiler->createSub(index, compiler->addConstValue(1));
        CompilerValue *item = compiler->addInput("ITEM");
        compiler->createListReplace(list, index, item);
    }

    return nullptr;
}

CompilerValue *ListBlocks::compileItemOfList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *index = compiler->addInput("INDEX");
        CompilerValue *max = compiler->addListSize(list);
        index = getListIndex(compiler, index, list, max);
        index = compiler->createSub(index, compiler->addConstValue(1));
        return compiler->addListItem(list, index);
    }

    return nullptr;
}

CompilerValue *ListBlocks::compileItemNumOfList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *item = compiler->addInput("ITEM");
        return compiler->createAdd(compiler->addListItemIndex(list, item), compiler->addConstValue(1));
    }

    return nullptr;
}

CompilerValue *ListBlocks::compileLengthOfList(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list)
        return compiler->addListSize(list);

    return nullptr;
}

CompilerValue *ListBlocks::compileListContainsItem(Compiler *compiler)
{
    List *list = static_cast<List *>(compiler->field("LIST")->valuePtr().get());
    assert(list);

    if (list) {
        CompilerValue *item = compiler->addInput("ITEM");
        return compiler->addListContains(list, item);
    }

    return nullptr;
}
