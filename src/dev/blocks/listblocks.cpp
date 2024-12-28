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
}

CompilerValue *ListBlocks::compileAddToList(Compiler *compiler)
{
    auto list = compiler->field("LIST")->valuePtr();
    assert(list);

    if (list)
        compiler->createListAppend(static_cast<List *>(list.get()), compiler->addInput("ITEM"));

    return nullptr;
}

CompilerValue *ListBlocks::getListIndex(Compiler *compiler, CompilerValue *input, List *list)
{
    CompilerLocalVariable *ret = compiler->createLocalVariable(Compiler::StaticType::Number);
    CompilerValue *size = compiler->addListSize(list);

    CompilerValue *isRandom1 = compiler->createCmpEQ(input, compiler->addConstValue("random"));
    CompilerValue *isRandom2 = compiler->createCmpEQ(input, compiler->addConstValue("any"));
    CompilerValue *isRandom = compiler->createOr(isRandom1, isRandom2);

    compiler->beginIfStatement(isRandom);
    {
        CompilerValue *random = compiler->createRandomInt(compiler->addConstValue(1), size);
        compiler->createLocalVariableWrite(ret, random);
    }
    compiler->beginElseBranch();
    {
        CompilerValue *isLast = compiler->createCmpEQ(input, compiler->addConstValue("last"));
        compiler->createLocalVariableWrite(ret, compiler->createSelect(isLast, size, input, Compiler::StaticType::Number));
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
            index = getListIndex(compiler, index, list);
            index = compiler->createSub(index, compiler->addConstValue(1));
            CompilerValue *min = compiler->addConstValue(-1);
            CompilerValue *max = compiler->addListSize(list);
            cond = compiler->createAnd(compiler->createCmpGT(index, min), compiler->createCmpLT(index, max));
            compiler->beginIfStatement(cond);
            {
                compiler->createListRemove(list, index);
            }
            compiler->endIf();
        }
        compiler->endIf();
    }

    return nullptr;
}
