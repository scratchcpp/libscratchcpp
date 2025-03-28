#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/broadcast.h>

#include "../common.h"
#include "testextension.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class ScriptBuilderTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_engine = m_project.engine().get();
            m_extension.registerBlocks(m_engine);
            m_target = std::make_shared<Sprite>();
            m_builder = std::make_unique<ScriptBuilder>(&m_extension, m_engine, m_target);
        }

        Project m_project;
        IEngine *m_engine = nullptr;
        std::shared_ptr<Target> m_target;
        TestExtension m_extension;
        std::unique_ptr<ScriptBuilder> m_builder;
};

TEST_F(ScriptBuilderTest, AddBlock)
{
    ASSERT_EQ(m_builder->currentBlock(), nullptr);

    m_builder->addBlock("test_simple");
    Block *block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->compileFunction());

    m_builder->addBlock(std::make_shared<Block>("", "test_simple"));
    block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->compileFunction());

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\n");
}

TEST_F(ScriptBuilderTest, AddValueInput)
{
    m_builder->addBlock("test_print");
    m_builder->addValueInput("STRING", 10);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->addBlock("test_print");
    m_builder->addValueInput("STRING", "Hello world");
    block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "10\nHello world\n");
}

TEST_F(ScriptBuilderTest, AddNullInput)
{
    m_builder->addBlock("test_print");
    m_builder->addNullInput("STRING");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n");
}

TEST_F(ScriptBuilderTest, AddObscuredInput)
{
    m_builder->addBlock("test_print");
    auto valueBlock = std::make_shared<Block>("", "test_teststr");
    m_builder->addObscuredInput("STRING", valueBlock);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
}

TEST_F(ScriptBuilderTest, AddObscuredInputMultipleBlocks)
{
    m_builder->addBlock("test_substack");
    auto substack = std::make_shared<Block>("", "test_simple");
    auto block1 = std::make_shared<Block>("", "test_simple");
    substack->setNext(block1.get());
    block1->setParent(substack.get());
    auto block2 = std::make_shared<Block>("", "test_simple");
    block1->setNext(block2.get());
    block2->setParent(block1.get());
    m_builder->addObscuredInput("SUBSTACK", substack);

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\ntest\n");
}

TEST_F(ScriptBuilderTest, AdvancedObscuredInput)
{
    for (int i = 1; i <= 3; i++) {
        m_builder->addBlock("test_input");
        m_builder->addValueInput("INPUT", i);
        auto valueBlock = m_builder->takeBlock();

        m_builder->addBlock("test_input");
        m_builder->addObscuredInput("INPUT", valueBlock);
        valueBlock = m_builder->takeBlock();

        m_builder->addBlock("test_print");
        m_builder->addObscuredInput("STRING", valueBlock);
    }

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
}

TEST_F(ScriptBuilderTest, AddNullObscuredInput)
{
    m_builder->addBlock("test_print");
    m_builder->addNullObscuredInput("STRING");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n");
}

TEST_F(ScriptBuilderTest, AddDropdownInput)
{
    m_builder->addBlock("test_print_dropdown");
    m_builder->addDropdownInput("STRING", "hello");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print_dropdown");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\n");
}

TEST_F(ScriptBuilderTest, AddDropdownField)
{
    m_builder->addBlock("test_print_field");
    m_builder->addDropdownField("STRING", "hello");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print_field");
    ASSERT_TRUE(block->inputs().empty());
    ASSERT_EQ(block->fields().size(), 1);
    ASSERT_EQ(block->fieldAt(0)->name(), "STRING");

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\n");
}

TEST_F(ScriptBuilderTest, AddEntityInput)
{
    auto var = std::make_shared<Variable>("", "");
    m_target->addVariable(var);

    m_builder->addBlock("test_simple");
    m_builder->addEntityInput("VARIABLE", "test", InputValue::Type::Variable, var);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "VARIABLE");
    ASSERT_EQ(block->inputAt(0)->primaryValue()->valuePtr(), var);
    ASSERT_EQ(block->inputAt(0)->primaryValue()->type(), InputValue::Type::Variable);

    m_builder->addBlock("test_simple");
    m_builder->addEntityInput("VARIABLE", "test", InputValue::Type::Variable, var);
    m_builder->build();
}

