#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <dev/engine/compiler_p.h>
#include <enginemock.h>
#include <targetmock.h>
#include <codebuilderfactorymock.h>
#include <codebuildermock.h>
#include <executablecodemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;

class CompilerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            CompilerPrivate::builderFactory = &m_builderFactory;
            m_builder = std::make_shared<CodeBuilderMock>();
            m_code = std::make_shared<ExecutableCodeMock>();
        }

        void TearDown() override
        {
            CompilerPrivate::builderFactory = nullptr;
            m_builder.reset();
            m_compareBlock.reset();
            m_testVar.reset();
        }

        void compile(Compiler &compiler, std::shared_ptr<Block> block)
        {
            ASSERT_EQ(compiler.block(), nullptr);
            EXPECT_CALL(m_builderFactory, create(block->id())).WillOnce(Return(m_builder));
            EXPECT_CALL(*m_builder, finalize()).WillOnce(Return(m_code));
            ASSERT_EQ(compiler.compile(block), m_code);
            ASSERT_EQ(compiler.block(), nullptr);
        }

        EngineMock m_engine;
        TargetMock m_target;
        CodeBuilderFactoryMock m_builderFactory;
        static inline std::shared_ptr<CodeBuilderMock> m_builder;
        std::shared_ptr<ExecutableCodeMock> m_code;
        static inline std::shared_ptr<Block> m_compareBlock;
        static inline std::shared_ptr<Variable> m_testVar;
};

TEST_F(CompilerTest, Constructors)
{
    Compiler compiler(&m_engine, &m_target);
    ASSERT_EQ(compiler.engine(), &m_engine);
    ASSERT_EQ(compiler.target(), &m_target);
}

