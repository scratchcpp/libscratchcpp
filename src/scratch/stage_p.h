// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/stage.h>

namespace libscratchcpp
{

struct StagePrivate
{
        StagePrivate();
        StagePrivate(const StagePrivate &) = delete;

        IStageHandler *iface = nullptr;
        int tempo = 60;
        Stage::VideoState videoState = Stage::VideoState::Off;
        int videoTransparency = 50;
        std::string textToSpeechLanguage;
};

} // namespace libscratchcpp
