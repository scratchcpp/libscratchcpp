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

TEST(AudioPlayerTest, Pitch)
{
    AudioPlayer player;
    ASSERT_EQ(player.pitch(), 1);

    player.setPitch(1.5f);
    ASSERT_EQ(player.pitch(), 1.5f);
}
