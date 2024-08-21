#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sound.h>
#include <scratch/sound_p.h>
#include <enginemock.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>
#include <targetmock.h>

#include "../common.h"
#include "blocks/soundblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class SoundBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<SoundBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = std::make_shared<Block>(block->id() + "_menu", block->opcode() + "_menu");
                menu->setShadow(true);
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<SoundBlocks::Fields>(-1), selectedValue, static_cast<SoundBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Fields id, const std::string &value, SoundBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        Engine m_engine;
        EngineMock m_engineMock;
};

TEST_F(SoundBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Sound");
}

TEST_F(SoundBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(SoundBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_play", &SoundBlocks::compilePlay));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_playuntildone", &SoundBlocks::compilePlayUntilDone));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_stopallsounds", &SoundBlocks::compileStopAllSounds));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_changevolumeby", &SoundBlocks::compileChangeVolumeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_setvolumeto", &SoundBlocks::compileSetVolumeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_volume", &SoundBlocks::compileVolume));

    // Monitor names
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_section.get(), "sound_volume", &SoundBlocks::volumeMonitorName));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SOUND_MENU", SoundBlocks::SOUND_MENU));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VOLUME", SoundBlocks::VOLUME));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(SoundBlocksTest, Play)
{
    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2));
    EXPECT_CALL(*player1, setVolume);
    EXPECT_CALL(*player2, setVolume);
    Target target;
    target.addSound(std::make_shared<Sound>("test", "", ""));
    target.addSound(std::make_shared<Sound>("some sound", "", ""));
    Compiler compiler(&m_engineMock, &target);

    // start sound (some sound)
    auto block1 = std::make_shared<Block>("a", "sound_play");
    addDropdownInput(block1, "SOUND_MENU", SoundBlocks::SOUND_MENU, "some sound");

    // start sound (1)
    auto block2 = std::make_shared<Block>("b", "sound_play");
    addDropdownInput(block2, "SOUND_MENU", SoundBlocks::SOUND_MENU, "1");

    // start sound (5)
    auto block3 = std::make_shared<Block>("c", "sound_play");
    addDropdownInput(block3, "SOUND_MENU", SoundBlocks::SOUND_MENU, "5");

    // start sound (-3)
    auto block4 = std::make_shared<Block>("d", "sound_play");
    addDropdownInput(block4, "SOUND_MENU", SoundBlocks::SOUND_MENU, "-3");

    // start sound (nonexistent sound)
    auto block5 = std::make_shared<Block>("e", "sound_play");
    addDropdownInput(block5, "SOUND_MENU", SoundBlocks::SOUND_MENU, "nonexistent sound");

    // start sound (null block)
    auto block6 = std::make_shared<Block>("f", "sound_play");
    addDropdownInput(block6, "SOUND_MENU", SoundBlocks::SOUND_MENU, "", createNullBlock("g"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndex)).WillOnce(Return(2));
    compiler.setBlock(block1);
    SoundBlocks::compilePlay(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndex)).WillOnce(Return(2));
    compiler.setBlock(block2);
    SoundBlocks::compilePlay(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndex)).WillOnce(Return(2));
    compiler.setBlock(block3);
    SoundBlocks::compilePlay(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndex)).WillOnce(Return(2));
    compiler.setBlock(block4);
    SoundBlocks::compilePlay(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndex)).Times(0);
    compiler.setBlock(block5);
    SoundBlocks::compilePlay(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::play)).WillOnce(Return(3));
    compiler.setBlock(block6);
    SoundBlocks::compilePlay(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_CONST, 1, vm::OP_EXEC, 2, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_CONST, 3, vm::OP_EXEC, 2,
                                    vm::OP_NULL,  vm::OP_EXEC,  3, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, 0, 4, -4 }));

    SoundPrivate::audioOutput = nullptr;
}

