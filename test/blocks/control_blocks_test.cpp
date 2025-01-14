#include <scratchcpp/project.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/variable.h>

#include <enginemock.h>
#include <stacktimermock.h>

#include "../common.h"
#include "blocks/controlblocks.h"
#include "util.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;
using ::testing::SaveArg;
using ::testing::_;

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

TEST_F(ControlBlocksTest, Stop)
{
    auto target = std::make_shared<Sprite>();

    // Stop all
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_stop");
        builder.addDropdownField("STOP_OPTION", "all");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, stop());
        thread.run();
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    // Stop this script
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_stop");
        builder.addDropdownField("STOP_OPTION", "this script");
        builder.addBlock("test_print_test");

        builder.build();

        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_TRUE(testing::internal::GetCapturedStdout().empty());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    // Stop other scripts in sprite
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_stop");
        builder.addDropdownField("STOP_OPTION", "other scripts in sprite");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, stopTarget(target.get(), &thread));
        thread.run();
    }

    // Stop other scripts in stage
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_stop");
        builder.addDropdownField("STOP_OPTION", "other scripts in stage");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, stopTarget(target.get(), &thread));
        thread.run();
    }
}

TEST_F(ControlBlocksTest, Wait)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_wait");
        builder.addValueInput("DURATION", 2.5);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(false));
        EXPECT_CALL(m_engineMock, requestRedraw()).Times(0);
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(false));
        EXPECT_CALL(m_engineMock, requestRedraw()).Times(0);
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw()).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_wait");
        builder.addNullObscuredInput("DURATION");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(0.0));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw()).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(ControlBlocksTest, WaitUntil)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_wait_until");
        builder.addValueInput("CONDITION", false);
        builder.build();
        m_engine->start();

        m_engine->step();
        ASSERT_TRUE(m_engine->isRunning());

        m_engine->step();
        ASSERT_TRUE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_wait_until");
        builder.addValueInput("CONDITION", true);
        builder.build();
        m_engine->start();

        m_engine->step();
        m_engine->step();
        ASSERT_FALSE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_wait_until");
        auto block = std::make_shared<Block>("", "test_condition");
        builder.addObscuredInput("CONDITION", block);
        builder.build();

        conditionReturnValue = false;
        m_engine->start();

        m_engine->step();
        ASSERT_TRUE(m_engine->isRunning());

        m_engine->step();
        ASSERT_TRUE(m_engine->isRunning());

        conditionReturnValue = true;
        m_engine->step();
        m_engine->step();
        ASSERT_FALSE(m_engine->isRunning());
    }
}

TEST_F(ControlBlocksTest, RepeatUntil)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_repeat_until");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addValueInput("CONDITION", false);
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

        builder.addBlock("control_repeat_until");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addValueInput("CONDITION", true);
        builder.build();
        m_engine->start();

        testing::internal::CaptureStdout();
        m_engine->step();
        m_engine->step();
        ASSERT_TRUE(testing::internal::GetCapturedStdout().empty());
        ASSERT_FALSE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_repeat_until");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        auto block = std::make_shared<Block>("", "test_condition");
        builder.addObscuredInput("CONDITION", block);
        builder.build();

        conditionReturnValue = false;
        m_engine->start();

        testing::internal::CaptureStdout();
        m_engine->step();
        ASSERT_EQ(testing::internal::GetCapturedStdout().substr(0, 10), "test\ntest\n");
        ASSERT_TRUE(m_engine->isRunning());

        conditionReturnValue = true;
        m_engine->step();
        m_engine->step();
        ASSERT_FALSE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_repeat_until");
        builder.addValueInput("CONDITION", false);

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            m_engine->step();
            ASSERT_TRUE(m_engine->isRunning());
        }
    }
}

TEST_F(ControlBlocksTest, While)
{
    auto target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_while");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addValueInput("CONDITION", true);
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

        builder.addBlock("control_while");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addValueInput("CONDITION", false);
        builder.build();
        m_engine->start();

        testing::internal::CaptureStdout();
        m_engine->step();
        m_engine->step();
        ASSERT_TRUE(testing::internal::GetCapturedStdout().empty());
        ASSERT_FALSE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_while");
        auto substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        auto block = std::make_shared<Block>("", "test_condition");
        builder.addObscuredInput("CONDITION", block);
        builder.build();

        conditionReturnValue = true;
        m_engine->start();

        testing::internal::CaptureStdout();
        m_engine->step();
        ASSERT_EQ(testing::internal::GetCapturedStdout().substr(0, 10), "test\ntest\n");
        ASSERT_TRUE(m_engine->isRunning());

        conditionReturnValue = false;
        m_engine->step();
        m_engine->step();
        ASSERT_FALSE(m_engine->isRunning());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_while");
        builder.addValueInput("CONDITION", true);

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            m_engine->step();
            ASSERT_TRUE(m_engine->isRunning());
        }
    }
}

