#pragma once

#include <iscratchstage.h>

using namespace libscratchcpp;

class TestStageInterface : public IScratchStage
{
    public:
        TestStageInterface();

        void setTempo(int tempo) override;
        void setVideoState(Stage::VideoState videoState) override;
        void setVideoTransparency(int videoTransparency) override;

        void setCostume(const char *data) override;

        int tempo = 60;
        Stage::VideoState videoState = Stage::VideoState::Off;
        int videoTrasparency = 50;

        const char *costumeData = nullptr;
};
