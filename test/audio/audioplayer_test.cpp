#include <audio/internal/audioplayer.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(AudioPlayerTest, Volume)
{
    AudioPlayer player;
    ASSERT_EQ(player.volume(), 1);

    player.setVolume(0.86f);
    ASSERT_EQ(player.volume(), 0.86f);
}
