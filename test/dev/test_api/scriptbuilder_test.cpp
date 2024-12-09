#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/block.h>
#include <scratchcpp/list.h>

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
    m_builder->addBlock("test_simple");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
}

TEST_F(ScriptBuilderTest, AddValueInput)
{
    m_builder->addBlock("test_print");
    m_builder->addValueInput("STRING", 10);
    m_builder->addBlock("test_print");
    m_builder->addValueInput("STRING", "Hello world");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "10\nHello world\n");
}

TEST_F(ScriptBuilderTest, AddNullInput)
{
    m_builder->addBlock("test_print");
    m_builder->addNullInput("STRING");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n");
}

TEST_F(ScriptBuilderTest, AddObscuredInput)
{
    m_builder->addBlock("test_print");
    auto block = std::make_shared<Block>("", "test_teststr");
    m_builder->addObscuredInput("STRING", block);
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
}

TEST_F(ScriptBuilderTest, AddNullObscuredInput)
{
    m_builder->addBlock("test_print");
    m_builder->addNullObscuredInput("STRING");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n");
}

TEST_F(ScriptBuilderTest, AddDropdownInput)
{
    m_builder->addBlock("test_print_dropdown");
    m_builder->addDropdownInput("STRING", "hello");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\n");
}

TEST_F(ScriptBuilderTest, AddDropdownField)
{
    m_builder->addBlock("test_print_field");
    m_builder->addDropdownField("STRING", "hello");
    m_builder->build();

    testing::internal::CaptureStdout();
    m_builder->run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\n");
}

TEST_F(ScriptBuilderTest, ReporterBlocks)
{
    m_builder->addReporterBlock("test_teststr");
    m_builder->captureBlockReturnValue();

    m_builder->addReporterBlock("test_input");
    m_builder->addValueInput("INPUT", -93.4);
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
