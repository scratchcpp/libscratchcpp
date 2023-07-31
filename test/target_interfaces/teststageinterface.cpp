#include "teststageinterface.h"

TestStageInterface::TestStageInterface()
{
}

void TestStageInterface::setTempo(int tempo)
{
    this->tempo = tempo;
}

void TestStageInterface::setVideoState(Stage::VideoState videoState)
{
    this->videoState = videoState;
}

void TestStageInterface::setVideoTransparency(int videoTransparency)
{
    this->videoTrasparency = videoTransparency;
}

void TestStageInterface::setCostume(const char *data)
{
    costumeData = data;
}
