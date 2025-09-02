#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/list.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratch/sound_p.h>
#include <enginemock.h>
#include <targetmock.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>

#include "../common.h"
#include "blocks/soundblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class SoundBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<SoundBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            m_extension->onInit(m_engine);

            SoundPrivate::setAudioOutput(&m_outputMock);
        }

        void TearDown() override { SoundPrivate::setAudioOutput(nullptr); }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
        AudioOutputMock m_outputMock;
};

TEST_F(SoundBlocksTest, Play_NoSounds)
{
    auto sprite = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("sound_play");
    builder.addDropdownInput("SOUND_MENU", "pop");
    builder.build();
    builder.run();
}

TEST_F(SoundBlocksTest, Play_InvalidSoundName)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("test", "b", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addDropdownInput("SOUND_MENU", "meow");
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_SoundName)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addDropdownInput("SOUND_MENU", "meow");
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_NumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", 3);
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start());
    builder.run();
}

TEST_F(SoundBlocksTest, Play_PositiveOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("4", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", 4);
    builder.build();

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_NegativeOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("-4", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", -4);
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_InvalidNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", std::numeric_limits<double>::quiet_NaN());
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", std::numeric_limits<double>::infinity());
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", -std::numeric_limits<double>::infinity());
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_StringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", "3");
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start());
    builder.run();
}

TEST_F(SoundBlocksTest, Play_StringIndexExists)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", "3");
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_OutOfRangeStringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", "+7.0");
    builder.build();

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_WhitespaceString)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("test", "a", "mp3"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("pop", "b", "wav"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", "");
    builder.addBlock("sound_play");
    builder.addValueInput("SOUND_MENU", "   ");
    builder.build();

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, Play_Stage)
{
    auto stage = std::make_shared<Stage>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    stage->addSound(std::make_shared<Sound>("test", "a", "mp3"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    stage->addSound(std::make_shared<Sound>("pop", "b", "wav"));

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_play");
    builder.addDropdownInput("SOUND_MENU", "test");
    builder.build();

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    builder.run();
}

TEST_F(SoundBlocksTest, PlayUntilDone_NoSounds)
{
    auto sprite = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "pop");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_InvalidSoundName)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("test", "b", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "meow");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_SoundName)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "meow");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player2, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_KillWaitingThread)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    auto sound2 = std::make_shared<Sound>("meow", "b", "wav");
    sprite->addSound(sound2);

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "meow");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player2, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());
    ASSERT_EQ(sound2->owner(), &thread);

    EXPECT_CALL(*player2, stop());
    m_engine->threadAboutToStop()(&thread);
    thread.kill();
    ASSERT_EQ(sound2->owner(), nullptr);
}

TEST_F(SoundBlocksTest, PlayUntilDone_StopWaitingSound)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    auto sound2 = std::make_shared<Sound>("meow", "b", "wav");
    sprite->addSound(sound2);

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "meow");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player2, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());
    ASSERT_EQ(sound2->owner(), &thread);

    sound2->stop();
    EXPECT_CALL(*player2, isPlaying()).Times(0);
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_NumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", 3);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_PositiveOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("4", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", 4);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player1, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_NegativeOutOfRangeNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("-4", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", -4);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player2, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_InvalidNumberIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", std::numeric_limits<double>::quiet_NaN());
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", std::numeric_limits<double>::infinity());
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", -std::numeric_limits<double>::infinity());
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_StringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("meow", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", "3");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_StringIndexExists)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", "3");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player2, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_OutOfRangeStringIndex)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("pop", "a", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("3", "b", "wav"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player3));
    sprite->addSound(std::make_shared<Sound>("test", "c", "mp3"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", "+7.0");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player3, start).Times(0);
    EXPECT_CALL(*player1, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_WhitespaceString)
{
    auto sprite = std::make_shared<Sprite>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    sprite->addSound(std::make_shared<Sound>("test", "a", "mp3"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    sprite->addSound(std::make_shared<Sound>("pop", "b", "wav"));

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", "");
    builder.addBlock("sound_playuntildone");
    builder.addValueInput("SOUND_MENU", "   ");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start).Times(0);
    EXPECT_CALL(*player2, start).Times(0);
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, PlayUntilDone_Stage)
{
    auto stage = std::make_shared<Stage>();
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player1));
    stage->addSound(std::make_shared<Sound>("test", "a", "mp3"));

    EXPECT_CALL(m_outputMock, createAudioPlayer()).WillOnce(Return(player2));
    stage->addSound(std::make_shared<Sound>("pop", "b", "wav"));

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_playuntildone");
    builder.addDropdownInput("SOUND_MENU", "test");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, stage.get());
    auto code = compiler.compile(block);
    Script script(stage.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(stage.get(), &m_engineMock, &script);

    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player2, start).Times(0);
    EXPECT_CALL(*player1, isPlaying()).WillRepeatedly(Return(true));
    thread.run();
    ASSERT_FALSE(thread.isFinished());

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    thread.run();
    ASSERT_TRUE(thread.isFinished());
}

