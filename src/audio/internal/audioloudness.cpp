// SPDX-License-Identifier: Apache-2.0

#include <miniaudio.h>
#include <cmath>
#include <iostream>

#include "audioloudness.h"

using namespace libscratchcpp;

static int loudness = -1;
static float lastValue = 0.0f;

static void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    const float *pMicDataArray = static_cast<const float *>(pInput);

    // https://github.com/scratchfoundation/scratch-audio/blob/068aca613604e39b2adbe785b17931cc43eec35f/src/Loudness.js#L36-L80
    // Compute the RMS of the sound
    float sum = 0.0f;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        float value = pMicDataArray[i];
        sum += value * value;
    }

    float rms = std::sqrt(sum / static_cast<float>(frameCount));

    // Smooth the value, if it is descending
    if (lastValue != 0.0f) {
        rms = std::max(rms, lastValue * 0.6f);
    }

    lastValue = rms;

    // Scale the measurement
    rms *= 1.63f;
    rms = std::sqrt(rms);
    // Scale it up to 0-100 and round
    rms = std::round(rms * 100.0f);
    // Prevent it from going above 100
    rms = std::min(rms, 100.0f);

    loudness = rms;
}

AudioLoudness::AudioLoudness()
{
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 1; // mono
    deviceConfig.sampleRate = 44100;
    deviceConfig.periodSizeInFrames = 2048;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = NULL;

    m_device = new ma_device;

    if (ma_device_init(NULL, &deviceConfig, m_device) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio capture device." << std::endl;
        delete m_device;
        m_device = nullptr;
        return;
    }

    if (ma_device_start(m_device) != MA_SUCCESS) {
        std::cerr << "Failed to start audio capture device." << std::endl;
        ma_device_uninit(m_device);
        delete m_device;
        m_device = nullptr;
        return;
    }
}

AudioLoudness::~AudioLoudness()
{
    if (m_device) {
        ma_device_uninit(m_device);
        delete m_device;
    }
}

int AudioLoudness::getLoudness() const
{
    return loudness;
}
