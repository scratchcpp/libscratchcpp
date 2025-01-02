#include <scratchcpp/inputvalue.h>
#include <scratchcpp/block.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#ifdef USE_LLVM
#include <scratchcpp/dev/compiler.h>
#else
#include <scratchcpp/compiler.h>
#endif
#include <scratchcpp/input.h>
#include <enginemock.h>
#include <targetmock.h>
#ifdef USE_LLVM
#include <codebuilderfactorymock.h>
#include <codebuildermock.h>
#endif

#include "../common.h"
#include "dev/engine/compiler_p.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;

TEST(InputValueTest, Constructors)
{
    InputValue value1;
    ASSERT_EQ(value1.type(), InputValue::Type::Number);

    InputValue value2(InputValue::Type::String);
    ASSERT_EQ(value2.type(), InputValue::Type::String);
}

TEST(InputValueTest, Type)
{
    InputValue value;

    value.setType(InputValue::Type::Color);
    ASSERT_EQ(value.type(), InputValue::Type::Color);
}

TEST(InputValueTest, Value)
{
    InputValue value;
    ASSERT_EQ(value.value(), Value());

    value.setValue("test");
    ASSERT_EQ(value.value().toString(), "test");
}

TEST(InputValueTest, ValueBlock)
{
    InputValue value;
    ASSERT_EQ(value.valueBlock(), nullptr);
    ASSERT_EQ(value.valueBlockId(), "");

    auto block = std::make_shared<Block>("abc", "");
    value.setValueBlock(block);
    ASSERT_EQ(value.valueBlock(), block);
    ASSERT_EQ(value.valueBlockId(), "abc");

    value.setValueBlock(nullptr);
    ASSERT_EQ(value.valueBlock(), nullptr);
    ASSERT_EQ(value.valueBlockId(), "");

    value.setValueBlockId("hello");
    ASSERT_EQ(value.valueBlockId(), "hello");
    ASSERT_EQ(value.valueBlock(), nullptr);
}

TEST(InputValueTest, ValuePtr)
{
    InputValue value1;
    ASSERT_EQ(value1.valuePtr(), nullptr);
    ASSERT_EQ(value1.valueId(), "");

    auto broadcast = std::make_shared<Broadcast>("abc", "");
    value1.setValuePtr(broadcast);
    ASSERT_EQ(value1.valuePtr(), broadcast);
    ASSERT_EQ(value1.valueId(), "abc");
    ASSERT_EQ(value1.type(), InputValue::Type::Broadcast);

    auto variable = std::make_shared<Variable>("def", "");
    value1.setValuePtr(variable);
    ASSERT_EQ(value1.valuePtr(), variable);
    ASSERT_EQ(value1.valueId(), "def");
    ASSERT_EQ(value1.type(), InputValue::Type::Variable);

    auto list = std::make_shared<List>("ghi", "");
    value1.setValuePtr(list);
    ASSERT_EQ(value1.valuePtr(), list);
    ASSERT_EQ(value1.valueId(), "ghi");
    ASSERT_EQ(value1.type(), InputValue::Type::List);

    auto block = std::make_shared<Block>("jkl", "");
    value1.setValuePtr(block);
    ASSERT_EQ(value1.valuePtr(), nullptr);
    ASSERT_EQ(value1.valueId(), "");
    ASSERT_EQ(value1.type(), InputValue::Type::List);

    value1.setValuePtr(nullptr);
    ASSERT_EQ(value1.valuePtr(), nullptr);
    ASSERT_EQ(value1.valueId(), "");
    ASSERT_EQ(value1.type(), InputValue::Type::List);

    InputValue value2(InputValue::Type::Integer);

    value2.setValuePtr(block);
    ASSERT_EQ(value2.valuePtr(), nullptr);
    ASSERT_EQ(value1.valueId(), "");
    ASSERT_EQ(value2.type(), InputValue::Type::Integer);

    value2.setValuePtr(variable);
    value2.setValueId("hello");
    ASSERT_EQ(value2.valuePtr(), nullptr);
    ASSERT_EQ(value2.valueId(), "hello");
    ASSERT_EQ(value2.type(), InputValue::Type::Variable);
}

#ifdef USE_LLVM
TEST(InputValueTest, Compile)
{
    EngineMock engine;
    TargetMock target;
    CodeBuilderFactoryMock builderFactory;
    auto builder = std::make_shared<CodeBuilderMock>();
    CompilerPrivate::builderFactory = &builderFactory;
    Compiler compiler(&engine, &target);

    auto block = std::make_shared<Block>("", "");
    EXPECT_CALL(builderFactory, create).WillOnce(Return(builder));
    EXPECT_CALL(*builder, finalize);
    compiler.compile(block);
    CompilerPrivate::builderFactory = nullptr;

    auto input = std::make_shared<Input>("", Input::Type::Shadow);
    input->setPrimaryValue(5);
    block->addInput(input);

    InputValue *value = input->primaryValue();
    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::Number);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::PositiveNumber);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::PositiveInteger);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::Integer);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::Angle);
    value->compile(&compiler);

    // TODO: Add support for colors
    // TODO: Use a loop from type 4 to 11
    /*value->setType(InputValue::Type::Color);
    value->compile(&compiler);*/

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::String);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addConstValue(input->primaryValue()->value()));
    value->setType(InputValue::Type::Broadcast);
    value->compile(&compiler);

    auto variable = std::make_shared<Variable>("", "");
    auto list = std::make_shared<List>("", "");

    EXPECT_CALL(*builder, addVariableValue(variable.get()));
    value->setType(InputValue::Type::Variable);
    value->setValuePtr(variable);
    value->compile(&compiler);

    EXPECT_CALL(*builder, addListContents(list.get()));
    value->setType(InputValue::Type::List);
    value->setValuePtr(list);
    value->compile(&compiler);
}
#else
TEST(InputValueTest, Compile)
{
    EngineMock engine;
    Compiler compiler(&engine);

    auto block = std::make_shared<Block>("", "");
    auto input = std::make_shared<Input>("", Input::Type::Shadow);
    input->setPrimaryValue(5);
    block->addInput(input);

    InputValue *value = input->primaryValue();

    compiler.init();
    compiler.setBlock(block);
    value->setType(InputValue::Type::Number);
    value->compile(&compiler);
    value->setType(InputValue::Type::PositiveNumber);
    value->compile(&compiler);
    value->setType(InputValue::Type::PositiveInteger);
    value->compile(&compiler);
    value->setType(InputValue::Type::Integer);
    value->compile(&compiler);
    value->setType(InputValue::Type::Angle);
    value->compile(&compiler);
    // TODO: Add support for colors
    /*value->setType(InputValue::Type::Color);
    value->compile(&compiler);*/
    value->setType(InputValue::Type::String);
    value->compile(&compiler);
    value->setType(InputValue::Type::Broadcast);
    value->compile(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_CONST, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);

    auto variable = std::make_shared<Variable>("", "");
    auto list = std::make_shared<List>("", "");

    compiler.init();
    compiler.setBlock(block);
    value->setType(InputValue::Type::Variable);
    value->setValuePtr(variable);
    value->compile(&compiler);
    value->setType(InputValue::Type::List);
    value->setValuePtr(list);
    value->compile(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_READ_VAR, 0, vm::OP_READ_LIST, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.variables(), std::vector<Variable *>({ variable.get() }));
    ASSERT_EQ(compiler.lists(), std::vector<List *>({ list.get() }));
}
#endif // USE_LLVM
