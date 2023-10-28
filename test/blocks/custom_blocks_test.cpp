#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/customblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class CustomBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<CustomBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        void addPrototypeInput(std::shared_ptr<Block> definitionBlock, std::shared_ptr<Block> prototypeBlock) const
        {
            auto input = std::make_shared<Input>("custom_block", Input::Type::NoShadow);
            input->setValueBlock(prototypeBlock);
            input->setInputId(CustomBlocks::CUSTOM_BLOCK);
            definitionBlock->addInput(input);
        }

        void addArgumentInput(std::shared_ptr<Block> block, const std::string &argId, const Value &value) const
        {
            auto input = std::make_shared<Input>(argId, Input::Type::Shadow);
            input->setPrimaryValue(value);
            block->addInput(input);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(CustomBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Custom blocks");
}

TEST_F(CustomBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(CustomBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "procedures_definition", &CustomBlocks::compileDefinition)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "procedures_call", &CustomBlocks::compileCall)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "argument_reporter_boolean", &CustomBlocks::compileArgument)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "argument_reporter_string_number", &CustomBlocks::compileArgument)).Times(1);

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "custom_block", CustomBlocks::CUSTOM_BLOCK));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "VALUE", CustomBlocks::VALUE));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(CustomBlocksTest, CustomBlocks)
{
    Compiler compiler(&m_engine);

    // define [ test (string or number) (boolean) ]
    // print (boolean)
    // print (invalid) - to test non-existent arguments (arguments can be dragged from another custom block definition)
    auto block1 = std::make_shared<Block>("a", "procedures_definition");
    auto prototypeBlock = std::make_shared<Block>("b", "procedures_prototype");
    auto prototype1 = prototypeBlock->mutationPrototype();
    prototype1->setProcCode("test %s %b");
    prototype1->setArgumentNames({ "string or number", "boolean" });
    prototype1->setArgumentIds({ "c", "d" });
    prototype1->setWarp(false);
    addPrototypeInput(block1, prototypeBlock);
    block1->setCompileFunction(&CustomBlocks::compileDefinition);

    auto testBlock = std::make_shared<Block>("e", "some_block");

    auto input = std::make_shared<Input>("test_input", Input::Type::ObscuredShadow);
    auto argBlock = std::make_shared<Block>("f", "argument_reporter_boolean");
    argBlock->setCompileFunction(&CustomBlocks::compileArgument);
    auto valueField = std::make_shared<Field>("VALUE", "boolean");
    valueField->setFieldId(CustomBlocks::VALUE);
    argBlock->addField(valueField);
    input->setValueBlock(argBlock);
    input->setInputId(-100);
    testBlock->addInput(input);

    input = std::make_shared<Input>("test_input2", Input::Type::ObscuredShadow);
    argBlock = std::make_shared<Block>("g", "argument_reporter_boolean");
    argBlock->setCompileFunction(&CustomBlocks::compileArgument);
    valueField = std::make_shared<Field>("VALUE", "invalid");
    valueField->setFieldId(CustomBlocks::VALUE);
    argBlock->addField(valueField);
    input->setValueBlock(argBlock);
    input->setInputId(-101);
    testBlock->addInput(input);

    testBlock->setCompileFunction([](Compiler *compiler) {
        compiler->addInput(-100);
        compiler->addInstruction(vm::OP_PRINT);
        compiler->addInput(-101);
        compiler->addInstruction(vm::OP_PRINT);
    });
    testBlock->setParent(block1);
    block1->setNext(testBlock);

    // define [ (a) + (b) ]
    // (run without screen refresh)
    auto block2 = std::make_shared<Block>("h", "procedures_definition");
    prototypeBlock = std::make_shared<Block>("i", "procedures_prototype");
    auto prototype2 = prototypeBlock->mutationPrototype();
    prototype2->setProcCode("%s + %s");
    prototype2->setArgumentNames({ "a", "b" });
    prototype2->setArgumentIds({ "j", "k" });
    prototype2->setWarp(true);
    addPrototypeInput(block2, prototypeBlock);
    block2->setCompileFunction(&CustomBlocks::compileDefinition);

    // call [ 5 + (null) ]
    auto block3 = std::make_shared<Block>("l", "procedures_call");
    auto prototype3 = block3->mutationPrototype();
    prototype3->setProcCode(prototype2->procCode());
    prototype3->setArgumentNames(prototype2->argumentNames());
    prototype3->setArgumentIds(prototype2->argumentIds());
    addArgumentInput(block3, "j", 5);
    block3->setCompileFunction(&CustomBlocks::compileCall);

    compiler.compile(block1);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_READ_ARG, 1, vm::OP_PRINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.procedureArgIndex("test %s %b", "string or number"), 0);
    ASSERT_EQ(compiler.procedureArgIndex("test %s %b", "boolean"), 1);
    ASSERT_EQ(compiler.procedureArgIndex("test %s %b", "invalid"), -1);
    ASSERT_EQ(compiler.procedureArgIndex("test %s %s", "boolean"), -1);
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_WARP, vm::OP_HALT }));
    ASSERT_EQ(compiler.procedureArgIndex("%s + %s", "a"), 0);
    ASSERT_EQ(compiler.procedureArgIndex("%s + %s", "b"), 1);
    ASSERT_EQ(compiler.procedureArgIndex("%s + %s", "invalid"), -1);
    ASSERT_EQ(compiler.procedureArgIndex("test %s %s", "a"), -1);
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_INIT_PROCEDURE, vm::OP_CONST, 0, vm::OP_ADD_ARG, vm::OP_NULL, vm::OP_ADD_ARG, vm::OP_CALL_PROCEDURE, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.procedureIndex("%s + %s"), 0);
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues(), std::vector<Value>{ 5 });
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}
