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
