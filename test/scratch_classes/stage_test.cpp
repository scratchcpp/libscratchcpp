#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/value.h>
#include <enginemock.h>
#include <graphicseffectmock.h>
#include <stagehandlermock.h>
#include <targetmock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

TEST(StageTest, IsStage)
{
    Stage stage;
    ASSERT_TRUE(stage.isStage());
}

TEST(StageTest, Interface)
{
    Stage stage;
    ASSERT_EQ(stage.getInterface(), nullptr);

    StageHandlerMock handler;
    EXPECT_CALL(handler, init);
    stage.setInterface(&handler);
    ASSERT_EQ(stage.getInterface(), &handler);
}

TEST(StageTest, CostumeIndex)
{
    Stage stage;
    EngineMock engine;
    stage.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(2);

    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");
    stage.addCostume(c1);
    stage.addCostume(c2);

    stage.setCostumeIndex(0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    stage.setCostumeIndex(1);
    ASSERT_EQ(stage.costumeIndex(), 1);
}

TEST(StageTest, Tempo)
{
    Stage stage;
    ASSERT_EQ(stage.tempo(), 60);
    stage.setTempo(120);
    ASSERT_EQ(stage.tempo(), 120);
}

TEST(StageTest, VideoState)
{
    Stage stage;
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off); // default
    ASSERT_EQ(stage.videoStateStr(), "off");

    stage.setVideoState(Stage::VideoState::On);
    ASSERT_EQ(stage.videoState(), Stage::VideoState::On);
    ASSERT_EQ(stage.videoStateStr(), "on");

    stage.setVideoState(Stage::VideoState::OnFlipped);
    ASSERT_EQ(stage.videoState(), Stage::VideoState::OnFlipped);
    ASSERT_EQ(stage.videoStateStr(), "on-flipped");

    stage.setVideoState(Stage::VideoState::Off);
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off);
    ASSERT_EQ(stage.videoStateStr(), "off");

    stage.setVideoState("invalid");
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off); // shouldn't change
    ASSERT_EQ(stage.videoStateStr(), "off");

    stage.setVideoState("on");
    ASSERT_EQ(stage.videoState(), Stage::VideoState::On);
    ASSERT_EQ(stage.videoStateStr(), "on");

    stage.setVideoState("on-flipped");
    ASSERT_EQ(stage.videoState(), Stage::VideoState::OnFlipped);
    ASSERT_EQ(stage.videoStateStr(), "on-flipped");

    stage.setVideoState("off");
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off);
    ASSERT_EQ(stage.videoStateStr(), "off");

    stage.setVideoState(std::string("invalid"));
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off); // shouldn't change
    ASSERT_EQ(stage.videoStateStr(), "off");

    stage.setVideoState(std::string("on"));
    ASSERT_EQ(stage.videoState(), Stage::VideoState::On);
    ASSERT_EQ(stage.videoStateStr(), "on");

    stage.setVideoState(std::string("on-flipped"));
    ASSERT_EQ(stage.videoState(), Stage::VideoState::OnFlipped);
    ASSERT_EQ(stage.videoStateStr(), "on-flipped");

    stage.setVideoState(std::string("off"));
    ASSERT_EQ(stage.videoState(), Stage::VideoState::Off);
    ASSERT_EQ(stage.videoStateStr(), "off");
}

TEST(StageTest, VideoTransparency)
{
    Stage stage;
    ASSERT_EQ(stage.videoTransparency(), 50);
    stage.setVideoTransparency(100);
    ASSERT_EQ(stage.videoTransparency(), 100);
}

TEST(StageTest, TextToSpeechLanguage)
{
    Stage stage;
    ASSERT_EQ(stage.textToSpeechLanguage(), "");
    stage.setTextToSpeechLanguage("English");
    ASSERT_EQ(stage.textToSpeechLanguage(), "English");
}

TEST(StageTest, DefaultBoundingRect)
{
    Stage stage;
    Rect rect = stage.boundingRect();
    ASSERT_EQ(rect.left(), 0);
    ASSERT_EQ(rect.top(), 0);
    ASSERT_EQ(rect.right(), 0);
    ASSERT_EQ(rect.bottom(), 0);
}

TEST(StageTest, CurrentCostumeWidth)
{
    Stage stage;
    StageHandlerMock handler;
    EXPECT_CALL(handler, init);
    stage.setInterface(&handler);

    EXPECT_CALL(handler, costumeWidth()).WillOnce(Return(46));
    ASSERT_EQ(stage.currentCostumeWidth(), 46);
}

TEST(StageTest, CurrentCostumeHeight)
{
    Stage stage;
    StageHandlerMock handler;
    EXPECT_CALL(handler, init);
    stage.setInterface(&handler);

    EXPECT_CALL(handler, costumeHeight()).WillOnce(Return(24));
    ASSERT_EQ(stage.currentCostumeHeight(), 24);
}