TEST_F(ControlBlocksTest, ForEach)
{
    auto target = std::make_shared<Sprite>();
    auto var1 = std::make_shared<Variable>("", "");
    auto var2 = std::make_shared<Variable>("", "");
    target->addVariable(var1);
    target->addVariable(var2);

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_for_each");

        auto substack = std::make_shared<Block>("", "test_print");
        auto input = std::make_shared<Input>("STRING", Input::Type::ObscuredShadow);
        input->primaryValue()->setValuePtr(var1);
        substack->addInput(input);

        builder.addObscuredInput("SUBSTACK", substack);

        builder.addValueInput("VALUE", 5);
        builder.addEntityField("VARIABLE", var1);

        builder.addBlock("control_for_each");
        substack = std::make_shared<Block>("", "test_print_test");
        builder.addObscuredInput("SUBSTACK", substack);
        builder.addNullObscuredInput("VALUE");
        builder.addEntityField("VARIABLE", var2);

        builder.build();

        var1->setValue(10);
        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n4\n5\n");
        ASSERT_EQ(var1->value(), 5);
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->addVariable(var1);

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_for_each");

        auto substack = std::make_shared<Block>("", "test_print");
        auto input = std::make_shared<Input>("STRING", Input::Type::ObscuredShadow);
        input->primaryValue()->setValuePtr(var1);
        substack->addInput(input);

        auto setVar = std::make_shared<Block>("", "test_set_var");
        substack->setNext(setVar);
        setVar->setParent(substack);
        auto field = std::make_shared<Field>("VARIABLE", "");
        setVar->addField(field);
        input = std::make_shared<Input>("VALUE", Input::Type::Shadow);
        input->setPrimaryValue(0);
        setVar->addInput(input);

        auto printAgain = std::make_shared<Block>("", "test_print");
        setVar->setNext(printAgain);
        printAgain->setParent(setVar);
        input = std::make_shared<Input>("STRING", Input::Type::ObscuredShadow);
        printAgain->addInput(input);

        builder.addObscuredInput("SUBSTACK", substack);

        builder.addValueInput("VALUE", 3);
        builder.addEntityField("VARIABLE", var1);

        field->setValuePtr(var1);
        input->primaryValue()->setValuePtr(var1);

        builder.build();

        var1->setValue(7);
        testing::internal::CaptureStdout();
        builder.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n0\n2\n0\n3\n0\n");
        ASSERT_EQ(var1->value(), 0);
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->addVariable(var1);

    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);
        builder.addBlock("control_for_each");
        builder.addValueInput("VALUE", "Infinity");
        builder.addEntityField("VARIABLE", var1);

        builder.build();
        m_engine->start();

        for (int i = 0; i < 2; i++) {
            m_engine->step();
            ASSERT_TRUE(m_engine->isRunning());
        }

        ASSERT_GT(var1->value(), 0);
    }
}

TEST_F(ControlBlocksTest, StartAsClone)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("control_start_as_clone");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addCloneInitScript(block));
    compiler.compile(block);
}

