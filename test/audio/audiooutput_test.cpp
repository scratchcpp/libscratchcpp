#include <audio/audiooutput.h>
#include <audio/internal/audioplayer.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioOutputTest, Create)
{
    auto factory = AudioOutput::instance();
    std::shared_ptr<IAudioPlayer> player = factory->createAudioPlayer();
    ASSERT_TRUE(player);
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioPlayer>(player));
}
