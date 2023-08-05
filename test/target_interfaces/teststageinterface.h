#pragma once

#include <scratchcpp/istagehandler.h>

using namespace libscratchcpp;

class TestStageInterface : public IStageHandler
{
    public:
        TestStageInterface();

        void onStageChanged(Stage *stage) override;

        void onCostumeChanged(const char *data) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

        Stage *stage = nullptr;

        const char *costumeData = nullptr;

        int tempo = 60;
        Stage::VideoState videoState = Stage::VideoState::Off;
        int videoTrasparency = 50;
};