TEST_F(ControlBlocksTest, CreateCloneOfSprite)
{
    EXPECT_CALL(m_engineMock, cloneLimit()).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
    auto target = std::make_shared<Sprite>();
    target->setEngine(&m_engineMock);

    // create clone of [Sprite1]
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addDropdownInput("CLONE_OPTION", "Sprite1");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("Sprite1")).WillOnce(Return(4));
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Sprite sprite;
        sprite.setEngine(&m_engineMock);
        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, targetAt(4)).WillOnce(Return(&sprite));
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, &sprite));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), &sprite);
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->setEngine(&m_engineMock);

    // create clone of [myself]
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addDropdownInput("CLONE_OPTION", "_myself_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, target.get()));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), target.get());
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->setEngine(&m_engineMock);

    // create clone of ["_mYself_"]
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addDropdownInput("CLONE_OPTION", "_mYself_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("_mYself_")).WillOnce(Return(4));
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Sprite sprite;
        sprite.setEngine(&m_engineMock);
        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, targetAt(4)).WillOnce(Return(&sprite));
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, &sprite));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), &sprite);
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->setEngine(&m_engineMock);

    // create clone of (null block)
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addNullObscuredInput("CLONE_OPTION");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Sprite sprite;
        sprite.setEngine(&m_engineMock);
        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, findTarget("0")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillOnce(Return(&sprite));
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, &sprite));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), &sprite);
    }

    m_engine->clear();
    target = std::make_shared<Sprite>();
    target->setEngine(&m_engineMock);

    // create clone of ("_myself_")
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        auto valueBlock = std::make_shared<Block>("", "test_input");
        auto input = std::make_shared<Input>("INPUT", Input::Type::Shadow);
        input->setPrimaryValue("_myself_");
        valueBlock->addInput(input);
        builder.addObscuredInput("CLONE_OPTION", valueBlock);
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, target.get()));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), target.get());
    }

    // create clone of ("_mYself_")
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        auto valueBlock = std::make_shared<Block>("", "test_input");
        auto input = std::make_shared<Input>("INPUT", Input::Type::Shadow);
        input->setPrimaryValue("_mYself_");
        valueBlock->addInput(input);
        builder.addObscuredInput("CLONE_OPTION", valueBlock);
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Sprite sprite;
        sprite.setEngine(&m_engineMock);
        std::shared_ptr<Sprite> clone;
        EXPECT_CALL(m_engineMock, findTarget("_mYself_")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillOnce(Return(&sprite));
        EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
        EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, &sprite));
        thread.run();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->cloneSprite(), &sprite);
    }
}

TEST_F(ControlBlocksTest, CreateCloneOfStage)
{
    EXPECT_CALL(m_engineMock, cloneLimit()).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
    auto target = std::make_shared<Stage>();
    target->setEngine(&m_engineMock);

    // create clone of [Stage]
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addDropdownInput("CLONE_OPTION", "_stage_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(8));
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Stage stage;
        stage.setEngine(&m_engineMock);
        EXPECT_CALL(m_engineMock, targetAt(8)).WillOnce(Return(&stage));
        EXPECT_CALL(m_engineMock, initClone).Times(0);
        thread.run();
    }

    m_engine->clear();
    target = std::make_shared<Stage>();
    target->setEngine(&m_engineMock);

    // create clone of [myself]
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addDropdownInput("CLONE_OPTION", "_myself_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, initClone).Times(0);
        thread.run();
    }

    m_engine->clear();
    target = std::make_shared<Stage>();
    target->setEngine(&m_engineMock);

    // create clone of (null block)
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        builder.addNullObscuredInput("CLONE_OPTION");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        Stage stage;
        stage.setEngine(&m_engineMock);
        EXPECT_CALL(m_engineMock, findTarget("0")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillOnce(Return(&stage));
        EXPECT_CALL(m_engineMock, initClone).Times(0);
        thread.run();
    }

    m_engine->clear();
    target = std::make_shared<Stage>();
    target->setEngine(&m_engineMock);

    // create clone of ("_myself_")
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target);

        builder.addBlock("control_create_clone_of");
        auto valueBlock = std::make_shared<Block>("", "test_input");
        auto input = std::make_shared<Input>("INPUT", Input::Type::Shadow);
        input->setPrimaryValue("_myself_");
        valueBlock->addInput(input);
        builder.addObscuredInput("CLONE_OPTION", valueBlock);
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block);
        Script script(target.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget).Times(0);
        EXPECT_CALL(m_engineMock, initClone).Times(0);
        thread.run();
    }
}

TEST_F(ControlBlocksTest, DeleteThisClone)
{
    Sprite sprite;
    sprite.setEngine(&m_engineMock);

    std::shared_ptr<Sprite> clone;
    EXPECT_CALL(m_engineMock, cloneLimit()).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
    EXPECT_CALL(m_engineMock, moveDrawableBehindOther(_, &sprite));
    EXPECT_CALL(m_engineMock, requestRedraw());
    sprite.clone();
    ASSERT_TRUE(clone);

    ScriptBuilder builder(m_extension.get(), m_engine, clone);

    builder.addBlock("control_delete_this_clone");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, clone.get());
    auto code = compiler.compile(block);
    Script script(clone.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(clone.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, stopTarget(clone.get(), nullptr));
    EXPECT_CALL(m_engineMock, deinitClone(clone));
    thread.run();
}

TEST_F(ControlBlocksTest, DeleteThisCloneStage)
{
    auto target = std::make_shared<Stage>();
    target->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("control_delete_this_clone");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, stopTarget).Times(0);
    EXPECT_CALL(m_engineMock, deinitClone).Times(0);
    thread.run();
}