TEST_F(SoundBlocksTest, PlayImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::playByIndex, &SoundBlocks::play };
    static Value constValues[] = { 2, 5, -1, "test", "nonexistent", "1", "4", "-3" };

    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
    EXPECT_CALL(*player1, setVolume);
    EXPECT_CALL(*player2, setVolume);
    EXPECT_CALL(*player3, setVolume);
    TargetMock target;
    EXPECT_CALL(target, isStage()).WillRepeatedly(Return(true));
    target.addSound(std::make_shared<Sound>("some sound", "", ""));
    target.addSound(std::make_shared<Sound>("test", "", ""));
    target.addSound(std::make_shared<Sound>("another sound", "", ""));

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(*player3, start());
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player3, start());
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player3, start());
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player2, start());
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player1, start());
    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player1, start());
    vm.reset();
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(*player3, start());
    vm.reset();
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    SoundPrivate::audioOutput = nullptr;
}

TEST_F(SoundBlocksTest, PlayUntilDone)
{
    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2));
    EXPECT_CALL(*player1, setVolume);
    EXPECT_CALL(*player2, setVolume);
    Target target;
    target.addSound(std::make_shared<Sound>("test", "", ""));
    target.addSound(std::make_shared<Sound>("some sound", "", ""));
    Compiler compiler(&m_engineMock, &target);

    // play sound (some sound) until done
    auto block1 = std::make_shared<Block>("a", "sound_playuntildone");
    addDropdownInput(block1, "SOUND_MENU", SoundBlocks::SOUND_MENU, "some sound");

    // play sound (1) until done
    auto block2 = std::make_shared<Block>("b", "sound_playuntildone");
    addDropdownInput(block2, "SOUND_MENU", SoundBlocks::SOUND_MENU, "1");

    // play sound (5) until done
    auto block3 = std::make_shared<Block>("c", "sound_playuntildone");
    addDropdownInput(block3, "SOUND_MENU", SoundBlocks::SOUND_MENU, "5");

    // play sound (-3) until done
    auto block4 = std::make_shared<Block>("d", "sound_playuntildone");
    addDropdownInput(block4, "SOUND_MENU", SoundBlocks::SOUND_MENU, "-3");

    // play sound (nonexistent sound) until done
    auto block5 = std::make_shared<Block>("e", "sound_playuntildone");
    addDropdownInput(block5, "SOUND_MENU", SoundBlocks::SOUND_MENU, "nonexistent sound");

    // play sound (null block) until done
    auto block6 = std::make_shared<Block>("f", "sound_playuntildone");
    addDropdownInput(block6, "SOUND_MENU", SoundBlocks::SOUND_MENU, "", createNullBlock("g"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndexUntilDone)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSoundByIndex)).WillOnce(Return(3));
    compiler.setBlock(block1);
    SoundBlocks::compilePlayUntilDone(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndexUntilDone)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSoundByIndex)).WillOnce(Return(3));
    compiler.setBlock(block2);
    SoundBlocks::compilePlayUntilDone(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndexUntilDone)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSoundByIndex)).WillOnce(Return(3));
    compiler.setBlock(block3);
    SoundBlocks::compilePlayUntilDone(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndexUntilDone)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSoundByIndex)).WillOnce(Return(3));
    compiler.setBlock(block4);
    SoundBlocks::compilePlayUntilDone(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playByIndexUntilDone)).Times(0);
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playUntilDone)).Times(0);
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSoundByIndex)).Times(0);
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSound)).Times(0);
    compiler.setBlock(block5);
    SoundBlocks::compilePlayUntilDone(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::playUntilDone)).WillOnce(Return(4));
    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::checkSound)).WillOnce(Return(5));
    compiler.setBlock(block6);
    SoundBlocks::compilePlayUntilDone(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              2,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              3,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              3,
              vm::OP_NULL,
              vm::OP_EXEC,
              4,
              vm::OP_EXEC,
              5,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, 0, 4, -4 }));

    SoundPrivate::audioOutput = nullptr;
}

