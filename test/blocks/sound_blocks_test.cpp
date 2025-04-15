#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratch/sound_p.h>
#include <enginemock.h>
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

            SoundPrivate::audioOutput = &m_outputMock;
        }

        void TearDown() override { SoundPrivate::audioOutput = nullptr; }

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
