// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/block.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>
#include <scratchcpp/target.h>
#include <scratchcpp/monitor.h>

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

Rgb ListBlocks::color() const
{
    return rgb(255, 102, 26);
}

void ListBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "data_listcontents", &compileListContents);
    engine->addCompileFunction(this, "data_addtolist", &compileAddToList);
    engine->addCompileFunction(this, "data_deleteoflist", &compileDeleteOfList);
    engine->addCompileFunction(this, "data_deletealloflist", &compileDeleteAllOfList);
    engine->addCompileFunction(this, "data_insertatlist", &compileInsertAtList);
    engine->addCompileFunction(this, "data_replaceitemoflist", &compileReplaceItemOfList);
    engine->addCompileFunction(this, "data_itemoflist", &compileItemOfList);
    engine->addCompileFunction(this, "data_itemnumoflist", &compileItemNumOfList);
    engine->addCompileFunction(this, "data_lengthoflist", &compileLengthOfList);
    engine->addCompileFunction(this, "data_listcontainsitem", &compileListContainsItem);
    engine->addCompileFunction(this, "data_showlist", &compileShowList);
    engine->addCompileFunction(this, "data_hidelist", &compileHideList);

    // Monitor names
    engine->addMonitorNameFunction(this, "data_listcontents", &listContentsMonitorName);
}

CompilerValue *ListBlocks::compileListContents(Compiler *compiler)
{
    auto list = compiler->field("LIST")->valuePtr();
    assert(list);

    // If this block is used in a list monitor, return the list pointer
    if (compiler->block()->isMonitorBlock())
        return compiler->addConstValue(list.get());
    else
        return compiler->addListContents(static_cast<List *>(list.get()));
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

    CompilerValue *isRandom1 = compiler->createStrCmpEQ(input, compiler->addConstValue("random"), true);
    CompilerValue *isRandom2 = compiler->createStrCmpEQ(input, compiler->addConstValue("any"), true);
    CompilerValue *isRandom = compiler->createOr(isRandom1, isRandom2);

    compiler->beginIfStatement(isRandom);
    {
        CompilerValue *random = compiler->createRandomInt(compiler->addConstValue(1), listSize);
        compiler->createLocalVariableWrite(ret, random);
    }
    compiler->beginElseBranch();
    {
        CompilerValue *isLast = compiler->createStrCmpEQ(input, compiler->addConstValue("last"), true);
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
        CompilerValue *cond = compiler->createStrCmpEQ(index, compiler->addConstValue("all"), true);
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

CompilerValue *ListBlocks::compileShowList(Compiler *compiler)
{
    Field *field = compiler->field("LIST");
    assert(field);
    List *list = static_cast<List *>(field->valuePtr().get());
    assert(list);

    CompilerConstant *listPtr = compiler->addConstValue(list);
    compiler->addTargetFunctionCall("data_showlist", Compiler::StaticType::Void, { Compiler::StaticType::Pointer }, { listPtr });

    return nullptr;
}

CompilerValue *ListBlocks::compileHideList(Compiler *compiler)
{
    Field *field = compiler->field("LIST");
    assert(field);
    List *list = static_cast<List *>(field->valuePtr().get());
    assert(list);

    CompilerConstant *listPtr = compiler->addConstValue(list);
    compiler->addTargetFunctionCall("data_hidelist", Compiler::StaticType::Void, { Compiler::StaticType::Pointer }, { listPtr });

    return nullptr;
}

const std::string &ListBlocks::listContentsMonitorName(Block *block)
{
    static const std::string empty = "";
    auto field = block->fieldAt(block->findField("LIST"));

    if (!field)
        return empty;

    List *list = dynamic_cast<List *>(field->valuePtr().get());

    if (list)
        return list->name();
    else
        return empty;
}

extern "C" void data_showlist(Target *target, List *list)
{
    Monitor *monitor = list->monitor();

    if (!monitor) {
        /*
         * We need shared_ptr.
         * Since this case doesn't occur frequently,
         * we can look up the list by ID.
         */
        auto index = target->findListById(list->id());
        monitor = target->engine()->createListMonitor(target->listAt(index), "data_listcontents", "LIST", -1, &ListBlocks::compileListContents);
    }

    monitor->setVisible(true);
}

extern "C" void data_hidelist(Target *target, List *list)
{
    Monitor *monitor = list->monitor();

    if (monitor)
        monitor->setVisible(false);
}
