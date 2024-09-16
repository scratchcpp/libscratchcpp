// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/list.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/monitor.h>

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
    engine->addCompileFunction(this, "data_listcontents", &compileListContents);
    engine->addCompileFunction(this, "data_addtolist", &compileAddToList);
    engine->addCompileFunction(this, "data_deleteoflist", &compileDeleteFromList);
    engine->addCompileFunction(this, "data_deletealloflist", &compileDeleteAllOfList);
    engine->addCompileFunction(this, "data_insertatlist", &compileInsertToList);
    engine->addCompileFunction(this, "data_replaceitemoflist", &compileReplaceItemOfList);
    engine->addCompileFunction(this, "data_itemoflist", &compileItemOfList);
    engine->addCompileFunction(this, "data_itemnumoflist", &compileItemNumberInList);
    engine->addCompileFunction(this, "data_lengthoflist", &compileLengthOfList);
    engine->addCompileFunction(this, "data_listcontainsitem", &compileListContainsItem);
    engine->addCompileFunction(this, "data_showlist", &compileShowList);
    engine->addCompileFunction(this, "data_hidelist", &compileHideList);

    // Monitor names
    engine->addMonitorNameFunction(this, "data_listcontents", &listContentsMonitorName);

    // Inputs
    engine->addInput(this, "ITEM", ITEM);
    engine->addInput(this, "INDEX", INDEX);

    // Fields
    engine->addField(this, "LIST", LIST);
}

void ListBlocks::compileListContents(Compiler *compiler)
{
    // NOTE: This block is only used by list monitors
    // Instead of returning the actual list contents, let's just return the index of the list
    // and let the renderer read the list using the index
    compiler->addConstValue(static_cast<size_t>(compiler->listIndex(compiler->field(LIST)->valuePtr())));
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

void ListBlocks::compileShowList(Compiler *compiler)
{
    Field *field = compiler->field(LIST);
    assert(field);
    List *var = static_cast<List *>(field->valuePtr().get());
    assert(var);

    compiler->addConstValue(var->id());

    if (var->target() == static_cast<Target *>(compiler->engine()->stage()))
        compiler->addFunctionCall(&showGlobalList);
    else
        compiler->addFunctionCall(&showList);
}

void ListBlocks::compileHideList(Compiler *compiler)
{
    Field *field = compiler->field(LIST);
    assert(field);
    List *var = static_cast<List *>(field->valuePtr().get());
    assert(var);

    compiler->addConstValue(var->id());

    if (var->target() == static_cast<Target *>(compiler->engine()->stage()))
        compiler->addFunctionCall(&hideGlobalList);
    else
        compiler->addFunctionCall(&hideList);
}

void ListBlocks::setListVisible(std::shared_ptr<List> list, bool visible, IEngine *engine)
{
    if (list) {
        Monitor *monitor = list->monitor();

        if (!monitor)
            monitor = engine->createListMonitor(list, "data_listcontents", "LIST", LIST, &compileListContents);

        monitor->setVisible(visible);
    }
}

unsigned int ListBlocks::showGlobalList(VirtualMachine *vm)
{
    if (Stage *target = vm->engine()->stage()) {
        int index = target->findListById(vm->getInput(0, 1)->toString());
        setListVisible(target->listAt(index), true, vm->engine());
    }

    return 1;
}

unsigned int ListBlocks::showList(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        if (!target->isStage() && static_cast<Sprite *>(target)->isClone()) {
            Sprite *sprite = static_cast<Sprite *>(target)->cloneSprite(); // use clone root list
            int index = sprite->findListById(vm->getInput(0, 1)->toString());
            setListVisible(sprite->listAt(index), true, vm->engine());
        } else {
            int index = target->findListById(vm->getInput(0, 1)->toString());
            setListVisible(target->listAt(index), true, vm->engine());
        }
    }

    return 1;
}

unsigned int ListBlocks::hideGlobalList(VirtualMachine *vm)
{
    if (Stage *target = vm->engine()->stage()) {
        int index = target->findListById(vm->getInput(0, 1)->toString());
        setListVisible(target->listAt(index), false, vm->engine());
    }

    return 1;
}

unsigned int ListBlocks::hideList(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        if (!target->isStage() && static_cast<Sprite *>(target)->isClone()) {
            Sprite *sprite = static_cast<Sprite *>(target)->cloneSprite(); // use clone root list
            int index = sprite->findListById(vm->getInput(0, 1)->toString());
            setListVisible(sprite->listAt(index), false, vm->engine());
        } else {
            int index = target->findListById(vm->getInput(0, 1)->toString());
            setListVisible(target->listAt(index), false, vm->engine());
        }
    }

    return 1;
}

const std::string &ListBlocks::listContentsMonitorName(Block *block)
{
    List *list = dynamic_cast<List *>(block->findFieldById(LIST)->valuePtr().get());

    if (list)
        return list->name();
    else {
        static const std::string empty = "";
        return empty;
    }
}
