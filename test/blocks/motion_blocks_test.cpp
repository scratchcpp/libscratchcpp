#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <enginemock.h>
#include <randomgeneratormock.h>
#include <stacktimermock.h>

#include "../common.h"
#include "blocks/motionblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class MotionBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<MotionBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(MotionBlocksTest, MoveSteps)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_movesteps");
        builder.addValueInput("STEPS", 30.25);

        sprite->setX(5.2);
        sprite->setY(-0.25);
        sprite->setDirection(-61.42);

        builder.build();
        builder.run();
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, -21.36);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, 14.22);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_movesteps");
        builder.addValueInput("STEPS", 30.25);

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, TurnRight)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_turnright");
        builder.addValueInput("DEGREES", 12.05);

        sprite->setDirection(124.37);

        builder.build();
        builder.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 136.42);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_turnright");
        builder.addValueInput("DEGREES", 12.05);

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, TurnLeft)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_turnleft");
        builder.addValueInput("DEGREES", 12.05);

        sprite->setDirection(124.37);

        builder.build();
        builder.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 112.32);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_turnleft");
        builder.addValueInput("DEGREES", 12.05);

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, PointInDirection)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_pointindirection");
        builder.addValueInput("DIRECTION", -60.5);

        sprite->setDirection(50.02);

        builder.build();
        builder.run();
        ASSERT_EQ(sprite->direction(), -60.5);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_pointindirection");
        builder.addValueInput("DIRECTION", -60.5);

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, PointTowardsMouse)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-45.12));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-123.48));
        thread.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, -101.51);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(125.23));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-3.21));
        thread.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 29.66);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "_mouse_");
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "_mouse_");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, PointTowardsRandomPosition)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        ctx->setRng(&rng);

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(95.2));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-100.025));
        code->run(ctx.get());
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 90);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        ctx->setRng(&rng);

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(-21.28));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-100.025));
        code->run(ctx.get());
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, -90);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "_random_");
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "_random_");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, PointTowardsSprite)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(-45.12);
        anotherSprite->setY(-123.48);
        anotherSprite->setDirection(-2.5);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(anotherSprite.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, -101.51);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(nullptr));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(sprite->direction(), 21.58);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "_stage_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(sprite->direction(), 21.58);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(125.23);
        anotherSprite->setY(-3.21);
        anotherSprite->setDirection(82.54);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillOnce(Return(anotherSprite.get()));
        thread.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 29.66);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillOnce(Return(nullptr));
        thread.run();
        ASSERT_EQ(sprite->direction(), 21.58);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);
        sprite->setDirection(21.58);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "_stage_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        thread.run();
        ASSERT_EQ(std::round(sprite->direction() * 100) / 100, 21.58);
    }

    {
        auto stage = std::make_shared<Stage>();

        auto sprite = std::make_shared<Sprite>();
        sprite->setName("Test");
        sprite->setEngine(&m_engineMock);
        sprite->setX(153.2);
        sprite->setY(59.27);
        sprite->setDirection(-21.58);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_pointtowards");
        builder.addDropdownInput("TOWARDS", "Test");
        builder.addBlock("motion_pointtowards");
        builder.addValueInput("TOWARDS", "Test");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, GoToXY)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_gotoxy");
        builder.addValueInput("X", -55.2);
        builder.addValueInput("Y", 23.254);

        sprite->setX(51.28);
        sprite->setY(-0.5);

        builder.build();
        builder.run();
        ASSERT_EQ(sprite->x(), -55.2);
        ASSERT_EQ(sprite->y(), 23.254);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_gotoxy");
        builder.addValueInput("X", -55.2);
        builder.addValueInput("Y", 23.254);

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, GoToMouse)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-45.12));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-123.48));
        thread.run();
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(125.23));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-3.21));
        thread.run();
        ASSERT_EQ(sprite->x(), 125.23);
        ASSERT_EQ(sprite->y(), -3.21);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "_mouse_");
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "_mouse_");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, GoToRandomPosition)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        ctx->setRng(&rng);

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(95.2));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-100.025));
        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), 95.2);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        ctx->setRng(&rng);

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(-21.28));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-100.025));
        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), -21.28);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "_random_");
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "_random_");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, GoToSprite)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(-45.12);
        anotherSprite->setY(-123.48);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(anotherSprite.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(nullptr));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "_stage_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        thread.run();
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(125.23);
        anotherSprite->setY(-3.21);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillOnce(Return(anotherSprite.get()));
        thread.run();
        ASSERT_EQ(sprite->x(), 125.23);
        ASSERT_EQ(sprite->y(), -3.21);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillOnce(Return(nullptr));
        thread.run();
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "_stage_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        thread.run();
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    {
        auto stage = std::make_shared<Stage>();

        auto sprite = std::make_shared<Sprite>();
        sprite->setName("Test");
        sprite->setEngine(&m_engineMock);
        sprite->setX(153.2);
        sprite->setY(59.27);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_goto");
        builder.addDropdownInput("TO", "Test");
        builder.addBlock("motion_goto");
        builder.addValueInput("TO", "Test");

        builder.build();
        builder.run();
    }
}

