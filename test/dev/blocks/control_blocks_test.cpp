#include <scratchcpp/project.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/controlblocks.h"
#include "util.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class ControlBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<ControlBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            registerBlocks(m_engine, m_extension.get());
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(ControlBlocksTest, Forever)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_forever");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            testing::internal::CaptureStdout();
            m_engine->step();
            ASSERT_EQ(testing::internal::GetCapturedStdout().substr(0, 10), "test\ntest\n");
            ASSERT_TRUE(m_engine->isRunning());
        }
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_forever");

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            m_engine->step();
            ASSERT_TRUE(m_engine->isRunning());
        }
    }
}

TEST_F(ControlBlocksTest, Repeat)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_repeat");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addValueInput("TIMES", 5);

        builder.addBlock("control_repeat");
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addNullObscuredInput("TIMES");

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\ntest\ntest\ntest\n");
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_repeat");
        builder.addValueInput("TIMES", "Infinity");

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            m_engine->step();
            ASSERT_TRUE(m_engine->isRunning());
        }
    }
}

TEST_F(ControlBlocksTest, If)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_if");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addValueInput("CONDITION", false);
        builder.addObscuredInput("SUBSTACK", substack);

        builder.addBlock("control_if");
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addNullObscuredInput("CONDITION");
        builder.addObscuredInput("SUBSTACK", substack);

        builder.addBlock("control_if");
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addValueInput("CONDITION", true);
        builder.addObscuredInput("SUBSTACK", substack);

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_if");
        builder.addValueInput("CONDITION", true);

        builder.build();
        builder.run();
    }
}

TEST_F(ControlBlocksTest, IfElse)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", false);
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        auto substack2 = std::make_shared<Block>("", "test_print_test2");
        builder.addObscuredInput("SUBSTACK2", substack2);

        builder.addBlock("control_if_else");
        builder.addNullObscuredInput("CONDITION");
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        substack2 = std::make_shared<Block>("", "test_print_test2");
        builder.addObscuredInput("SUBSTACK2", substack2);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", true);
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        substack2 = std::make_shared<Block>("", "test_print_test2");
        builder.addObscuredInput("SUBSTACK2", substack2);

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "test2\ntest2\ntest\n");
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", false);
        auto substack2 = std::make_shared<Block>("", "test_print_test2");
        builder.addObscuredInput("SUBSTACK2", substack2);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", true);
        substack2 = std::make_shared<Block>("", "test_print_test2");
        builder.addObscuredInput("SUBSTACK2", substack2);

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "test2\n");
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", false);
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", true);
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", false);

        builder.addBlock("control_if_else");
        builder.addValueInput("CONDITION", true);

        builder.build();
        builder.run();
    }
}
