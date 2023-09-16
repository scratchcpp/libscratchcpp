#include <scratchcpp/sound.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(SoundTest, Constructors)
{
    Sound sound("sound1", "a", "svg");
    ASSERT_EQ(sound.id(), "a");
    ASSERT_EQ(sound.name(), "sound1");
    ASSERT_EQ(sound.dataFormat(), "svg");
    ASSERT_EQ(sound.fileName(), "a.svg");
    ASSERT_EQ(sound.rate(), 0);
    ASSERT_EQ(sound.sampleCount(), 0);
}

TEST(SoundTest, Rate)
{
    Sound sound("sound1", "a", "svg");

    sound.setRate(2);
    ASSERT_EQ(sound.rate(), 2);
}

TEST(SoundTest, SampleCount)
{
    Sound sound("sound1", "a", "svg");

    sound.setSampleCount(10);
    ASSERT_EQ(sound.sampleCount(), 10);
}