TEST(StageTest, DefaultFastBoundingRect)
{
    Stage stage;
    Rect rect = stage.fastBoundingRect();
    ASSERT_EQ(rect.left(), 0);
    ASSERT_EQ(rect.top(), 0);
    ASSERT_EQ(rect.right(), 0);
    ASSERT_EQ(rect.bottom(), 0);
}

TEST(StageTest, TouchingSprite)
{
    Stage stage;
    Sprite another;
    EngineMock engine;
    another.setEngine(&engine);
    ASSERT_FALSE(stage.touchingSprite(nullptr));

    StageHandlerMock iface;
    EXPECT_CALL(iface, init);
    stage.setInterface(&iface);

    EXPECT_CALL(engine, cloneLimit()).WillRepeatedly(Return(-1));
    EXPECT_CALL(engine, initClone).Times(3);
    EXPECT_CALL(engine, requestRedraw).Times(3);
    EXPECT_CALL(engine, moveSpriteBehindOther).Times(3);
    auto clone1 = another.clone();
    auto clone2 = another.clone();
    auto clone3 = another.clone();
    std::vector<Sprite *> clones = { &another, clone1.get(), clone2.get(), clone3.get() };
    std::vector<Sprite *> actualClones;

    EXPECT_CALL(iface, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&clones, &actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(stage.touchingSprite(&another));
    ASSERT_EQ(clones, actualClones);

    EXPECT_CALL(iface, touchingClones).WillOnce(Return(true));
    ASSERT_TRUE(stage.touchingSprite(&another));
}

TEST(StageTest, TouchingPoint)
{
    Stage stage;
    ASSERT_FALSE(stage.touchingPoint(0, 0));

    StageHandlerMock iface;
    EXPECT_CALL(iface, init);
    stage.setInterface(&iface);

    EXPECT_CALL(iface, touchingPoint(51.4, -74.05)).WillOnce(Return(false));
    ASSERT_FALSE(stage.touchingPoint(51.4, -74.05));

    EXPECT_CALL(iface, touchingPoint(-12.46, 120.72)).WillOnce(Return(true));
    ASSERT_TRUE(stage.touchingPoint(-12.46, 120.72));
}

TEST(SpriteTest, TouchingColor)
{
    Stage stage;
    ASSERT_FALSE(stage.touchingColor(0));
    ASSERT_FALSE(stage.touchingColor(0, 0));

    StageHandlerMock iface;
    EXPECT_CALL(iface, init);
    stage.setInterface(&iface);

    Value v1 = 4278228630, v2 = "#FF00FA";
    EXPECT_CALL(iface, touchingColor(v1)).WillOnce(Return(false));
    ASSERT_FALSE(stage.touchingColor(v1));

    EXPECT_CALL(iface, touchingColor(v2)).WillOnce(Return(true));
    ASSERT_TRUE(stage.touchingColor(v2));

    EXPECT_CALL(iface, touchingColor(v1, v2)).WillOnce(Return(false));
    ASSERT_FALSE(stage.touchingColor(v1, v2));

    EXPECT_CALL(iface, touchingColor(v2, v1)).WillOnce(Return(true));
    ASSERT_TRUE(stage.touchingColor(v2, v1));
}

TEST(StageTest, GraphicsEffects)
{
    Stage stage;

    EngineMock engine;
    stage.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(4);

    GraphicsEffectMock effect1, effect2;
    EXPECT_CALL(effect1, clamp(48.21)).WillOnce(Return(48.21));
    stage.setGraphicsEffectValue(&effect1, 48.21);
    ASSERT_EQ(stage.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(stage.graphicsEffectValue(&effect2), 0);

    EXPECT_CALL(effect2, clamp(-107.08)).WillOnce(Return(-107.08));
    stage.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(stage.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(stage.graphicsEffectValue(&effect2), -107.08);

    EXPECT_CALL(effect1, clamp(300)).WillOnce(Return(101.5));
    stage.setGraphicsEffectValue(&effect1, 300);
    ASSERT_EQ(stage.graphicsEffectValue(&effect1), 101.5);
    ASSERT_EQ(stage.graphicsEffectValue(&effect2), -107.08);

    stage.clearGraphicsEffects();
    ASSERT_EQ(stage.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(stage.graphicsEffectValue(&effect2), 0);
}

TEST(StageTest, BubbleTypeRedraw)
{
    Stage stage;
    EngineMock engine;
    stage.setEngine(&engine);

    EXPECT_CALL(engine, requestRedraw).Times(0);
    stage.bubble()->setType(TextBubble::Type::Say);
    stage.bubble()->setType(TextBubble::Type::Think);
}

TEST(StageTest, BubbleTextRedraw)
{
    Stage stage;
    EngineMock engine;
    stage.setEngine(&engine);

    EXPECT_CALL(engine, requestRedraw());
    stage.bubble()->setText("hello");

    EXPECT_CALL(engine, requestRedraw).Times(0);
    stage.bubble()->setText("");
}

TEST(StageTest, LayerOrder)
{
    Stage stage;
    ASSERT_EQ(stage.layerOrder(), 0);
    stage.setLayerOrder(2);
    ASSERT_EQ(stage.layerOrder(), 2);
}
