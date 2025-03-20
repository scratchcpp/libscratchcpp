#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/list.h>
#include <scratchcpp/scratchconfiguration.h>
#include <enginemock.h>
#include <graphicseffectmock.h>
#include <stacktimermock.h>

#include "../common.h"
#include "blocks/looksblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::_;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<LooksBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            m_extension->onInit(m_engine);

            // Create and register fake graphic effects
            auto colorEffect = std::make_shared<GraphicsEffectMock>();
            auto fisheyeEffect = std::make_shared<GraphicsEffectMock>();
            auto whirlEffect = std::make_shared<GraphicsEffectMock>();
            auto pixelateEffect = std::make_shared<GraphicsEffectMock>();
            auto mosaicEffect = std::make_shared<GraphicsEffectMock>();
            auto brightnessEffect = std::make_shared<GraphicsEffectMock>();
            auto ghostEffect = std::make_shared<GraphicsEffectMock>();

            EXPECT_CALL(*colorEffect, name()).WillOnce(Return("COLOR"));
            ScratchConfiguration::registerGraphicsEffect(colorEffect);

            EXPECT_CALL(*fisheyeEffect, name()).WillOnce(Return("FISHEYE"));
            ScratchConfiguration::registerGraphicsEffect(fisheyeEffect);

            EXPECT_CALL(*whirlEffect, name()).WillOnce(Return("WHIRL"));
            ScratchConfiguration::registerGraphicsEffect(whirlEffect);

            EXPECT_CALL(*pixelateEffect, name()).WillOnce(Return("PIXELATE"));
            ScratchConfiguration::registerGraphicsEffect(pixelateEffect);

            EXPECT_CALL(*mosaicEffect, name()).WillOnce(Return("MOSAIC"));
            ScratchConfiguration::registerGraphicsEffect(mosaicEffect);

            EXPECT_CALL(*brightnessEffect, name()).WillOnce(Return("BRIGHTNESS"));
            ScratchConfiguration::registerGraphicsEffect(brightnessEffect);

            EXPECT_CALL(*ghostEffect, name()).WillOnce(Return("GHOST"));
            ScratchConfiguration::registerGraphicsEffect(ghostEffect);

            EXPECT_CALL(*colorEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*fisheyeEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*whirlEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*pixelateEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*mosaicEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*brightnessEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*ghostEffect, clamp(_)).WillRepeatedly(ReturnArg<0>());
        }

        void TearDown() override
        {
            // Remove fake graphic effects
            ScratchConfiguration::removeGraphicsEffect("COLOR");
            ScratchConfiguration::removeGraphicsEffect("FISHEYE");
            ScratchConfiguration::removeGraphicsEffect("WHIRL");
            ScratchConfiguration::removeGraphicsEffect("PIXELATE");
            ScratchConfiguration::removeGraphicsEffect("MOSAIC");
            ScratchConfiguration::removeGraphicsEffect("BRIGHTNESS");
            ScratchConfiguration::removeGraphicsEffect("GHOST");
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(LooksBlocksTest, StopProject)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    GraphicsEffectMock effect;
    m_engine->setTargets({ stage, sprite });

    stage->bubble()->setText("abc");
    sprite->bubble()->setText("def");
    EXPECT_CALL(effect, clamp(10)).WillOnce(Return(10));
    sprite->setGraphicsEffectValue(&effect, 10);
    EXPECT_CALL(effect, clamp(2.5)).WillOnce(Return(2.5));
    stage->setGraphicsEffectValue(&effect, 2.5);

    m_engine->stop();
    ASSERT_TRUE(stage->bubble()->text().empty());
    ASSERT_TRUE(sprite->bubble()->text().empty());
    ASSERT_EQ(stage->graphicsEffectValue(&effect), 0);
    ASSERT_EQ(sprite->graphicsEffectValue(&effect), 0);
}

TEST_F(LooksBlocksTest, SayAndThinkForSecs)
{
    std::vector<TextBubble::Type> types = { TextBubble::Type::Say, TextBubble::Type::Think };
    std::vector<std::string> opcodes = { "looks_sayforsecs", "looks_thinkforsecs" };

    for (int i = 0; i < types.size(); i++) {
        TextBubble::Type type = types[i];
        const std::string &opcode = opcodes[i];

        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        m_engine->clear();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock(opcode);
        builder.addValueInput("MESSAGE", "Hello world");
        builder.addValueInput("SECS", 2.5);
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
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(false));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_TRUE(sprite->bubble()->text().empty());

        // Change text while waiting
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("test");

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "test");
        ASSERT_EQ(sprite->bubble()->type(), type);

        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("Hello world");

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        // Kill waiting thread
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        m_engine->threadAboutToStop()(&thread);
        code->kill(ctx.get());
        ASSERT_EQ(sprite->bubble()->owner(), nullptr);
        ASSERT_TRUE(sprite->bubble()->text().empty());

        // Kill waiting thread after changing text
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("test");

        m_engine->threadAboutToStop()(&thread);
        code->kill(ctx.get());
        ASSERT_EQ(sprite->bubble()->owner(), nullptr);
        ASSERT_EQ(sprite->bubble()->text(), "test");
        ASSERT_EQ(sprite->bubble()->type(), type);
    }
}

