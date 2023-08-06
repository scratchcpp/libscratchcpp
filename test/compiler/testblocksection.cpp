#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <gtest/gtest.h>

#include "testblocksection.h"

using namespace libscratchcpp;

std::string TestBlockSection::name() const
{
    return "Test";
}

void TestBlockSection::registerBlocks(IEngine *engine)
{
    engine->addInput(this, "INPUT1", INPUT1);

    engine->addField(this, "FIELD1", FIELD1);

    engine->addFieldValue(this, "test", TestValue);

    engine->addCompileFunction(this, "test_block1", &compileTestBlock1);
    engine->addCompileFunction(this, "test_block2", &compileTestBlock2);
}

void TestBlockSection::compileTestBlock1(Compiler *compiler)
{
    compiler->addInput(INPUT1);
    ASSERT_EQ(compiler->inputBlock(INPUT1), compiler->input(INPUT1)->valueBlock());

    compiler->addInstruction(vm::OP_PRINT);
}

void TestBlockSection::compileTestBlock2(Compiler *compiler)
{
    int id = compiler->field(FIELD1)->specialValueId();

    if (id == TestValue) {
        compiler->addInstruction(vm::OP_NULL);
        compiler->addInstruction(vm::OP_PRINT);
    }
}
