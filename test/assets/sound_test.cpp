#include <scratchcpp/sound.h>
#include <scratchcpp/sprite.h>
#include <scratch/sound_p.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>
#include <enginemock.h>

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

TEST_F(SoundTest, SetEffect)
{
    Sound sound("sound1", "a", "wav");

    // Pitch
    EXPECT_CALL(*m_player, setPitch(0.125));
    sound.setEffect(Sound::Effect::Pitch, -400);

    EXPECT_CALL(*m_player, setPitch(0.125));
    sound.setEffect(Sound::Effect::Pitch, -360);

    EXPECT_CALL(*m_player, setPitch(0.5));
    sound.setEffect(Sound::Effect::Pitch, -120);

    EXPECT_CALL(*m_player, setPitch(1));
    sound.setEffect(Sound::Effect::Pitch, 0);

    EXPECT_CALL(*m_player, setPitch(2));
    sound.setEffect(Sound::Effect::Pitch, 120);

    EXPECT_CALL(*m_player, setPitch(8));
    sound.setEffect(Sound::Effect::Pitch, 360);

    EXPECT_CALL(*m_player, setPitch(8));
    sound.setEffect(Sound::Effect::Pitch, 400);

    // Pan
    EXPECT_CALL(*m_player, setPan(-1));
    sound.setEffect(Sound::Effect::Pan, -150);

    EXPECT_CALL(*m_player, setPan(-1));
    sound.setEffect(Sound::Effect::Pan, -100);

    EXPECT_CALL(*m_player, setPan(-0.5));
    sound.setEffect(Sound::Effect::Pan, -50);

    EXPECT_CALL(*m_player, setPan(0));
    sound.setEffect(Sound::Effect::Pan, 0);

    EXPECT_CALL(*m_player, setPan(0.5));
    sound.setEffect(Sound::Effect::Pan, 50);

    EXPECT_CALL(*m_player, setPan(1));
    sound.setEffect(Sound::Effect::Pan, 100);

    EXPECT_CALL(*m_player, setPan(1));
    sound.setEffect(Sound::Effect::Pan, 150);
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

TEST_F(SoundTest, Target)
{
    Sound sound("sound1", "a", "wav");
    ASSERT_EQ(sound.target(), nullptr);

    Target target;
    sound.setTarget(&target);
    ASSERT_EQ(sound.target(), &target);
}

TEST_F(SoundTest, Clone)
{
    auto sound = std::make_shared<Sound>("sound1", "a", "wav");
    sound->setRate(44100);
    sound->setSampleCount(10000);

    const char *data = "abc";
    void *dataPtr = const_cast<void *>(static_cast<const void *>(data));

    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(false));
    EXPECT_CALL(*m_player, load(3, dataPtr, 44100)).WillOnce(Return(true));
    sound->setData(3, dataPtr);

    auto clonePlayer = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(clonePlayer));
    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(true));
    EXPECT_CALL(*clonePlayer, loadCopy(m_player.get())).WillOnce(Return(true));
    EXPECT_CALL(*m_player, volume()).WillOnce(Return(0.45));
    EXPECT_CALL(*clonePlayer, setVolume(0.45));
    EXPECT_CALL(*m_player, pitch()).WillOnce(Return(1.25));
    EXPECT_CALL(*clonePlayer, setPitch(1.25));
    EXPECT_CALL(*m_player, pan()).WillOnce(Return(-0.75));
    EXPECT_CALL(*clonePlayer, setPan(-0.75));
    EXPECT_CALL(*clonePlayer, isLoaded()).WillOnce(Return(true));
    auto clone = sound->clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(clone->name(), sound->name());
    ASSERT_EQ(clone->id(), sound->id());
    ASSERT_EQ(clone->dataFormat(), sound->dataFormat());
    ASSERT_EQ(clone->rate(), sound->rate());
    ASSERT_EQ(clone->sampleCount(), sound->sampleCount());

    auto cloneClonePlayer = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(cloneClonePlayer));
    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(true));
    EXPECT_CALL(*cloneClonePlayer, loadCopy(m_player.get())).WillOnce(Return(true));
    EXPECT_CALL(*clonePlayer, volume()).WillOnce(Return(0.62));
    EXPECT_CALL(*cloneClonePlayer, setVolume(0.62));
    EXPECT_CALL(*clonePlayer, pitch()).WillOnce(Return(0.5));
    EXPECT_CALL(*cloneClonePlayer, setPitch(0.5));
    EXPECT_CALL(*clonePlayer, pan()).WillOnce(Return(0.25));
    EXPECT_CALL(*cloneClonePlayer, setPan(0.25));
    EXPECT_CALL(*cloneClonePlayer, isLoaded()).WillOnce(Return(true));
    auto cloneClone = clone->clone();
    ASSERT_TRUE(cloneClone);

    auto anotherClonePlayer = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(anotherClonePlayer));
    EXPECT_CALL(*m_player, isLoaded()).WillOnce(Return(false));
    EXPECT_CALL(*anotherClonePlayer, loadCopy).Times(0);
    EXPECT_CALL(*clonePlayer, volume()).WillOnce(Return(0.62));
    EXPECT_CALL(*anotherClonePlayer, setVolume(0.62));
    EXPECT_CALL(*clonePlayer, pitch()).WillOnce(Return(2));
    EXPECT_CALL(*anotherClonePlayer, setPitch(2));
    EXPECT_CALL(*clonePlayer, pan()).WillOnce(Return(1));
    EXPECT_CALL(*anotherClonePlayer, setPan(1));
    EXPECT_CALL(*anotherClonePlayer, isLoaded()).Times(0);
    auto anotherClone = clone->clone();
    ASSERT_TRUE(anotherClone);

    // Stopping/starting the sound should stop its clones
    auto anotherPlayer = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(m_playerFactory, createAudioPlayer()).WillOnce(Return(anotherPlayer));
    auto another = std::make_shared<Sound>("another", "c", "mp3");
    Sprite sprite;
    EngineMock engine;
    sprite.setEngine(&engine);

    EXPECT_CALL(engine, cloneLimit()).WillOnce(Return(-1));
    EXPECT_CALL(engine, initClone);
    EXPECT_CALL(engine, requestRedraw);
    EXPECT_CALL(engine, moveSpriteBehindOther);
    auto spriteClone = sprite.clone();

    EXPECT_CALL(*anotherPlayer, setVolume).Times(2);
    EXPECT_CALL(*m_player, setVolume);
    EXPECT_CALL(*clonePlayer, setVolume);
    sprite.addSound(another);
    sprite.addSound(sound);
    spriteClone->addSound(another);
    spriteClone->addSound(clone);

    EXPECT_CALL(*m_player, stop());
    EXPECT_CALL(*clonePlayer, stop());
    sound->stop();

    EXPECT_CALL(*m_player, stop());
    EXPECT_CALL(*clonePlayer, stop());
    clone->stop();

    EXPECT_CALL(*m_player, start());
    EXPECT_CALL(*clonePlayer, stop());
    sound->start();

    EXPECT_CALL(*m_player, stop());
    EXPECT_CALL(*clonePlayer, start());
    clone->start();
}
