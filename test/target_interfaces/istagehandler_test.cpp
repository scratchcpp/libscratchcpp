#include <scratchcpp/stage.h>

#include "teststageinterface.h"
#include "../common.h"

#define INIT_STAGE() stage.setInterface(&stageInterface);

using namespace libscratchcpp;

Stage stage;
TestStageInterface stageInterface;

TEST(IStageHandlerTest, Sprite)
{
    INIT_STAGE();
    ASSERT_EQ(stageInterface.stage, &stage);
}

TEST(IStageHandlerTest, Tempo)
{
    INIT_STAGE();
    stage.setTempo(120);
    ASSERT_EQ(stageInterface.tempo, 120);
    stage.setTempo(55);
    ASSERT_EQ(stage.tempo(), 55);
}

TEST(IStageHandlerTest, VideoState)
{
    INIT_STAGE();
    stage.setVideoState(Stage::VideoState::On);
    ASSERT_EQ(stageInterface.videoState, Stage::VideoState::On);
    stage.setVideoState(Stage::VideoState::OnFlipped);
    ASSERT_EQ(stageInterface.videoState, Stage::VideoState::OnFlipped);
}

TEST(IStageHandlerTest, CostumeData)
{
    INIT_STAGE();
    // TODO: Add tests for costume data
}
