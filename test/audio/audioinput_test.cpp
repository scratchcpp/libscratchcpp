#include <audio/audioinput.h>

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include <audio/internal/audioloudness.h>
#else
#include <audio/internal/audioloudnessstub.h>
#endif

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioInputTest, Create)
{
    auto factory = AudioInput::instance();
    std::shared_ptr<IAudioLoudness> audioLoudness = factory->audioLoudness();
    ASSERT_TRUE(audioLoudness);

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioLoudness>(audioLoudness));
#else
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioLoudnessStub>(audioLoudness));
#endif
}
