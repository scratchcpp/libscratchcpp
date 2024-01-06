// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "audioplayer.h"
#include "audioengine.h"

using namespace libscratchcpp;

AudioPlayer::AudioPlayer()
{
    m_decoder = new ma_decoder;
    m_sound = new ma_sound;
}

AudioPlayer::~AudioPlayer()
{
    if (m_loaded) {
        ma_sound_uninit(m_sound);
        ma_decoder_uninit(m_decoder);
    }

    delete m_decoder;
    delete m_sound;
}

bool AudioPlayer::load(unsigned int size, const void *data, unsigned long sampleRate)
{
    if (!AudioEngine::initialized())
        return false;

    ma_engine *engine = AudioEngine::engine();

    if (!data || size == 0)
        return false;

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, sampleRate);
    ma_result result = ma_decoder_init_memory(data, size, &config, m_decoder);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to decode sound." << std::endl;
        return false;
    }

    ma_result initResult = ma_sound_init_from_data_source(engine, m_decoder, MA_SOUND_FLAG_DECODE, NULL, m_sound);

    if (initResult != MA_SUCCESS) {
        std::cerr << "Failed to init sound." << std::endl;
        ma_decoder_uninit(m_decoder);
        return false;
    }

    m_loaded = true;
    ma_sound_set_volume(m_sound, m_volume);
    return true;
}

void AudioPlayer::setVolume(float volume)
{
    m_volume = volume;

    if (!m_loaded)
        return;

    ma_sound_set_volume(m_sound, volume);
}

void AudioPlayer::start()
{
    if (!m_loaded)
        return;

    if (isPlaying())
        stop();

    ma_result result = ma_sound_seek_to_pcm_frame(m_sound, 0);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to seek to PCM frame 0." << std::endl;
        m_started = false;
    }

    result = ma_sound_start(m_sound);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start sound." << std::endl;
        m_started = false;
    } else
        m_started = true;
}

void AudioPlayer::stop()
{
    if (!m_loaded)
        return;

    ma_result result = ma_sound_stop(m_sound);
    m_started = false;

    if (result != MA_SUCCESS)
        std::cerr << "Failed to stop sound." << std::endl;
}

bool AudioPlayer::isPlaying() const
{
    if (!m_loaded)
        return false;

    return m_started && !m_sound->atEnd;
}
