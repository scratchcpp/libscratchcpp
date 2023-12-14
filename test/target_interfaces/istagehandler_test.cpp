#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <stagehandlermock.h>
#include <enginemock.h>
#include <graphicseffectmock.h>

#include "../common.h"

using namespace libscratchcpp;

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
