#include <scratchcpp/sound.h>
#include <scratch/sound_p.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

class SoundTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_player = std::make_shared<AudioPlayerMock>();
            SoundPrivate::audioOutput = &m_playerFactory;
            EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(m_player));
        }

        void TearDown() override { SoundPrivate::audioOutput = nullptr; }

        AudioOutputMock m_playerFactory;
        std::shared_ptr<AudioPlayerMock> m_player;
};

TEST_F(SoundTest, Constructors)
{
    Sound sound("sound1", "a", "wav");
    ASSERT_EQ(sound.id(), "a");
    ASSERT_EQ(sound.name(), "sound1");
    ASSERT_EQ(sound.dataFormat(), "wav");
    ASSERT_EQ(sound.fileName(), "a.wav");
    ASSERT_EQ(sound.rate(), 0);
    ASSERT_EQ(sound.sampleCount(), 0);
}

TEST_F(SoundTest, Rate)
{
    Sound sound("sound1", "a", "wav");

    sound.setRate(2);
    ASSERT_EQ(sound.rate(), 2);
}

TEST_F(SoundTest, SampleCount)
{
    Sound sound("sound1", "a", "wav");

    sound.setSampleCount(10);
    ASSERT_EQ(sound.sampleCount(), 10);
}

TEST_F(SoundTest, ProcessData)
{
    Sound sound("sound1", "a", "wav");
    sound.setRate(44100);

    const char *data = "abc";
    void *dataPtr = const_cast<void *>(static_cast<const void *>(data));

    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(false));
    EXPECT_CALL(*m_player, load(3, dataPtr, 44100)).WillOnce(Return(true));
    sound.setData(3, dataPtr);

    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(true));
    EXPECT_CALL(*m_player, load).Times(0);
    sound.setData(3, dataPtr);
}

TEST_F(SoundTest, SetVolume)
{
    Sound sound("sound1", "a", "wav");

    EXPECT_CALL(*m_player, setVolume(0.5604));
    sound.setVolume(56.04);
}

TEST_F(SoundTest, Start)
{
    Sound sound("sound1", "a", "wav");

    EXPECT_CALL(*m_player, start());
    sound.start();
}

TEST_F(SoundTest, Stop)
{
    Sound sound("sound1", "a", "wav");

    EXPECT_CALL(*m_player, stop());
    sound.stop();
}

TEST_F(SoundTest, IsPlaying)
{
    Sound sound("sound1", "a", "wav");

    EXPECT_CALL(*m_player, isPlaying()).WillOnce(Return(true));
    ASSERT_TRUE(sound.isPlaying());

    EXPECT_CALL(*m_player, isPlaying()).WillOnce(Return(false));
    ASSERT_FALSE(sound.isPlaying());

    SoundPrivate::audioOutput = nullptr;
}

TEST_F(SoundTest, Clone)
{
    Sound sound("sound1", "a", "wav");
    sound.setRate(44100);
    sound.setSampleCount(10000);

    const char *data = "abc";
    void *dataPtr = const_cast<void *>(static_cast<const void *>(data));

    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(false));
    EXPECT_CALL(*m_player, load(3, dataPtr, 44100)).WillOnce(Return(true));
    sound.setData(3, dataPtr);

    auto clonePlayer = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(clonePlayer));
    EXPECT_CALL(*clonePlayer, loadCopy(m_player.get())).WillOnce(Return(true));
    EXPECT_CALL(*m_player, volume()).WillOnce(Return(0.45));
    EXPECT_CALL(*clonePlayer, setVolume(0.45));
    EXPECT_CALL(*clonePlayer, isLoaded()).WillOnce(Return(true));
    auto clone = sound.clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(clone->name(), sound.name());
    ASSERT_EQ(clone->id(), sound.id());
    ASSERT_EQ(clone->dataFormat(), sound.dataFormat());
    ASSERT_EQ(clone->rate(), sound.rate());
    ASSERT_EQ(clone->sampleCount(), sound.sampleCount());
}
