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
#include <randomgeneratormock.h>

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

TEST_F(LooksBlocksTest, Show_Sprite)
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

TEST_F(LooksBlocksTest, Show_Stage)
{
    auto stage = std::make_shared<Stage>();
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_show");

    builder.build();
    builder.run();
}

TEST_F(LooksBlocksTest, Hide_Sprite)
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

TEST_F(LooksBlocksTest, Hide_Stage)
{
    auto stage = std::make_shared<Stage>();
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_hide");

    builder.build();
    builder.run();
}

TEST_F(LooksBlocksTest, ChangeEffectBy_Color)
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

TEST_F(LooksBlocksTest, ChangeEffectBy_Brightness)
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

TEST_F(LooksBlocksTest, ChangeEffectBy_Ghost)
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

TEST_F(LooksBlocksTest, ChangeEffectBy_Invalid)
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

TEST_F(LooksBlocksTest, SetEffectTo_Fisheye)
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

TEST_F(LooksBlocksTest, SetEffectTo_Pixelate)
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

TEST_F(LooksBlocksTest, SetEffectTo_Mosaic)
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

TEST_F(LooksBlocksTest, SetEffectTo_Invalid)
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

TEST_F(LooksBlocksTest, ChangeSizeBy_Sprite)
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

TEST_F(LooksBlocksTest, ChangeSizeBy_Stage)
{
    auto stage = std::make_shared<Stage>();
    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("looks_changesizeby");
    builder.addValueInput("CHANGE", 2.5);

    builder.build();
    builder.run();
}

TEST_F(LooksBlocksTest, SetSizeTo_Sprite)
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

TEST_F(LooksBlocksTest, SetSizeTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("looks_setsizeto");
    builder.addValueInput("SIZE", 2.5);

    builder.build();
    builder.run();
}

TEST_F(LooksBlocksTest, Size_Sprite)
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

TEST_F(LooksBlocksTest, Size_Stage)
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

TEST_F(LooksBlocksTest, SwitchCostumeTo_NoCostumes)
{
    auto sprite = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchcostumeto");
    builder.addDropdownInput("COSTUME", "costume2");
    builder.build();
    builder.run();
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

TEST_F(LooksBlocksTest, SwitchCostumeTo_InvalidNumberIndex)
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
    builder.addValueInput("COSTUME", std::numeric_limits<double>::quiet_NaN());
    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", std::numeric_limits<double>::infinity());
    builder.addBlock("looks_switchcostumeto");
    builder.addValueInput("COSTUME", -std::numeric_limits<double>::infinity());
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);

    sprite->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 0);
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

TEST_F(LooksBlocksTest, SwitchCostumeTo_StringIndexExists)
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
    builder.addValueInput("COSTUME", "3");
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->costumeIndex(), 1);
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

