#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include <audio/internal/audioengine.h>
#else
#include <audio/internal/audioenginestub.h>
#endif

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioEngineTest, Instance)
{
    IAudioEngine *instance = IAudioEngine::instance();
    ASSERT_TRUE(instance);

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
    ASSERT_TRUE(dynamic_cast<AudioEngine *>(instance));
#else
    ASSERT_TRUE(dynamic_cast<AudioEngineStub *>(instance));
#endif
}

TEST(AudioEngineTest, Volume)
{
    IAudioEngine *engine = IAudioEngine::instance();
    ASSERT_TRUE(engine);
    ASSERT_EQ(engine->volume(), 1.0f);

    engine->setVolume(0.86f);
    ASSERT_EQ(engine->volume(), 0.86f);
}