TEST_F(LooksBlocksTest, SayAndThink)
{
    std::vector<TextBubble::Type> types = { TextBubble::Type::Say, TextBubble::Type::Think };
    std::vector<std::string> opcodes = { "looks_say", "looks_think" };

    for (int i = 0; i < types.size(); i++) {
        TextBubble::Type type = types[i];
        const std::string &opcode = opcodes[i];

        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        m_engine->clear();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock(opcode);
        builder.addValueInput("MESSAGE", "Hello world");

        builder.build();
        builder.run();
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);
        ASSERT_EQ(sprite->bubble()->owner(), nullptr);
    }
}

TEST_F(LooksBlocksTest, Show)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("looks_show");
        builder.build();

        sprite->setVisible(false);

        builder.run();
        ASSERT_TRUE(sprite->visible());

        builder.run();
        ASSERT_TRUE(sprite->visible());
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("looks_show");

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, Hide)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("looks_hide");
        builder.build();

        sprite->setVisible(true);

        builder.run();
        ASSERT_FALSE(sprite->visible());

        builder.run();
        ASSERT_FALSE(sprite->visible());
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("looks_hide");

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, ChangeEffectBy)
{
    // Color
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("COLOR");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_changeeffectby");
        builder.addDropdownField("EFFECT", "COLOR");
        builder.addValueInput("CHANGE", 45.2);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, m_engine);
        script.setCode(code);
        Thread thread(stage.get(), m_engine, &script);

        stage->setGraphicsEffectValue(effect, 86.84);
        thread.run();
        ASSERT_EQ(std::round(stage->graphicsEffectValue(effect) * 100) / 100, 132.04);
    }

    // Brightness
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("BRIGHTNESS");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_changeeffectby");
        builder.addDropdownField("EFFECT", "BRIGHTNESS");
        builder.addValueInput("CHANGE", 12.05);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, m_engine);
        script.setCode(code);
        Thread thread(stage.get(), m_engine, &script);

        thread.run();
        ASSERT_EQ(std::round(stage->graphicsEffectValue(effect) * 100) / 100, 12.05);
    }

    // Ghost
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("GHOST");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_changeeffectby");
        builder.addDropdownField("EFFECT", "GHOST");
        builder.addValueInput("CHANGE", -8.06);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, m_engine);
        script.setCode(code);
        Thread thread(stage.get(), m_engine, &script);

        stage->setGraphicsEffectValue(effect, 13.12);
        thread.run();
        ASSERT_EQ(std::round(stage->graphicsEffectValue(effect) * 100) / 100, 5.06);
    }

    // Invalid
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        builder.addBlock("looks_changeeffectby");
        builder.addDropdownField("EFFECT", "INVALID");
        builder.addValueInput("CHANGE", 8.3);

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, SetEffectTo)
{
    // Fisheye
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("FISHEYE");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_seteffectto");
        builder.addDropdownField("EFFECT", "FISHEYE");
        builder.addValueInput("VALUE", 45.2);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, m_engine);
        script.setCode(code);
        Thread thread(stage.get(), m_engine, &script);

        stage->setGraphicsEffectValue(effect, 86.84);
        thread.run();
        ASSERT_EQ(std::round(stage->graphicsEffectValue(effect) * 100) / 100, 45.2);
    }

    // Pixelate
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("PIXELATE");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_seteffectto");
        builder.addDropdownField("EFFECT", "PIXELATE");
        builder.addValueInput("VALUE", 12.05);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, stage.get());
        auto code = compiler.compile(block);
        Script script(stage.get(), block, m_engine);
        script.setCode(code);
        Thread thread(stage.get(), m_engine, &script);

        thread.run();
        ASSERT_EQ(std::round(stage->graphicsEffectValue(effect) * 100) / 100, 12.05);
    }

    // Mosaic
    {
        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect("MOSAIC");
        ASSERT_TRUE(effect);

        builder.addBlock("looks_seteffectto");
        builder.addDropdownField("EFFECT", "MOSAIC");
        builder.addValueInput("VALUE", -8.06);
        auto block = builder.currentBlock();

        Compiler compiler(m_engine, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, m_engine);
        script.setCode(code);
        Thread thread(sprite.get(), m_engine, &script);

        sprite->setGraphicsEffectValue(effect, 13.12);
        thread.run();
        ASSERT_EQ(std::round(sprite->graphicsEffectValue(effect) * 100) / 100, -8.06);
    }

    // Invalid
    {
        auto stage = std::make_shared<Stage>();
        stage->setEngine(m_engine);

        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        builder.addBlock("looks_seteffectto");
        builder.addDropdownField("EFFECT", "INVALID");
        builder.addValueInput("VALUE", 8.3);

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, ClearGraphicEffects)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(m_engine);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    IGraphicsEffect *effect1 = ScratchConfiguration::getGraphicsEffect("WHIRL");
    IGraphicsEffect *effect2 = ScratchConfiguration::getGraphicsEffect("GHOST");
    IGraphicsEffect *effect3 = ScratchConfiguration::getGraphicsEffect("MOSAIC");
    ASSERT_TRUE(effect1);
    ASSERT_TRUE(effect2);
    ASSERT_TRUE(effect3);

    builder.addBlock("looks_cleargraphiceffects");
    auto block = builder.currentBlock();

    Compiler compiler(m_engine, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, m_engine);
    script.setCode(code);
    Thread thread(sprite.get(), m_engine, &script);

    sprite->setGraphicsEffectValue(effect1, 86.84);
    sprite->setGraphicsEffectValue(effect2, -5.18);
    sprite->setGraphicsEffectValue(effect3, 12.98);
    thread.run();
    ASSERT_EQ(sprite->graphicsEffectValue(effect1), 0);
    ASSERT_EQ(sprite->graphicsEffectValue(effect2), 0);
    ASSERT_EQ(sprite->graphicsEffectValue(effect3), 0);
}