TEST_F(LooksBlocksTest, SwitchBackdropTo_NoBackdrops)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "backdrop2");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts).Times(0);
    thread.run();
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_BackdropName)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "backdrop2");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop2->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_InvalidBackdropName)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "invalid");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop2->broadcast(), &thread, false));
    stage->setCostumeIndex(1);
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(testBackdrop->broadcast(), &thread, false));
    stage->setCostumeIndex(2);
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_NextBackdrop)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "next backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop2->broadcast(), &thread, false));
    stage->setCostumeIndex(0);
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(testBackdrop->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 2);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_NextBackdropExists)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto nextBackdrop = std::make_shared<Costume>("next backdrop", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(nextBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "next backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(nextBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(nextBackdrop->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_PreviousBackdrop)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "previous backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(testBackdrop->broadcast(), &thread, false));
    stage->setCostumeIndex(0);
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 2);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop2->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_PreviousBackdropExists)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto previousBackdrop = std::make_shared<Costume>("previous backdrop", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(previousBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "previous backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(previousBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(previousBackdrop->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_RandomBackdrop)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "random backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    RandomGeneratorMock rng;
    ctx->setRng(&rng);

    EXPECT_CALL(rng, randintExcept(0, 2, 1)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    stage->setCostumeIndex(1);
    code->run(ctx.get());
    ASSERT_EQ(stage->costumeIndex(), 0);

    EXPECT_CALL(rng, randintExcept(0, 2, 0)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, startBackdropScripts(testBackdrop->broadcast(), &thread, false));
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(stage->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_RandomBackdropExists)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto randomBackdrop = std::make_shared<Costume>("random backdrop", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(randomBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "random backdrop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(randomBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(randomBackdrop->broadcast(), &thread, false));
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_NumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", 3);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(testBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_PositiveOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", 5);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(numberBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_NegativeOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", -1);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(numberBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_InvalidNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", std::numeric_limits<double>::quiet_NaN());
    auto block = builder.currentBlock();
    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", std::numeric_limits<double>::infinity());
    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", -std::numeric_limits<double>::infinity());

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    stage->setCostumeIndex(2);
    thread.reset();
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_StringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", "3");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_StringIndexExists)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", "3");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(numberBackdrop->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_OutOfRangeStringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", "+7.0");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_WhitespaceString)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(m_engine);

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto numberBackdrop = std::make_shared<Costume>("3", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(numberBackdrop);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    m_engine->setTargets({ stage });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", "");
    builder.addBlock("looks_switchbackdropto");
    builder.addValueInput("BACKDROP", "   ");
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();
    ASSERT_EQ(stage->costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(stage.get()));
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_switchbackdropto");
    builder.addDropdownInput("BACKDROP", "backdrop1");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, stage.get());
    auto code = compiler.compile(block);
    Script script(stage.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(stage.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, startBackdropScripts(backdrop1->broadcast(), &thread, false));
    stage->setCostumeIndex(2);
    thread.run();
    ASSERT_EQ(stage->costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, GoToFrontBack_MoveSpriteToFront)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_gotofrontback");
    builder.addDropdownField("FRONT_BACK", "front");
    builder.build();

    m_engine->moveDrawableToBack(sprite.get());

    builder.run();
    std::initializer_list<int> layers = { sprite->layerOrder(), sprite1->layerOrder(), sprite2->layerOrder(), sprite3->layerOrder() };
    ASSERT_EQ(sprite->layerOrder(), std::max(layers));

    builder.run();
    ASSERT_EQ(sprite->layerOrder(), std::max(layers));
}

TEST_F(LooksBlocksTest, GoToFrontBack_MoveSpriteToBack)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_gotofrontback");
    builder.addDropdownField("FRONT_BACK", "back");
    builder.build();

    m_engine->moveDrawableToFront(sprite.get());

    builder.run();
    ASSERT_EQ(sprite->layerOrder(), 1);

    builder.run();
    ASSERT_EQ(sprite->layerOrder(), 1);
}

TEST_F(LooksBlocksTest, GoToFrontBack_MoveStageToFront)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_gotofrontback");
    builder.addDropdownField("FRONT_BACK", "front");
    builder.build();

    builder.run();
    ASSERT_EQ(stage->layerOrder(), 0);
}

TEST_F(LooksBlocksTest, GoToFrontBack_MoveStageToBack)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_gotofrontback");
    builder.addDropdownField("FRONT_BACK", "back");
    builder.build();

    builder.run();
    ASSERT_EQ(sprite->layerOrder(), 0);
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveSpriteForwardPositive)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "forward");
    builder.addValueInput("NUM", 2);
    builder.build();

    // sprite, sprite1, sprite3, sprite2
    m_engine->moveDrawableToFront(sprite1.get());
    m_engine->moveDrawableToFront(sprite3.get());
    m_engine->moveDrawableToFront(sprite2.get());
    m_engine->moveDrawableToBack(sprite.get());

    // sprite1, sprite3, sprite, sprite2
    builder.run();
    ASSERT_GT(sprite->layerOrder(), sprite1->layerOrder());
    ASSERT_GT(sprite->layerOrder(), sprite3->layerOrder());
    ASSERT_LT(sprite->layerOrder(), sprite2->layerOrder());
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveSpriteForwardNegative)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "forward");
    builder.addValueInput("NUM", -1);
    builder.build();

    // sprite1, sprite3, sprite2, sprite
    m_engine->moveDrawableToFront(sprite1.get());
    m_engine->moveDrawableToFront(sprite3.get());
    m_engine->moveDrawableToFront(sprite2.get());
    m_engine->moveDrawableToFront(sprite.get());

    // sprite1, sprite3, sprite, sprite2
    builder.run();
    ASSERT_GT(sprite->layerOrder(), sprite1->layerOrder());
    ASSERT_GT(sprite->layerOrder(), sprite3->layerOrder());
    ASSERT_LT(sprite->layerOrder(), sprite2->layerOrder());
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveSpriteBackwardPositive)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "backward");
    builder.addValueInput("NUM", 1);
    builder.build();

    // sprite1, sprite3, sprite2, sprite
    m_engine->moveDrawableToFront(sprite1.get());
    m_engine->moveDrawableToFront(sprite3.get());
    m_engine->moveDrawableToFront(sprite2.get());
    m_engine->moveDrawableToFront(sprite.get());

    // sprite1, sprite3, sprite, sprite2
    builder.run();
    ASSERT_GT(sprite->layerOrder(), sprite1->layerOrder());
    ASSERT_GT(sprite->layerOrder(), sprite3->layerOrder());
    ASSERT_LT(sprite->layerOrder(), sprite2->layerOrder());
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveSpriteBackwardNegative)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "backward");
    builder.addValueInput("NUM", -2);
    builder.build();

    // sprite, sprite1, sprite3, sprite2
    m_engine->moveDrawableToFront(sprite1.get());
    m_engine->moveDrawableToFront(sprite3.get());
    m_engine->moveDrawableToFront(sprite2.get());
    m_engine->moveDrawableToBack(sprite.get());

    // sprite1, sprite3, sprite, sprite2
    builder.run();
    ASSERT_GT(sprite->layerOrder(), sprite1->layerOrder());
    ASSERT_GT(sprite->layerOrder(), sprite3->layerOrder());
    ASSERT_LT(sprite->layerOrder(), sprite2->layerOrder());
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveStageForward)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "forward");
    builder.addValueInput("NUM", 3);
    builder.build();

    builder.run();
    ASSERT_EQ(stage->layerOrder(), 0);
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers_MoveStageBackward)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    auto sprite2 = std::make_shared<Sprite>();
    auto sprite3 = std::make_shared<Sprite>();
    m_engine->setTargets({ stage, sprite, sprite1, sprite2, sprite3 });
    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_goforwardbackwardlayers");
    builder.addDropdownField("FORWARD_BACKWARD", "backward");
    builder.addValueInput("NUM", 3);
    builder.build();

    builder.run();
    ASSERT_EQ(stage->layerOrder(), 0);
}

