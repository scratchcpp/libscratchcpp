#include <audio/audiooutput.h>

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include <audio/internal/audioplayer.h>
#else
#include <audio/internal/audioplayerstub.h>
#endif

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioOutputTest, Create)
{
    auto factory = AudioOutput::instance();
    std::shared_ptr<IAudioPlayer> player = factory->createAudioPlayer();
    ASSERT_TRUE(player);

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioPlayer>(player));
#else
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioPlayerStub>(player));
#endif
}