TEST_F(CompilerTest, AddFunctionCall)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    m_compareBlock = block;
    block->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->block(), m_compareBlock);
        EXPECT_CALL(*m_builder, addFunctionCall("test1", 3, false));
        compiler->addFunctionCall("test1", 3, false);

        EXPECT_CALL(*m_builder, addFunctionCall("test2", 0, true));
        compiler->addFunctionCall("test2", 0, true);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddConstValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->addConstValue(1);

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addConstValue("test");

        EXPECT_CALL(*m_builder, addConstValue(Value(3.5)));
        compiler->addConstValue(3.5);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddVariableValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        Variable var1("", ""), var2("", "");
        EXPECT_CALL(*m_builder, addVariableValue(&var1));
        compiler->addVariableValue(&var1);

        EXPECT_CALL(*m_builder, addVariableValue(&var2));
        compiler->addVariableValue(&var2);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddListValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        List list1("", ""), list2("", "");
        EXPECT_CALL(*m_builder, addListContents(&list1));
        compiler->addListContents(&list1);

        EXPECT_CALL(*m_builder, addListContents(&list2));
        compiler->addListContents(&list2);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddInput)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    auto valueBlock = std::make_shared<Block>("b", "");
    m_compareBlock = valueBlock;
    valueBlock->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->block(), m_compareBlock);
        compiler->addConstValue("value block");
    });

    auto menu = std::make_shared<Block>("c", "");
    menu->setShadow(true);
    menu->addField(std::make_shared<Field>("MENU", "selected item"));

    auto shadow = std::make_shared<Input>("SHADOW", Input::Type::Shadow);
    shadow->setPrimaryValue("test");
    block->addInput(shadow);

    auto shadowBlock = std::make_shared<Input>("SHADOW_BLOCK", Input::Type::Shadow);
    shadowBlock->setValueBlock(valueBlock);
    block->addInput(shadowBlock);

    auto shadowMenu = std::make_shared<Input>("SHADOW_MENU", Input::Type::Shadow);
    shadowMenu->setValueBlock(menu);
    block->addInput(shadowMenu);

    auto noShadow = std::make_shared<Input>("NO_SHADOW", Input::Type::NoShadow);
    noShadow->setPrimaryValue("test");
    block->addInput(noShadow);

    auto noShadowBlock = std::make_shared<Input>("NO_SHADOW_BLOCK", Input::Type::NoShadow);
    noShadowBlock->setValueBlock(valueBlock);
    block->addInput(noShadowBlock);

    auto noShadowMenu = std::make_shared<Input>("NO_SHADOW_MENU", Input::Type::Shadow);
    noShadowMenu->setValueBlock(menu);
    block->addInput(noShadowMenu);

    auto obscuredShadow = std::make_shared<Input>("OBSCURED_SHADOW", Input::Type::ObscuredShadow);
    obscuredShadow->setPrimaryValue("test");
    block->addInput(obscuredShadow);

    auto obscuredShadowVariable = std::make_shared<Input>("OBSCURED_SHADOW_VARIABLE", Input::Type::ObscuredShadow);
    m_testVar = std::make_shared<Variable>("", "");
    obscuredShadowVariable->primaryValue()->setValuePtr(m_testVar);
    block->addInput(obscuredShadowVariable);

    auto obscuredShadowBlock = std::make_shared<Input>("OBSCURED_SHADOW_BLOCK", Input::Type::ObscuredShadow);
    obscuredShadowBlock->setValueBlock(valueBlock);
    block->addInput(obscuredShadowBlock);

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, addConstValue(Value()));
        compiler->addInput("INVALID");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("SHADOW");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("SHADOW_BLOCK");

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item")));
        compiler->addInput("SHADOW_MENU");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("NO_SHADOW");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("NO_SHADOW_BLOCK");

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item")));
        compiler->addInput("NO_SHADOW_MENU");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("OBSCURED_SHADOW");

        EXPECT_CALL(*m_builder, addVariableValue(m_testVar.get()));
        compiler->addInput("OBSCURED_SHADOW_VARIABLE");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("OBSCURED_SHADOW_BLOCK");
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, Field)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->addField(std::make_shared<Field>("TEST", "test"));
    block->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->field("INVALID"), nullptr);
        ASSERT_EQ(compiler->field("TEST"), compiler->block()->fieldAt(0).get());
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, UnsupportedBlocks)
{
    auto valueBlock1 = std::make_shared<Block>("v1", "value_block1");
    auto valueBlock2 = std::make_shared<Block>("v2", "value_block2");
    valueBlock2->setCompileFunction([](Compiler *) {});

    auto valueBlock3 = std::make_shared<Block>("v3", "value_block3");
    auto valueBlock4 = std::make_shared<Block>("v4", "value_block4");
    valueBlock4->setCompileFunction([](Compiler *) {});

    auto valueBlock5 = std::make_shared<Block>("v5", "value_block5");
    auto valueBlock6 = std::make_shared<Block>("v6", "value_block6");
    valueBlock6->setCompileFunction([](Compiler *) {});

    auto block1 = std::make_shared<Block>("b1", "block1");

    auto block2 = std::make_shared<Block>("b2", "block2");
    block2->setParent(block1);
    block1->setNext(block2);

    auto block3 = std::make_shared<Block>("b3", "block3");
    auto input1 = std::make_shared<Input>("INPUT1", Input::Type::Shadow);
    input1->setValueBlock(valueBlock1);
    block3->addInput(input1);
    auto input2 = std::make_shared<Input>("INPUT2", Input::Type::Shadow);
    input2->setValueBlock(valueBlock2);
    block3->addInput(input2);
    auto input3 = std::make_shared<Input>("INPUT3", Input::Type::NoShadow);
    input3->setValueBlock(valueBlock3);
    block3->addInput(input3);
    auto input4 = std::make_shared<Input>("INPUT4", Input::Type::NoShadow);
    input4->setValueBlock(valueBlock4);
    block3->addInput(input4);
    auto input5 = std::make_shared<Input>("INPUT5", Input::Type::ObscuredShadow);
    input5->setValueBlock(valueBlock5);
    block3->addInput(input5);
    auto input6 = std::make_shared<Input>("INPUT6", Input::Type::ObscuredShadow);
    input6->setValueBlock(valueBlock6);
    block3->addInput(input6);
    block3->setCompileFunction([](Compiler *compiler) {
        compiler->addInput("INPUT1");
        compiler->addInput("INPUT2");
        compiler->addInput("INPUT3");
        compiler->addInput("INPUT4");
        compiler->addInput("INPUT5");
        compiler->addInput("INPUT6");
    });
    block3->setParent(block2);
    block2->setNext(block3);

    auto block4 = std::make_shared<Block>("b4", "block4");
    block4->setParent(block3);
    block3->setNext(block4);

    Compiler compiler(&m_engine, &m_target);
    EXPECT_CALL(*m_builder, addConstValue).WillRepeatedly(Return());
    compile(compiler, block1);

    ASSERT_EQ(compiler.unsupportedBlocks(), std::unordered_set<std::string>({ "block1", "block2", "value_block1", "value_block3", "value_block5", "block4" }));
}