TEST_F(SoundBlocksTest, StopAllSounds_Sprite)
{
    auto sprite = std::make_shared<Sprite>();

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_stopallsounds");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, stopSounds());
    thread.run();
}

TEST_F(SoundBlocksTest, StopAllSounds_Stage)
{
    auto stage = std::make_shared<Stage>();

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_stopallsounds");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, stage.get());
    auto code = compiler.compile(block);
    Script script(stage.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(stage.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, stopSounds());
    thread.run();
}

TEST_F(SoundBlocksTest, SetEffectTo_Invalid)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_seteffectto");
    builder.addDropdownField("EFFECT", "abc");
    builder.addValueInput("VALUE", 0);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, setSoundEffectValue).Times(0);
    thread.run();
}

TEST_F(SoundBlocksTest, SetEffectTo_Pitch)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_seteffectto");
    builder.addDropdownField("EFFECT", "PITCH");
    builder.addValueInput("VALUE", 75.2);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, setSoundEffectValue(Sound::Effect::Pitch, 75.2));
    thread.run();
}

TEST_F(SoundBlocksTest, SetEffectTo_Pan)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_seteffectto");
    builder.addDropdownField("EFFECT", "PAN");
    builder.addValueInput("VALUE", -23.8);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, setSoundEffectValue(Sound::Effect::Pan, -23.8));
    thread.run();
}

TEST_F(SoundBlocksTest, ChangeEffectBy_Invalid)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_changeeffectby");
    builder.addDropdownField("EFFECT", "abc");
    builder.addValueInput("VALUE", 1);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, soundEffectValue).Times(0);
    EXPECT_CALL(*target, setSoundEffectValue).Times(0);
    thread.run();
}

TEST_F(SoundBlocksTest, ChangeEffectBy_Pitch)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_changeeffectby");
    builder.addDropdownField("EFFECT", "PITCH");
    builder.addValueInput("VALUE", 75.2);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, soundEffectValue(Sound::Effect::Pitch)).WillOnce(Return(-28.6));
    EXPECT_CALL(*target, setSoundEffectValue(Sound::Effect::Pitch, 46.6));
    thread.run();
}

TEST_F(SoundBlocksTest, ChangeEffectBy_Pan)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_changeeffectby");
    builder.addDropdownField("EFFECT", "PAN");
    builder.addValueInput("VALUE", -23.8);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, soundEffectValue(Sound::Effect::Pan)).WillOnce(Return(12.5));
    EXPECT_CALL(*target, setSoundEffectValue(Sound::Effect::Pan, -11.3));
    thread.run();
}

TEST_F(SoundBlocksTest, ClearEffects)
{
    auto target = std::make_shared<TargetMock>();

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("sound_cleareffects");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);

    EXPECT_CALL(*target, clearSoundEffects());
    EXPECT_CALL(*target, setSoundEffectValue).Times(0);
    thread.run();
}

TEST_F(SoundBlocksTest, ChangeVolumeBy_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setVolume(42);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_changevolumeby");
    builder.addValueInput("VOLUME", 65.2);
    builder.build();

    sprite->setVolume(1.9);
    builder.run();
    ASSERT_EQ(std::round(sprite->volume() * 100) / 100, 67.1);
}

TEST_F(SoundBlocksTest, ChangeVolumeBy_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setVolume(18);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_changevolumeby");
    builder.addValueInput("VOLUME", -38.4);
    builder.build();

    stage->setVolume(78.5);
    builder.run();
    ASSERT_EQ(stage->volume(), 40.1);
}

TEST_F(SoundBlocksTest, SetVolumeTo_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setVolume(18);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_setvolumeto");
    builder.addValueInput("VOLUME", 65.2);
    builder.build();

    sprite->setVolume(1.9);
    builder.run();
    ASSERT_EQ(sprite->volume(), 65.2);
}

TEST_F(SoundBlocksTest, SetVolumeTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setVolume(42);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_setvolumeto");
    builder.addValueInput("VOLUME", 38.4);
    builder.build();

    stage->setVolume(78.5);
    builder.run();
    ASSERT_EQ(stage->volume(), 38.4);
}

TEST_F(SoundBlocksTest, Volume_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setVolume(50);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sound_volume");
    builder.captureBlockReturnValue();
    builder.build();

    sprite->setVolume(1.9);
    builder.run();
    ASSERT_EQ(sprite->volume(), 1.9);

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 1);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 1.9);
}

TEST_F(SoundBlocksTest, Volume_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setVolume(75);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sound_volume");
    builder.captureBlockReturnValue();
    builder.build();

    stage->setVolume(43.7);
    builder.run();
    ASSERT_EQ(stage->volume(), 43.7);

    List *list = builder.capturedValues();
    ASSERT_EQ(list->size(), 1);
    ASSERT_EQ(Value(list->data()[0]).toDouble(), 43.7);
}