TEST_F(ScriptBuilderTest, AddVariableEntityField)
{
    auto var = std::make_shared<Variable>("", "test var");
    m_target->addVariable(var);

    m_builder->addBlock("test_simple");
    m_builder->addEntityField("VARIABLE", var);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->inputs().empty());
    ASSERT_EQ(block->fields().size(), 1);
    ASSERT_EQ(block->fieldAt(0)->name(), "VARIABLE");
    ASSERT_EQ(block->fieldAt(0)->value().toString(), "test var");
    ASSERT_EQ(block->fieldAt(0)->valuePtr(), var);

    m_builder->addBlock("test_simple");
    m_builder->addEntityField("VARIABLE", var);
    m_builder->build();
}

TEST_F(ScriptBuilderTest, AddListEntityField)
{
    auto list = std::make_shared<List>("", "hello world");
    m_target->addList(list);

    m_builder->addBlock("test_simple");
    m_builder->addEntityField("LIST", list);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->inputs().empty());
    ASSERT_EQ(block->fields().size(), 1);
    ASSERT_EQ(block->fieldAt(0)->name(), "LIST");
    ASSERT_EQ(block->fieldAt(0)->value().toString(), "hello world");
    ASSERT_EQ(block->fieldAt(0)->valuePtr(), list);
}

TEST_F(ScriptBuilderTest, AddBroadcastEntityField)
{
    auto broadcast = std::make_shared<Broadcast>("", "test");
    m_engine->setBroadcasts({ broadcast });

    m_builder->addBlock("test_simple");
    m_builder->addEntityField("MESSAGE", broadcast);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->inputs().empty());
    ASSERT_EQ(block->fields().size(), 1);
    ASSERT_EQ(block->fieldAt(0)->name(), "MESSAGE");
    ASSERT_EQ(block->fieldAt(0)->value().toString(), "test");
    ASSERT_EQ(block->fieldAt(0)->valuePtr(), broadcast);
}

TEST_F(ScriptBuilderTest, CaptureBlockReturnValue)
{
    m_builder->addBlock("test_teststr");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_teststr");
    m_builder->captureBlockReturnValue();

    m_builder->addBlock("test_input");
    m_builder->addValueInput("INPUT", -93.4);
    block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_input");
    ASSERT_EQ(block->inputs().size(), 1);
    ASSERT_EQ(block->inputAt(0)->name(), "INPUT");
    m_builder->captureBlockReturnValue();

    m_builder->build();
    m_builder->run();

    List *values = m_builder->capturedValues();
    ASSERT_TRUE(values);
    ASSERT_EQ(values->size(), 2);
    std::string str;
    value_toString(&values->operator[](0), &str);
    ASSERT_EQ(str, "test");
    ASSERT_EQ(value_toDouble(&values->operator[](1)), -93.4);
}

TEST_F(ScriptBuilderTest, MultipleScripts)
{
    ScriptBuilder builder1(&m_extension, m_engine, m_target, false);
    builder1.addBlock("test_click_hat");
    builder1.addBlock("test_simple");

    ScriptBuilder builder2(&m_extension, m_engine, m_target);
    builder2.addBlock("test_print");
    builder2.addValueInput("STRING", "Hello world");

    Project project;
    IEngine *engine = project.engine().get();
    m_extension.registerBlocks(engine);
    auto target = std::make_shared<Stage>();
    ScriptBuilder builder3(&m_extension, engine, target);
    builder3.addBlock("test_simple");

    ScriptBuilder::buildMultiple({ &builder1, &builder2, &builder3 });

    testing::internal::CaptureStdout();
    builder2.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Hello world\n");

    testing::internal::CaptureStdout();
    builder3.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
}