TEST_F(SoundBlocksTest, PlayUntilDoneImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::playByIndexUntilDone, &SoundBlocks::checkSoundByIndex, &SoundBlocks::playUntilDone, &SoundBlocks::checkSound };
    static Value constValues[] = { 2, 5, -1, "test", "nonexistent", "1", "4", "-3" };

    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
    EXPECT_CALL(*player1, setVolume);
    EXPECT_CALL(*player2, setVolume);
    EXPECT_CALL(*player3, setVolume);
    TargetMock target;
    EXPECT_CALL(target, isStage()).WillRepeatedly(Return(true));
    target.addSound(std::make_shared<Sound>("some sound", "", ""));
    target.addSound(std::make_shared<Sound>("test", "", ""));
    target.addSound(std::make_shared<Sound>("another sound", "", ""));

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    // bytecode1
    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(true));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode2
    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(true));
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode3
    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode4
    EXPECT_CALL(*player2, start());
    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(true));
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(false));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode5
    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode6
    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(true));
    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode7
    EXPECT_CALL(*player1, start());
    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(true));
    vm.reset();
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    // bytecode8 + bytecode1
    VirtualMachine vm2(&target, nullptr, nullptr);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);

    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(true));
    vm.reset();
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(*player3, start());
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(true));
    vm2.setBytecode(bytecode1);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 1);
    ASSERT_FALSE(vm2.atEnd());

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());

    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(false));
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(vm2.atEnd());

    SoundPrivate::audioOutput = nullptr;
}

TEST_F(SoundBlocksTest, StopAllSounds)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sound_stopallsounds");

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::stopAllSounds)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SoundBlocks::compileStopAllSounds(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SoundBlocksTest, StopAllSoundsImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::stopAllSounds };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);

    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, stopSounds());
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(SoundBlocksTest, ChangeVolumeBy)
{
    Compiler compiler(&m_engineMock);

    // change volume by (53.05)
    auto block1 = std::make_shared<Block>("a", "sound_changevolumeby");
    addValueInput(block1, "VOLUME", SoundBlocks::VOLUME, 53.05);

    // change volume by (-2.13)
    auto block2 = std::make_shared<Block>("b", "sound_changevolumeby");
    addValueInput(block2, "VOLUME", SoundBlocks::VOLUME, -2.13);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::changeVolumeBy)).Times(2).WillRepeatedly(Return(0));

    compiler.init();

    compiler.setBlock(block1);
    SoundBlocks::compileChangeVolumeBy(&compiler);

    compiler.setBlock(block2);
    SoundBlocks::compileChangeVolumeBy(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 53.05, -2.13 }));
}

TEST_F(SoundBlocksTest, ChangeVolumeByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::changeVolumeBy };
    static Value constValues[] = { 53.05, -2.13 };

    Target target;
    target.setVolume(42.4);

    VirtualMachine vm(&target, nullptr, nullptr);

    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.volume() * 100) / 100, 95.45);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.volume() * 100) / 100, 93.32);
}

TEST_F(SoundBlocksTest, SetVolumeTo)
{
    Compiler compiler(&m_engineMock);

    // set volume to (43.409) %
    auto block = std::make_shared<Block>("a", "sound_setvolumeto");
    addValueInput(block, "VOLUME", SoundBlocks::VOLUME, 43.409);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::setVolumeTo)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SoundBlocks::compileSetVolumeTo(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 43.409);
}

TEST_F(SoundBlocksTest, SetVolumeToImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::setVolumeTo };
    static Value constValues[] = { 43.409 };

    Target target;
    target.setVolume(42.4);

    VirtualMachine vm(&target, nullptr, nullptr);

    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.volume(), 43.409);
}

TEST_F(SoundBlocksTest, Volume)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sound_volume");

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::volume)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SoundBlocks::compileVolume(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SoundBlocksTest, VolumeMonitorName)
{
    ASSERT_EQ(SoundBlocks::volumeMonitorName(nullptr), "volume");
}

TEST_F(SoundBlocksTest, VolumeImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::volume };

    Target target;
    target.setVolume(42.4);

    VirtualMachine vm(&target, nullptr, nullptr);

    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 42.4);
}