TEST_F(LooksBlocksTest, ChangeSizeBy)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("looks_changesizeby");
        builder.addValueInput("CHANGE", 2.5);
        builder.build();

        sprite->setEngine(nullptr);
        sprite->setSize(45.62);
        builder.run();
        ASSERT_EQ(sprite->size(), 48.12);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        builder.addBlock("looks_changesizeby");
        builder.addValueInput("CHANGE", 2.5);

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, SetSizeTo)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("looks_setsizeto");
        builder.addValueInput("SIZE", 2.5);
        builder.build();

        sprite->setEngine(nullptr);
        sprite->setSize(45.62);
        builder.run();
        ASSERT_EQ(sprite->size(), 2.5);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        builder.addBlock("looks_setsizeto");
        builder.addValueInput("SIZE", 2.5);

        builder.build();
        builder.run();
    }
}

TEST_F(LooksBlocksTest, Size)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);
        builder.addBlock("looks_size");
        builder.captureBlockReturnValue();
        builder.build();

        sprite->setEngine(nullptr);
        sprite->setSize(45.62);
        builder.run();

        List *list = builder.capturedValues();
        ASSERT_EQ(list->size(), 1);
        ASSERT_EQ(Value(list->data()[0]).toDouble(), 45.62);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);
        builder.addBlock("looks_size");
        builder.captureBlockReturnValue();

        builder.build();
        builder.run();

        List *list = builder.capturedValues();
        ASSERT_EQ(list->size(), 1);
        ASSERT_EQ(Value(list->data()[0]).toDouble(), 100);
    }
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_CostumeName)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "costume2");
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_InvalidCostumeName)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "invalid");
    builder.build();

    sprite->setCostumeIndex(1);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    sprite->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_NextCostume)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "next costume");
    builder.build();

    sprite->setCostumeIndex(0);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_NextCostumeExists)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto nextCostume = std::make_shared<Costume>("next costume", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(nextCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "next costume");
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_PreviousCostume)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "previous costume");
    builder.build();

    sprite->setCostumeIndex(0);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_PreviousCostumeExists)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto previousCostume = std::make_shared<Costume>("previous costume", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(previousCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "previous costume");
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_NumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto numberCostume = std::make_shared<Costume>("3", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(numberCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", 3);
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_PositiveOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto numberCostume = std::make_shared<Costume>("3", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(numberCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", 5);
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_NegativeOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto numberCostume = std::make_shared<Costume>("3", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(numberCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", -1);
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_StringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", "3");
    builder.build();

    sprite->setCostumeIndex(1);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_OutOfRangeStringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto numberCostume = std::make_shared<Costume>("3", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(numberCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", "+7.0");
    builder.build();

    sprite->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_WhitespaceString)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto numberCostume = std::make_shared<Costume>("3", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(numberCostume);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", "");
    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", "   ");
    builder.build();

    sprite->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(costume1);
    stage->addCostume(costume2);
    stage->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "costume1");
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, NextCostume_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_nextcostume");
    builder.build();

    sprite->setCostumeIndex(0);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 2);

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, NextCostume_Stage)
{
    auto stage = std::make_shared<Stage>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(costume1);
    stage->addCostume(costume2);
    stage->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_nextcostume");
    builder.build();

    stage->setCostumeIndex(0);
    builder.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    builder.run();
    ASSERT_EQ(stage->costumeIndex(), 2);

    builder.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}
