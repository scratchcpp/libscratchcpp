#include <audio/audioplayerfactory.h>
#include <audio/audioplayer.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioPlayerFactoryTest, Create)
{
    auto factory = AudioPlayerFactory::instance();
    std::shared_ptr<IAudioPlayer> player = factory->create();
    ASSERT_TRUE(player);
    ASSERT_TRUE(std::dynamic_pointer_cast<AudioPlayer>(player));
}
