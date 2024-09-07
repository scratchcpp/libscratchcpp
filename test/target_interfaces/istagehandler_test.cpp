#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <stagehandlermock.h>
#include <enginemock.h>
#include <graphicseffectmock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

class IStageHandlerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            EXPECT_CALL(m_handler, init(&m_stage)).Times(1);
            m_stage.setInterface(&m_handler);
            m_stage.setEngine(&m_engine);
        }

        Stage m_stage;
        StageHandlerMock m_handler;
        EngineMock m_engine;
};

TEST_F(IStageHandlerTest, Tempo)
{
    EXPECT_CALL(m_handler, onTempoChanged(120)).Times(1);
    m_stage.setTempo(120);
}

TEST_F(IStageHandlerTest, VideoState)
{
    EXPECT_CALL(m_handler, onVideoStateChanged(Stage::VideoState::On)).Times(1);
    m_stage.setVideoState(Stage::VideoState::On);

    EXPECT_CALL(m_handler, onVideoStateChanged(Stage::VideoState::OnFlipped)).Times(1);
    m_stage.setVideoState(Stage::VideoState::OnFlipped);
}

TEST_F(IStageHandlerTest, Costume)
{
    auto costume1 = std::make_shared<Costume>("", "", "");
    auto costume2 = std::make_shared<Costume>("", "", "");
    m_stage.addCostume(costume1);
    m_stage.addCostume(costume2);

    EXPECT_CALL(m_handler, onCostumeChanged(costume1.get()));
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.setCostumeIndex(0);

    EXPECT_CALL(m_handler, onCostumeChanged(costume2.get()));
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.setCostumeIndex(1);
}

TEST_F(IStageHandlerTest, GraphicsEffects)
{
    GraphicsEffectMock effect;

    EXPECT_CALL(m_handler, onGraphicsEffectChanged(&effect, 16.7));
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.setGraphicsEffectValue(&effect, 16.7);

    EXPECT_CALL(m_handler, onGraphicsEffectsCleared());
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.clearGraphicsEffects();
}

TEST_F(IStageHandlerTest, BubbleType)
{
    EXPECT_CALL(m_handler, onBubbleTypeChanged(Target::BubbleType::Say));
    m_stage.setBubbleType(Target::BubbleType::Say);

    EXPECT_CALL(m_handler, onBubbleTypeChanged(Target::BubbleType::Think));
    m_stage.setBubbleType(Target::BubbleType::Think);
}

TEST_F(IStageHandlerTest, BubbleText)
{
    EXPECT_CALL(m_handler, onBubbleTextChanged("test"));
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.setBubbleText("test");

    // The text should be processed in Target::setBubbleText() (#571)
    std::string longstr =
        "EY8OUNzAqwgh7NRGk5TzCP3dkAhJy9TX"
        "Y9mqKElPjdQpKddYqjyCwUk2hx6YgVZV"
        "6BOdmZGxDMs8Hjv8W9G6j4gTxAWdOkzs"
        "8Ih80xzEDbvLilWsDwoB6FxH2kVVI4xs"
        "IXOETNQ6QMsCKLWc5XjHk2BS9nYvDGpJ"
        "uEmp9zIzFGT1kRSrOlU3ZwnN1YtvqFx"
        "3hkWVNtJ71dQ0PJHhOVQPUy19V01SPu3"
        "KIIS2wdSUVAc4RYMzepSveghzWbdcizy"
        "Tm1KKAj4svu9YoL8b9vsolG8gKunvKO7"
        "MurRKSeUbECELnJEKV6683xCq7RvmjAu"
        "2djZ54apiQc1lTixWns5GoG0SVNuFzHl"
        "q97qUiqiMecjVFM51YVif7c1Stip52Hl";

    EXPECT_CALL(m_handler, onBubbleTextChanged(longstr.substr(0, 330)));
    EXPECT_CALL(m_engine, requestRedraw());
    m_stage.setBubbleText(longstr);
}

TEST_F(IStageHandlerTest, BoundingRect)
{
    EXPECT_CALL(m_handler, boundingRect()).WillOnce(Return(Rect(-44.6, 89.1, 20.5, -0.48)));
    Rect rect = m_stage.boundingRect();
    ASSERT_EQ(rect.left(), -44.6);
    ASSERT_EQ(rect.top(), 89.1);
    ASSERT_EQ(rect.right(), 20.5);
    ASSERT_EQ(rect.bottom(), -0.48);
}

TEST_F(IStageHandlerTest, FastBoundingRect)
{
    EXPECT_CALL(m_handler, fastBoundingRect()).WillOnce(Return(Rect(-44.6, 89.1, 20.5, -0.48)));
    Rect rect = m_stage.fastBoundingRect();
    ASSERT_EQ(rect.left(), -44.6);
    ASSERT_EQ(rect.top(), 89.1);
    ASSERT_EQ(rect.right(), 20.5);
    ASSERT_EQ(rect.bottom(), -0.48);
}
