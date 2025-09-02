// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioengine.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT AudioEngineStub : public IAudioEngine
{
    public:
        friend class IAudioEngine;
        AudioEngineStub();

        float volume() const override;
        void setVolume(float volume) override;

    private:
        float m_volume = 1.0f;
};

} // namespace libscratchcpp