TEST_F(LooksBlocksTest, BackdropNumberName_SpriteBackdropNumber)
{
    auto sprite = std::make_shared<Sprite>();

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    m_engine->setTargets({ stage, sprite });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_backdropnumbername");
    builder.addDropdownField("NUMBER_NAME", "number");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 3);
    ASSERT_EQ(Value(list->data()[1]).toDouble(), 1);
}

TEST_F(LooksBlocksTest, BackdropNumberName_SpriteBackdropName)
{
    auto sprite = std::make_shared<Sprite>();

    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    m_engine->setTargets({ stage, sprite });
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_backdropnumbername");
    builder.addDropdownField("NUMBER_NAME", "name");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toString(), "test");
    ASSERT_EQ(Value(list->data()[1]).toString(), "backdrop1");
}

TEST_F(LooksBlocksTest, BackdropNumberName_StageBackdropNumber)
{
    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_backdropnumbername");
    builder.addDropdownField("NUMBER_NAME", "number");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 3);
    ASSERT_EQ(Value(list->data()[1]).toDouble(), 1);
}

TEST_F(LooksBlocksTest, BackdropNumberName_StageBackdropName)
{
    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_backdropnumbername");
    builder.addDropdownField("NUMBER_NAME", "name");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toString(), "test");
    ASSERT_EQ(Value(list->data()[1]).toString(), "backdrop1");
}

TEST_F(LooksBlocksTest, CostumeNumberName_SpriteCostumeNumber)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_costumenumbername");
    builder.addDropdownField("NUMBER_NAME", "number");
    builder.captureBlockReturnValue();
    builder.build();

    sprite->setCostumeIndex(2);
    builder.run();

    sprite->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 3);
    ASSERT_EQ(Value(list->data()[1]).toDouble(), 1);
}

TEST_F(LooksBlocksTest, CostumeNumberName_SpriteCostumeName)
{
    auto sprite = std::make_shared<Sprite>();
    auto costume1 = std::make_shared<Costume>("costume1", "a", "png");
    auto costume2 = std::make_shared<Costume>("costume2", "b", "png");
    auto testCostume = std::make_shared<Costume>("test", "c", "svg");
    sprite->addCostume(costume1);
    sprite->addCostume(costume2);
    sprite->addCostume(testCostume);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_costumenumbername");
    builder.addDropdownField("NUMBER_NAME", "name");
    builder.captureBlockReturnValue();
    builder.build();

    sprite->setCostumeIndex(2);
    builder.run();

    sprite->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toString(), "test");
    ASSERT_EQ(Value(list->data()[1]).toString(), "costume1");
}

TEST_F(LooksBlocksTest, CostumeNumberName_StageCostumeNumber)
{
    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_costumenumbername");
    builder.addDropdownField("NUMBER_NAME", "number");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 3);
    ASSERT_EQ(Value(list->data()[1]).toDouble(), 1);
}

TEST_F(LooksBlocksTest, CostumeNumberName_StageCostumeName)
{
    auto stage = std::make_shared<Stage>();
    auto backdrop1 = std::make_shared<Costume>("backdrop1", "a", "png");
    auto backdrop2 = std::make_shared<Costume>("backdrop2", "b", "png");
    auto testBackdrop = std::make_shared<Costume>("test", "c", "svg");
    stage->addCostume(backdrop1);
    stage->addCostume(backdrop2);
    stage->addCostume(testBackdrop);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);

    builder.addBlock("looks_costumenumbername");
    builder.addDropdownField("NUMBER_NAME", "name");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setCostumeIndex(2);
    builder.run();

    stage->setCostumeIndex(0);
    builder.run();

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(list->data()[0]).toString(), "test");
    ASSERT_EQ(Value(list->data()[1]).toString(), "backdrop1");
}