TEST_F(MotionBlocksTest, GlideSecsToXY)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setX(51.28);
        sprite->setY(-0.5);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_glidesecstoxy");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("X", -55.2);
        builder.addValueInput("Y", 23.254);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 29.98);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, 4.25);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.75));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, -23.26);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, 16.13);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -55.2);
        ASSERT_EQ(sprite->y(), 23.254);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glidesecstoxy");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("X", -55.2);
        builder.addValueInput("Y", 23.254);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.75));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(MotionBlocksTest, GlideToMouse)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-45.12));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-123.48));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.75));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, -10.55);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -116.44);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-45.12));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-123.48));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.75));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, -10.55);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -116.44);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.3));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.51));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(MotionBlocksTest, GlideToRandomPosition)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        StackTimerMock timer;
        ctx->setRng(&rng);
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(-45.12));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-123.48));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        RandomGeneratorMock rng;
        StackTimerMock timer;
        ctx->setRng(&rng);
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(640));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(500));
        EXPECT_CALL(rng, randintDouble(-320, 320)).WillOnce(Return(-45.12));
        EXPECT_CALL(rng, randintDouble(-250, 250)).WillOnce(Return(-123.48));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.3));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "_random_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.51));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(MotionBlocksTest, GlideToSprite)
{
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(-45.12);
        anotherSprite->setY(-123.48);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(anotherSprite.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "def");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(2));
        EXPECT_CALL(m_engineMock, targetAt(2)).WillRepeatedly(Return(nullptr));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        EXPECT_CALL(timer, start).Times(0);

        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "_stage_");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        EXPECT_CALL(timer, start).Times(0);

        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto anotherSprite = std::make_shared<Sprite>();
        anotherSprite->setName("def");
        anotherSprite->setEngine(&m_engineMock);
        anotherSprite->setX(-45.12);
        anotherSprite->setY(-123.48);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillRepeatedly(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillRepeatedly(Return(anotherSprite.get()));

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(0.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, 47.06);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, -104.72);

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.55));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->x(), -45.12);
        ASSERT_EQ(sprite->y(), -123.48);
    }

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "def");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        EXPECT_CALL(timer, start).Times(0);

        EXPECT_CALL(m_engineMock, findTarget("def")).WillOnce(Return(5));
        EXPECT_CALL(m_engineMock, targetAt(5)).WillOnce(Return(nullptr));
        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());

    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setName("abc");
        sprite->setEngine(&m_engineMock);
        sprite->setX(70.1);
        sprite->setY(-100.025);

        auto stage = std::make_shared<Stage>();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "_stage_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        EXPECT_CALL(timer, start).Times(0);

        EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(stage.get()));
        code->run(ctx.get());
        ASSERT_EQ(sprite->x(), 70.1);
        ASSERT_EQ(sprite->y(), -100.025);
    }

    EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());

    {
        auto stage = std::make_shared<Stage>();

        auto sprite = std::make_shared<Sprite>();
        sprite->setName("Test");
        sprite->setEngine(&m_engineMock);
        sprite->setX(153.2);
        sprite->setY(59.27);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addDropdownInput("TO", "Test");
        auto block = builder.currentBlock();

        EXPECT_CALL(m_engineMock, findTarget("Test")).WillOnce(Return(9));
        EXPECT_CALL(m_engineMock, targetAt(9)).WillOnce(Return(sprite.get()));
        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.3));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }

    {
        auto stage = std::make_shared<Stage>();

        auto sprite = std::make_shared<Sprite>();
        sprite->setName("Test");
        sprite->setEngine(&m_engineMock);
        sprite->setX(153.2);
        sprite->setY(59.27);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_glideto");
        builder.addValueInput("SECS", 2.5);
        builder.addValueInput("TO", "Test");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(stage.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

        EXPECT_CALL(m_engineMock, findTarget("Test")).WillOnce(Return(9));
        EXPECT_CALL(m_engineMock, targetAt(9)).WillOnce(Return(sprite.get()));
        EXPECT_CALL(timer, start(2.5));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(1.3));
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(timer, elapsedTime()).WillOnce(Return(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw()).WillRepeatedly(Return());
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(MotionBlocksTest, ChangeXBy)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_changexby");
        builder.addValueInput("DX", 30.25);

        sprite->setX(5.2);
        sprite->setY(-0.25);
        sprite->setDirection(-61.42);

        builder.build();
        builder.run();
        ASSERT_EQ(sprite->x(), 35.45);
        ASSERT_EQ(sprite->y(), -0.25);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_changexby");
        builder.addValueInput("DX", 30.25);

        builder.build();
        builder.run();
    }
}
