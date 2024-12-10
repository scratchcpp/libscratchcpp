#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>
#include <scratchcpp/broadcast.h>

#include "../../common.h"
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
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_simple");
    ASSERT_TRUE(block->compileFunction());

    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
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

TEST_F(ScriptBuilderTest, AddEntityField)
{
    auto broadcast = std::make_shared<Broadcast>("", "");
    m_engine->setBroadcasts({ broadcast });

    m_builder->addBlock("test_print_field");
    m_builder->addEntityField("STRING", broadcast);
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_print_field");
    ASSERT_TRUE(block->inputs().empty());
    ASSERT_EQ(block->fields().size(), 1);
    ASSERT_EQ(block->fieldAt(0)->name(), "STRING");
    ASSERT_EQ(block->fieldAt(0)->valuePtr(), broadcast);
}

TEST_F(ScriptBuilderTest, ReporterBlocks)
{
    m_builder->addReporterBlock("test_teststr");
    auto block = m_builder->currentBlock();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->opcode(), "test_teststr");
    m_builder->captureBlockReturnValue();

    m_builder->addReporterBlock("test_input");
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
