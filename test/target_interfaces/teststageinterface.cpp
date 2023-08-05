#include "teststageinterface.h"

TestStageInterface::TestStageInterface()
{
}

void TestStageInterface::onStageChanged(Stage *stage)
{
    this->stage = stage;
}

void TestStageInterface::onTempoChanged(int tempo)
{
    this->tempo = tempo;
}

void TestStageInterface::onVideoStateChanged(Stage::VideoState videoState)
{
    this->videoState = videoState;
}

void TestStageInterface::onVideoTransparencyChanged(int videoTransparency)
{
    this->videoTrasparency = videoTransparency;
}

void TestStageInterface::onCostumeChanged(const char *data)
{
    costumeData = data;
}
