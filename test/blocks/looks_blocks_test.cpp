#include <scratchcpp/compiler.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>
#include <enginemock.h>
#include <randomgeneratormock.h>
#include <graphicseffectmock.h>
#include <clockmock.h>

#include "../common.h"
#include "blocks/looksblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"
#include "engine/internal/randomgenerator.h"
#include "engine/internal/clock.h"

using namespace libscratchcpp;

using ::testing::Return;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<LooksBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any looks block
        std::shared_ptr<Block> createLooksBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = createLooksBlock(block->id() + "_menu", block->opcode() + "_menu");
                menu->setShadow(true);
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<LooksBlocks::Fields>(-1), selectedValue, static_cast<LooksBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Fields id, const std::string &value, LooksBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(LooksBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Looks");
}

TEST_F(LooksBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(LooksBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_sayforsecs", &LooksBlocks::compileSayForSecs));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_say", &LooksBlocks::compileSay));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_thinkforsecs", &LooksBlocks::compileThinkForSecs));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_think", &LooksBlocks::compileThink));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_show", &LooksBlocks::compileShow));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_hide", &LooksBlocks::compileHide));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_changeeffectby", &LooksBlocks::compileChangeEffectBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_seteffectto", &LooksBlocks::compileSetEffectTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_cleargraphiceffects", &LooksBlocks::compileClearGraphicEffects));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_changesizeby", &LooksBlocks::compileChangeSizeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_setsizeto", &LooksBlocks::compileSetSizeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_size", &LooksBlocks::compileSize));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchcostumeto", &LooksBlocks::compileSwitchCostumeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_nextcostume", &LooksBlocks::compileNextCostume));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchbackdropto", &LooksBlocks::compileSwitchBackdropTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchbackdroptoandwait", &LooksBlocks::compileSwitchBackdropToAndWait));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_nextbackdrop", &LooksBlocks::compileNextBackdrop));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_gotofrontback", &LooksBlocks::compileGoToFrontBack));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_goforwardbackwardlayers", &LooksBlocks::compileGoForwardBackwardLayers));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_costumenumbername", &LooksBlocks::compileCostumeNumberName));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_backdropnumbername", &LooksBlocks::compileBackdropNumberName));

    // Monitor names
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_section.get(), "looks_costumenumbername", &LooksBlocks::costumeNumberNameMonitorName));
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_section.get(), "looks_backdropnumbername", &LooksBlocks::backdropNumberNameMonitorName));
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_section.get(), "looks_size", &LooksBlocks::sizeMonitorName));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "MESSAGE", LooksBlocks::MESSAGE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SECS", LooksBlocks::SECS));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "CHANGE", LooksBlocks::CHANGE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SIZE", LooksBlocks::SIZE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "COSTUME", LooksBlocks::COSTUME));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "BACKDROP", LooksBlocks::BACKDROP));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VALUE", LooksBlocks::VALUE));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "NUMBER_NAME", LooksBlocks::NUMBER_NAME));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "EFFECT", LooksBlocks::EFFECT));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "FRONT_BACK", LooksBlocks::FRONT_BACK));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "FORWARD_BACKWARD", LooksBlocks::FORWARD_BACKWARD));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "number", LooksBlocks::Number));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "name", LooksBlocks::Name));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "COLOR", LooksBlocks::ColorEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "FISHEYE", LooksBlocks::FisheyeEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "WHIRL", LooksBlocks::WhirlEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "PIXELATE", LooksBlocks::PixelateEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "MOSAIC", LooksBlocks::MosaicEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "BRIGHTNESS", LooksBlocks::BrightnessEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "GHOST", LooksBlocks::GhostEffect));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "front", LooksBlocks::Front));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "back", LooksBlocks::Back));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "forward", LooksBlocks::Forward));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "backward", LooksBlocks::Backward));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(LooksBlocksTest, SayForSecs)
{
    Compiler compiler(&m_engineMock);

    // say "Hello!" for 3.5 seconds
    auto block = std::make_shared<Block>("a", "looks_sayforsecs");
    addValueInput(block, "MESSAGE", LooksBlocks::MESSAGE, "Hello!");
    addValueInput(block, "SECS", LooksBlocks::SECS, 3.5);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::startSayForSecs)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::sayForSecs)).WillOnce(Return(1));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSayForSecs(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "Hello!", 3.5 }));
}

TEST_F(LooksBlocksTest, SayForSecsImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::startSayForSecs, &LooksBlocks::sayForSecs, &LooksBlocks::say };
    static Value constValues[] = { "test", 5.5, "hello" };

    Target target;
    target.setBubbleType(Target::BubbleType::Think);
    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.setBytecode(bytecode1);

    ClockMock clock;
    LooksBlocks::clock = &clock;

    std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(1000));
    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(6450));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
    target.setBubbleType(Target::BubbleType::Think);
    target.setBubbleText("another");
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "another");

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(6500));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_TRUE(target.bubbleText().empty());

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_TRUE(target.bubbleText().empty());

    // Run the say block while waiting
    VirtualMachine vm2(&target, &m_engineMock, nullptr);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.setBytecode(bytecode2);

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    vm2.run();
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "hello");

    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "hello");

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "hello");

    // Run the say for secs block while waiting
    vm2.reset();
    vm2.setBytecode(bytecode3);

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm2.reset();
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "hello");

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.reset();
    vm.run();
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    LooksBlocks::clock = Clock::instance().get();
}

TEST_F(LooksBlocksTest, Say)
{
    Compiler compiler(&m_engineMock);

    // say "Hello!"
    auto block = std::make_shared<Block>("a", "looks_say");
    addValueInput(block, "MESSAGE", LooksBlocks::MESSAGE, "Hello!");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::say)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSay(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toString(), "Hello!");
}

TEST_F(LooksBlocksTest, SayImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::say };
    static Value constValues[] = { "test" };

    Target target;
    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");

    target.setBubbleType(Target::BubbleType::Think);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "test");
}

TEST_F(LooksBlocksTest, ThinkForSecs)
{
    Compiler compiler(&m_engineMock);

    // think "Hmm..." for 3.5 seconds
    auto block = std::make_shared<Block>("a", "looks_thinkforsecs");
    addValueInput(block, "MESSAGE", LooksBlocks::MESSAGE, "Hmm...");
    addValueInput(block, "SECS", LooksBlocks::SECS, 3.5);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::startThinkForSecs)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::thinkForSecs)).WillOnce(Return(1));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileThinkForSecs(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "Hmm...", 3.5 }));
}

TEST_F(LooksBlocksTest, ThinkForSecsImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::startThinkForSecs, &LooksBlocks::thinkForSecs, &LooksBlocks::think };
    static Value constValues[] = { "test", 5.5, "hello" };

    Target target;
    target.setBubbleType(Target::BubbleType::Say);
    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.setBytecode(bytecode1);

    ClockMock clock;
    LooksBlocks::clock = &clock;

    std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(1000));
    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(6450));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
    target.setBubbleType(Target::BubbleType::Say);
    target.setBubbleText("another");
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleText(), "another");

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(6500));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_TRUE(target.bubbleText().empty());

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
    ASSERT_TRUE(target.bubbleText().empty());

    // Run the say block while waiting
    VirtualMachine vm2(&target, &m_engineMock, nullptr);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.setBytecode(bytecode2);

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    vm2.run();
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "hello");

    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "hello");

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "hello");

    // Run the say for secs block while waiting
    vm2.reset();
    vm2.setBytecode(bytecode3);

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm2.reset();
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) != LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "hello");

    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.reset();
    vm.run();
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) == LooksBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(LooksBlocks::m_timeMap.find(&vm2) == LooksBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm2.atEnd());
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    LooksBlocks::clock = Clock::instance().get();
}

TEST_F(LooksBlocksTest, Think)
{
    Compiler compiler(&m_engineMock);

    // say "Hmm..."
    auto block = std::make_shared<Block>("a", "looks_think");
    addValueInput(block, "MESSAGE", LooksBlocks::MESSAGE, "Hmm...");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::think)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileThink(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toString(), "Hmm...");
}

TEST_F(LooksBlocksTest, ThinkImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::think };
    static Value constValues[] = { "test" };

    Target target;
    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");

    target.setBubbleType(Target::BubbleType::Say);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleText(), "test");
}

TEST_F(LooksBlocksTest, Show)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_show");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::show)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileShow(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, ShowImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::show };

    Sprite sprite;
    sprite.setVisible(false);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());
}

TEST_F(LooksBlocksTest, Hide)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_hide");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::hide)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileHide(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, HideImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::hide };

    Sprite sprite;
    sprite.setVisible(true);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(sprite.visible());

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(sprite.visible());
}

TEST_F(LooksBlocksTest, ChangeEffectBy)
{
    Compiler compiler(&m_engineMock);

    // change [custom1] effect by 12.5
    auto block1 = std::make_shared<Block>("a", "looks_changeeffectby");
    addDropdownField(block1, "EFFECT", LooksBlocks::EFFECT, "custom1", static_cast<LooksBlocks::FieldValues>(-1));
    addValueInput(block1, "CHANGE", LooksBlocks::CHANGE, 12.5);

    // change [custom2] effect by -78.15
    auto block2 = std::make_shared<Block>("b", "looks_changeeffectby");
    addDropdownField(block2, "EFFECT", LooksBlocks::EFFECT, "custom2", static_cast<LooksBlocks::FieldValues>(-1));
    addValueInput(block2, "CHANGE", LooksBlocks::CHANGE, -78.15);

    // change [color] effect by 154.152
    auto block3 = std::make_shared<Block>("c", "looks_changeeffectby");
    addDropdownField(block3, "EFFECT", LooksBlocks::EFFECT, "color", LooksBlocks::ColorEffect);
    addValueInput(block3, "CHANGE", LooksBlocks::CHANGE, 154.152);

    // change [fisheye] effect by -124.054
    auto block4 = std::make_shared<Block>("d", "looks_changeeffectby");
    addDropdownField(block4, "EFFECT", LooksBlocks::EFFECT, "fisheye", LooksBlocks::FisheyeEffect);
    addValueInput(block4, "CHANGE", LooksBlocks::CHANGE, -124.054);

    // change [whirl] effect by 45.858
    auto block5 = std::make_shared<Block>("e", "looks_changeeffectby");
    addDropdownField(block5, "EFFECT", LooksBlocks::EFFECT, "whirl", LooksBlocks::WhirlEffect);
    addValueInput(block5, "CHANGE", LooksBlocks::CHANGE, 45.858);

    // change [pixelate] effect by -0.15
    auto block6 = std::make_shared<Block>("f", "looks_changeeffectby");
    addDropdownField(block6, "EFFECT", LooksBlocks::EFFECT, "pixelate", LooksBlocks::PixelateEffect);
    addValueInput(block6, "CHANGE", LooksBlocks::CHANGE, -0.15);

    // change [mosaic] effect by 0.84
    auto block7 = std::make_shared<Block>("g", "looks_changeeffectby");
    addDropdownField(block7, "EFFECT", LooksBlocks::EFFECT, "mosaic", LooksBlocks::MosaicEffect);
    addValueInput(block7, "CHANGE", LooksBlocks::CHANGE, 0.84);

    // change [brightness] effect by 40.87
    auto block8 = std::make_shared<Block>("h", "looks_changeeffectby");
    addDropdownField(block8, "EFFECT", LooksBlocks::EFFECT, "brightness", LooksBlocks::BrightnessEffect);
    addValueInput(block8, "CHANGE", LooksBlocks::CHANGE, 40.87);

    // change [ghost] effect by 50.04
    auto block9 = std::make_shared<Block>("i", "looks_changeeffectby");
    addDropdownField(block9, "EFFECT", LooksBlocks::EFFECT, "ghost", LooksBlocks::GhostEffect);
    addValueInput(block9, "CHANGE", LooksBlocks::CHANGE, 50.04);

    auto effect1 = std::make_shared<GraphicsEffectMock>();
    auto effect2 = std::make_shared<GraphicsEffectMock>();
    EXPECT_CALL(*effect1, name()).WillOnce(Return("custom1"));
    ScratchConfiguration::registerGraphicsEffect(effect1);
    EXPECT_CALL(*effect2, name()).WillOnce(Return("custom2"));
    ScratchConfiguration::registerGraphicsEffect(effect2);

    compiler.init();
    LooksBlocks::m_customGraphicsEffects.clear();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeEffectBy)).Times(3).WillRepeatedly(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileChangeEffectBy(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());

    compiler.setBlock(block1);
    LooksBlocks::compileChangeEffectBy(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());

    compiler.setBlock(block2);
    LooksBlocks::compileChangeEffectBy(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(1), effect2.get());

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeColorEffectBy)).WillOnce(Return(1));
    compiler.setBlock(block3);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeFisheyeEffectBy)).WillOnce(Return(2));
    compiler.setBlock(block4);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeWhirlEffectBy)).WillOnce(Return(3));
    compiler.setBlock(block5);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changePixelateEffectBy)).WillOnce(Return(4));
    compiler.setBlock(block6);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeMosaicEffectBy)).WillOnce(Return(5));
    compiler.setBlock(block7);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeBrightnessEffectBy)).WillOnce(Return(6));
    compiler.setBlock(block8);
    LooksBlocks::compileChangeEffectBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeGhostEffectBy)).WillOnce(Return(7));
    compiler.setBlock(block9);
    LooksBlocks::compileChangeEffectBy(&compiler);

    compiler.end();

    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(1), effect2.get());

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              2,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              3,
              vm::OP_CONST,
              4,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              5,
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              2,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              8,
              vm::OP_EXEC,
              4,
              vm::OP_CONST,
              9,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              10,
              vm::OP_EXEC,
              6,
              vm::OP_CONST,
              11,
              vm::OP_EXEC,
              7,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 12.5, 0, 1, -78.15, 154.152, -124.054, 45.858, -0.15, 0.84, 40.87, 50.04 }));

    ScratchConfiguration::removeGraphicsEffect("custom1");
    ScratchConfiguration::removeGraphicsEffect("custom2");
}

static void initEffects()
{
    // Create and register fake graphics effects
    auto colorEffect = std::make_shared<GraphicsEffectMock>();
    auto fisheyeEffect = std::make_shared<GraphicsEffectMock>();
    auto whirlEffect = std::make_shared<GraphicsEffectMock>();
    auto pixelateEffect = std::make_shared<GraphicsEffectMock>();
    auto mosaicEffect = std::make_shared<GraphicsEffectMock>();
    auto brightnessEffect = std::make_shared<GraphicsEffectMock>();
    auto ghostEffect = std::make_shared<GraphicsEffectMock>();

    EXPECT_CALL(*colorEffect, name()).WillOnce(Return("color"));
    ScratchConfiguration::registerGraphicsEffect(colorEffect);

    EXPECT_CALL(*fisheyeEffect, name()).WillOnce(Return("fisheye"));
    ScratchConfiguration::registerGraphicsEffect(fisheyeEffect);

    EXPECT_CALL(*whirlEffect, name()).WillOnce(Return("whirl"));
    ScratchConfiguration::registerGraphicsEffect(whirlEffect);

    EXPECT_CALL(*pixelateEffect, name()).WillOnce(Return("pixelate"));
    ScratchConfiguration::registerGraphicsEffect(pixelateEffect);

    EXPECT_CALL(*mosaicEffect, name()).WillOnce(Return("mosaic"));
    ScratchConfiguration::registerGraphicsEffect(mosaicEffect);

    EXPECT_CALL(*brightnessEffect, name()).WillOnce(Return("brightness"));
    ScratchConfiguration::registerGraphicsEffect(brightnessEffect);

    EXPECT_CALL(*ghostEffect, name()).WillOnce(Return("ghost"));
    ScratchConfiguration::registerGraphicsEffect(ghostEffect);

    LooksBlocks::m_colorEffect = ScratchConfiguration::getGraphicsEffect("color");
    LooksBlocks::m_fisheyeEffect = ScratchConfiguration::getGraphicsEffect("fisheye");
    LooksBlocks::m_whirlEffect = ScratchConfiguration::getGraphicsEffect("whirl");
    LooksBlocks::m_pixelateEffect = ScratchConfiguration::getGraphicsEffect("pixelate");
    LooksBlocks::m_mosaicEffect = ScratchConfiguration::getGraphicsEffect("mosaic");
    LooksBlocks::m_brightnessEffect = ScratchConfiguration::getGraphicsEffect("brightness");
    LooksBlocks::m_ghostEffect = ScratchConfiguration::getGraphicsEffect("ghost");
}

static void removeEffects()
{
    ScratchConfiguration::removeGraphicsEffect("color");
    ScratchConfiguration::removeGraphicsEffect("fisheye");
    ScratchConfiguration::removeGraphicsEffect("whirl");
    ScratchConfiguration::removeGraphicsEffect("pixelate");
    ScratchConfiguration::removeGraphicsEffect("mosaic");
    ScratchConfiguration::removeGraphicsEffect("brightness");
    ScratchConfiguration::removeGraphicsEffect("ghost");
}

TEST_F(LooksBlocksTest, ChangeEffectByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 5, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 6, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 7, vm::OP_HALT };
    static BlockFunc functions[] = {
        &LooksBlocks::changeEffectBy,         &LooksBlocks::changeColorEffectBy,  &LooksBlocks::changeFisheyeEffectBy,    &LooksBlocks::changeWhirlEffectBy,
        &LooksBlocks::changePixelateEffectBy, &LooksBlocks::changeMosaicEffectBy, &LooksBlocks::changeBrightnessEffectBy, &LooksBlocks::changeGhostEffectBy
    };
    static Value constValues[] = { 0, 55.15, 1, -40.54, 1.2, 2.3, -3.4, -4.5, 0.5, -8.54, 0.01 };

    GraphicsEffectMock effect1, effect2;

    Target target;
    target.setGraphicsEffectValue(&effect1, 12.5);
    target.setGraphicsEffectValue(&effect2, -100.48);

    // custom1
    VirtualMachine vm(&target, nullptr, nullptr);
    LooksBlocks::m_customGraphicsEffects.clear();
    LooksBlocks::m_customGraphicsEffects.push_back(&effect1);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 67.65);

    // custom2
    LooksBlocks::m_customGraphicsEffects.push_back(&effect2);
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), -141.02);

    // Initialize graphics effects
    initEffects();
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("color"), 12.4);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("fisheye"), -8.45);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("whirl"), 50.15);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("pixelate"), -45.21);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("mosaic"), 0.2);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("brightness"), -0.01);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("ghost"), 45.78);

    // color
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("color")), 13.6);

    // fisheye
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("fisheye")) * 100) / 100, -6.15);

    // whirl
    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("whirl")), 46.75);

    // pixelate
    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("pixelate")), -49.71);

    // mosaic
    vm.reset();
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("mosaic")), 0.7);

    // brightness
    vm.reset();
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("brightness")) * 100) / 100, -8.55);

    // ghost
    vm.reset();
    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("ghost")), 45.79);

    removeEffects();
}

TEST_F(LooksBlocksTest, SetEffectTo)
{
    Compiler compiler(&m_engineMock);

    // set [custom1] effect to 12.5
    auto block1 = std::make_shared<Block>("a", "looks_seteffectto");
    addDropdownField(block1, "EFFECT", LooksBlocks::EFFECT, "custom1", static_cast<LooksBlocks::FieldValues>(-1));
    addValueInput(block1, "VALUE", LooksBlocks::VALUE, 12.5);

    // set [custom2] effect to -78.15
    auto block2 = std::make_shared<Block>("b", "looks_seteffectto");
    addDropdownField(block2, "EFFECT", LooksBlocks::EFFECT, "custom2", static_cast<LooksBlocks::FieldValues>(-1));
    addValueInput(block2, "VALUE", LooksBlocks::VALUE, -78.15);

    // set [color] effect to 154.152
    auto block3 = std::make_shared<Block>("c", "looks_seteffectto");
    addDropdownField(block3, "EFFECT", LooksBlocks::EFFECT, "color", LooksBlocks::ColorEffect);
    addValueInput(block3, "VALUE", LooksBlocks::VALUE, 154.152);

    // set [fisheye] effect to -124.054
    auto block4 = std::make_shared<Block>("d", "looks_seteffectto");
    addDropdownField(block4, "EFFECT", LooksBlocks::EFFECT, "fisheye", LooksBlocks::FisheyeEffect);
    addValueInput(block4, "VALUE", LooksBlocks::VALUE, -124.054);

    // set [whirl] effect to 45.858
    auto block5 = std::make_shared<Block>("e", "looks_seteffectto");
    addDropdownField(block5, "EFFECT", LooksBlocks::EFFECT, "whirl", LooksBlocks::WhirlEffect);
    addValueInput(block5, "VALUE", LooksBlocks::VALUE, 45.858);

    // set [pixelate] effect to -0.15
    auto block6 = std::make_shared<Block>("f", "looks_seteffectto");
    addDropdownField(block6, "EFFECT", LooksBlocks::EFFECT, "pixelate", LooksBlocks::PixelateEffect);
    addValueInput(block6, "VALUE", LooksBlocks::VALUE, -0.15);

    // set [mosaic] effect to 0.84
    auto block7 = std::make_shared<Block>("g", "looks_seteffectto");
    addDropdownField(block7, "EFFECT", LooksBlocks::EFFECT, "mosaic", LooksBlocks::MosaicEffect);
    addValueInput(block7, "VALUE", LooksBlocks::VALUE, 0.84);

    // set [brightness] effect to 40.87
    auto block8 = std::make_shared<Block>("h", "looks_seteffectto");
    addDropdownField(block8, "EFFECT", LooksBlocks::EFFECT, "brightness", LooksBlocks::BrightnessEffect);
    addValueInput(block8, "VALUE", LooksBlocks::VALUE, 40.87);

    // set [ghost] effect to 50.04
    auto block9 = std::make_shared<Block>("i", "looks_seteffectto");
    addDropdownField(block9, "EFFECT", LooksBlocks::EFFECT, "ghost", LooksBlocks::GhostEffect);
    addValueInput(block9, "VALUE", LooksBlocks::VALUE, 50.04);

    auto effect1 = std::make_shared<GraphicsEffectMock>();
    auto effect2 = std::make_shared<GraphicsEffectMock>();
    EXPECT_CALL(*effect1, name()).WillOnce(Return("custom1"));
    ScratchConfiguration::registerGraphicsEffect(effect1);
    EXPECT_CALL(*effect2, name()).WillOnce(Return("custom2"));
    ScratchConfiguration::registerGraphicsEffect(effect2);

    compiler.init();
    LooksBlocks::m_customGraphicsEffects.clear();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setEffectTo)).Times(3).WillRepeatedly(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileSetEffectTo(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());

    compiler.setBlock(block1);
    LooksBlocks::compileSetEffectTo(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());

    compiler.setBlock(block2);
    LooksBlocks::compileSetEffectTo(&compiler);
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(1), effect2.get());

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setColorEffectTo)).WillOnce(Return(1));
    compiler.setBlock(block3);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setFisheyeEffectTo)).WillOnce(Return(2));
    compiler.setBlock(block4);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setWhirlEffectTo)).WillOnce(Return(3));
    compiler.setBlock(block5);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setPixelateEffectTo)).WillOnce(Return(4));
    compiler.setBlock(block6);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setMosaicEffectTo)).WillOnce(Return(5));
    compiler.setBlock(block7);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setBrightnessEffectTo)).WillOnce(Return(6));
    compiler.setBlock(block8);
    LooksBlocks::compileSetEffectTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setGhostEffectTo)).WillOnce(Return(7));
    compiler.setBlock(block9);
    LooksBlocks::compileSetEffectTo(&compiler);

    compiler.end();

    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(0), effect1.get());
    ASSERT_EQ(LooksBlocks::m_customGraphicsEffects.at(1), effect2.get());

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              2,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              3,
              vm::OP_CONST,
              4,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              5,
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              2,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              8,
              vm::OP_EXEC,
              4,
              vm::OP_CONST,
              9,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              10,
              vm::OP_EXEC,
              6,
              vm::OP_CONST,
              11,
              vm::OP_EXEC,
              7,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 12.5, 0, 1, -78.15, 154.152, -124.054, 45.858, -0.15, 0.84, 40.87, 50.04 }));

    ScratchConfiguration::removeGraphicsEffect("custom1");
    ScratchConfiguration::removeGraphicsEffect("custom2");
}

TEST_F(LooksBlocksTest, SetEffectToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 5, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 6, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 7, vm::OP_HALT };
    static BlockFunc functions[] = {
        &LooksBlocks::setEffectTo,         &LooksBlocks::setColorEffectTo,  &LooksBlocks::setFisheyeEffectTo,    &LooksBlocks::setWhirlEffectTo,
        &LooksBlocks::setPixelateEffectTo, &LooksBlocks::setMosaicEffectTo, &LooksBlocks::setBrightnessEffectTo, &LooksBlocks::setGhostEffectTo
    };
    static Value constValues[] = { 0, 55.15, 1, -40.54, 1.2, 2.3, -3.4, -4.5, 0.5, -8.54, 0.01 };

    GraphicsEffectMock effect1, effect2;

    Target target;
    target.setGraphicsEffectValue(&effect1, 12.5);
    target.setGraphicsEffectValue(&effect2, -100.48);

    // custom1
    VirtualMachine vm(&target, nullptr, nullptr);
    LooksBlocks::m_customGraphicsEffects.clear();
    LooksBlocks::m_customGraphicsEffects.push_back(&effect1);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 55.15);

    // custom2
    LooksBlocks::m_customGraphicsEffects.push_back(&effect2);
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), -40.54);

    // Initialize graphics effects
    initEffects();
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("color"), 12.4);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("fisheye"), -8.45);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("whirl"), 50.15);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("pixelate"), -45.21);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("mosaic"), 0.2);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("brightness"), -0.01);
    target.setGraphicsEffectValue(ScratchConfiguration::getGraphicsEffect("ghost"), 45.78);

    // color
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("color")), 1.2);

    // fisheye
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("fisheye")), 2.3);

    // whirl
    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("whirl")), -3.4);

    // pixelate
    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("pixelate")), -4.5);

    // mosaic
    vm.reset();
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("mosaic")), 0.5);

    // brightness
    vm.reset();
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("brightness")), -8.54);

    // ghost
    vm.reset();
    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(ScratchConfiguration::getGraphicsEffect("ghost")), 0.01);

    removeEffects();
}

TEST_F(LooksBlocksTest, ClearGraphicEffects)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_cleargraphiceffects");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::clearGraphicEffects)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileClearGraphicEffects(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, ClearGraphicEffectsImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::clearGraphicEffects };

    Target target;
    GraphicsEffectMock effect1, effect2;
    target.setGraphicsEffectValue(&effect1, 48.21);
    target.setGraphicsEffectValue(&effect2, -107.08);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), 0);
}

TEST_F(LooksBlocksTest, ChangeSizeBy)
{
    Compiler compiler(&m_engineMock);

    // change size by (10.05)
    auto block = std::make_shared<Block>("a", "looks_changesizeby");
    addValueInput(block, "CHANGE", LooksBlocks::CHANGE, 10.05);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeSizeBy)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileChangeSizeBy(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 10.05);
}

TEST_F(LooksBlocksTest, ChangeSizeByImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::changeSizeBy };
    static Value constValues[] = { 10.05 };

    Sprite sprite;
    sprite.setSize(1.308);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.size(), 11.358);
}

TEST_F(LooksBlocksTest, SetSizeTo)
{
    Compiler compiler(&m_engineMock);

    // set size to (87.654)
    auto block = std::make_shared<Block>("a", "looks_setsizeto");
    addValueInput(block, "SIZE", LooksBlocks::SIZE, 87.654);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setSizeTo)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSetSizeTo(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 87.654);
}

TEST_F(LooksBlocksTest, SetSizeToImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::setSizeTo };
    static Value constValues[] = { 87.654 };

    Sprite sprite;
    sprite.setSize(5.684);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.size(), 87.654);
}

TEST_F(LooksBlocksTest, Size)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_size");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::size)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSize(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, SizeMonitorName)
{
    ASSERT_EQ(LooksBlocks::sizeMonitorName(nullptr), "size");
}

TEST_F(LooksBlocksTest, SizeImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::size };

    Sprite sprite;
    sprite.setSize(-51.0684);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -51.0684);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo)
{
    Target target;
    Compiler compiler(&m_engineMock, &target);

    // switch costume to (costume2)
    auto block1 = std::make_shared<Block>("a", "looks_switchcostumeto");
    addDropdownInput(block1, "COSTUME", LooksBlocks::COSTUME, "costume2");

    // switch costume to (null block)
    auto block2 = std::make_shared<Block>("b", "looks_switchcostumeto");
    addDropdownInput(block2, "COSTUME", LooksBlocks::COSTUME, "", createNullBlock("c"));

    compiler.init();

    compiler.setBlock(block1);
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeTo)).WillOnce(Return(0));
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeTo)).WillOnce(Return(0));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "costume2" }));
}

TEST_F(LooksBlocksTest, SwitchCostumeToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 11, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode13[] = { vm::OP_START, vm::OP_CONST, 12, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode14[] = { vm::OP_START, vm::OP_CONST, 13, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchCostumeTo };
    static Value constValues[] = { "costume2", 0, 1, 2, 3, "2", "3", SpecialValue::NaN, SpecialValue::Infinity, SpecialValue::NegativeInfinity, "", "   ", "next costume", "previous costume" };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.setCostumeIndex(0);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    // "costume2"
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    // 0
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    target.setCostumeIndex(0);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    // 1
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    // 2
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    // 3
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    target.setCostumeIndex(1);

    // 2
    target.addCostume(std::make_shared<Costume>("2", "c3", "svg"));
    target.addCostume(std::make_shared<Costume>("test", "c4", "svg"));
    target.setCostumeIndex(0);

    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    // "2"
    target.setCostumeIndex(0);

    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    // "3"
    target.setCostumeIndex(0);

    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    // NaN
    target.setCostumeIndex(2);

    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    // Infinity
    target.setCostumeIndex(2);

    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    // -Infinity
    target.setCostumeIndex(2);

    vm.setBytecode(bytecode10);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    // ""
    target.setCostumeIndex(2);

    vm.setBytecode(bytecode11);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    // "   "
    target.setCostumeIndex(2);

    vm.setBytecode(bytecode12);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    // "next costume"
    vm.setBytecode(bytecode13);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 3);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    target.addCostume(std::make_shared<Costume>("next costume", "c5", "svg"));

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 4);

    // "previous costume"
    vm.setBytecode(bytecode14);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 3);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    target.setCostumeIndex(0);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 4);

    target.addCostume(std::make_shared<Costume>("previous costume", "c6", "svg"));

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 5);
}

TEST_F(LooksBlocksTest, NextCostume)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_nextcostume");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextCostume)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileNextCostume(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, NextCostumeImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::nextCostume };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.addCostume(std::make_shared<Costume>("costume3", "c3", "svg"));
    target.setCostumeIndex(0);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, PreviousCostume)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::previousCostume };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.addCostume(std::make_shared<Costume>("costume3", "c3", "svg"));
    target.setCostumeIndex(2);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 0);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo)
{
    Target target;
    Compiler compiler(&m_engineMock, &target);

    // switch backdrop to (backdrop2)
    auto block1 = std::make_shared<Block>("a", "looks_switchbackdropto");
    addDropdownInput(block1, "BACKDROP", LooksBlocks::BACKDROP, "backdrop2");

    // switch backdrop to (null block)
    auto block2 = std::make_shared<Block>("b", "looks_switchbackdropto");
    addDropdownInput(block2, "BACKDROP", LooksBlocks::BACKDROP, "", createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropTo)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropTo)).WillOnce(Return(0));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "backdrop2" }));
}

TEST_F(LooksBlocksTest, SwitchBackdropToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 11, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode13[] = { vm::OP_START, vm::OP_CONST, 12, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode14[] = { vm::OP_START, vm::OP_CONST, 13, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode15[] = { vm::OP_START, vm::OP_CONST, 14, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchBackdropTo };
    static Value constValues[] = {
        "backdrop2", 0, 1, 2, 3, "2", "3", SpecialValue::NaN, SpecialValue::Infinity, SpecialValue::NegativeInfinity, "", "   ", "next backdrop", "previous backdrop", "random backdrop"
    };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.setCostumeIndex(0);
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(&stage));

    Thread thread(&target, &m_engineMock, nullptr);
    VirtualMachine *vm = thread.vm();
    vm->setFunctions(functions);
    vm->setConstValues(constValues);

    // "backdrop2"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->setBytecode(bytecode1);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    // 0
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->setBytecode(bytecode2);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    stage.setCostumeIndex(0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    // 1
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    vm->setBytecode(bytecode3);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // 2
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    stage.addCostume(std::make_shared<Costume>("2", "b3", "svg"));
    stage.addCostume(std::make_shared<Costume>("test", "b4", "svg"));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode4);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    // "2"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode6);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "3"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode7);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // NaN
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode8);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // Infinity
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode9);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // -Infinity
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode10);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // ""
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode11);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "   "
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode12);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "next backdrop"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, false));
    vm->setBytecode(bytecode13);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(4)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 4);

    // "previous backdrop"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, false));
    vm->setBytecode(bytecode14);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    stage.setCostumeIndex(0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(4)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 4);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(5)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 5);

    // random backdrop
    RandomGeneratorMock rng;
    LooksBlocks::rng = &rng;

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, false));
    EXPECT_CALL(rng, randint(0, 5)).WillOnce(Return(3));
    stage.setCostumeIndex(0);
    vm->setBytecode(bytecode15);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(5)->broadcast(), &thread, false));
    EXPECT_CALL(rng, randint(0, 5)).WillOnce(Return(5));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 5);

    stage.addCostume(std::make_shared<Costume>("random backdrop", "b7", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(6)->broadcast(), &thread, false));
    EXPECT_CALL(rng, randint).Times(0);
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 6);

    LooksBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(LooksBlocksTest, SwitchBackdropToAndWait)
{
    Target target;
    Compiler compiler(&m_engineMock, &target);

    // switch backdrop to (backdrop2) and wait
    auto block1 = std::make_shared<Block>("a", "looks_switchbackdroptoandwait");
    addDropdownInput(block1, "BACKDROP", LooksBlocks::BACKDROP, "backdrop2");

    // switch backdrop to (null block) and wait
    auto block2 = std::make_shared<Block>("b", "looks_switchbackdroptoandwait");
    addDropdownInput(block2, "BACKDROP", LooksBlocks::BACKDROP, "", createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToAndWait)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToAndWait)).WillOnce(Return(0));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "backdrop2" }));
}

TEST_F(LooksBlocksTest, SwitchBackdropToAndWaitImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 11, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode13[] = { vm::OP_START, vm::OP_CONST, 12, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode14[] = { vm::OP_START, vm::OP_CONST, 13, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode15[] = { vm::OP_START, vm::OP_CONST, 14, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchBackdropToAndWait };
    static Value constValues[] = {
        "backdrop2", 0, 1, 2, 3, "2", "3", SpecialValue::NaN, SpecialValue::Infinity, SpecialValue::NegativeInfinity, "", "   ", "next backdrop", "previous backdrop", "random backdrop"
    };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.setCostumeIndex(0);
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(&stage));

    Thread thread(&target, &m_engineMock, nullptr);
    VirtualMachine *vm = thread.vm();
    vm->setFunctions(functions);
    vm->setConstValues(constValues);

    // "backdrop2"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, true));
    vm->setBytecode(bytecode1);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);
    ASSERT_FALSE(vm->atEnd());

    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_TRUE(vm->atEnd());

    // 0
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, true));
    vm->setBytecode(bytecode2);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);
    ASSERT_FALSE(vm->atEnd());

    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_TRUE(vm->atEnd());

    stage.setCostumeIndex(0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);
    ASSERT_TRUE(vm->atEnd());

    // 1
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, true));
    vm->setBytecode(bytecode3);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);
    ASSERT_TRUE(vm->atEnd());

    // 2
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, true));
    stage.addCostume(std::make_shared<Costume>("2", "b3", "svg"));
    stage.addCostume(std::make_shared<Costume>("test", "b4", "svg"));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode4);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    // "2"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, true));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode6);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "3"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, true));
    stage.setCostumeIndex(0);

    vm->setBytecode(bytecode7);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // NaN
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, true));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode8);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // Infinity
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, true));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode9);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // -Infinity
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, true));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode10);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    // ""
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, true));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode11);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "   "
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, true));
    stage.setCostumeIndex(2);

    vm->setBytecode(bytecode12);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    // "next backdrop"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, true));
    vm->resolvePromise();
    vm->setBytecode(bytecode13);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);
    ASSERT_FALSE(vm->atEnd());

    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_TRUE(vm->atEnd());

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(4)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 4);

    // "previous backdrop"
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, true));
    vm->resolvePromise();
    vm->setBytecode(bytecode14);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);
    ASSERT_FALSE(vm->atEnd());

    stage.setCostumeIndex(2); // checkBackdropScripts() should still check index 3 even after changing the backdrop
    vm->run();
    vm->resolvePromise();
    vm->run();
    stage.setCostumeIndex(3);

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_TRUE(vm->atEnd());

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    stage.setCostumeIndex(0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(4)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 4);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(5)->broadcast(), &thread, true));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 5);

    // random backdrop
    RandomGeneratorMock rng;
    LooksBlocks::rng = &rng;

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(3)->broadcast(), &thread, true));
    EXPECT_CALL(rng, randint(0, 5)).WillOnce(Return(3));
    stage.setCostumeIndex(0);
    vm->resolvePromise();
    vm->setBytecode(bytecode15);
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 3);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(5)->broadcast(), &thread, true));
    EXPECT_CALL(rng, randint(0, 5)).WillOnce(Return(5));
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 5);

    stage.addCostume(std::make_shared<Costume>("random backdrop", "b7", "svg"));

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(6)->broadcast(), &thread, true));
    EXPECT_CALL(rng, randint).Times(0);
    vm->reset();
    vm->run();
    vm->resolvePromise();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 6);

    LooksBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(LooksBlocksTest, NextBackdrop)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_nextbackdrop");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextBackdrop)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileNextBackdrop(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, NextBackdropImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::nextBackdrop };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));
    stage.setCostumeIndex(0);
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(&stage));

    Thread thread(&target, &m_engineMock, nullptr);
    VirtualMachine *vm = thread.vm();
    vm->setBytecode(bytecode);
    vm->setFunctions(functions);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);
}

TEST_F(LooksBlocksTest, PreviousBackdrop)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::previousBackdrop };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));
    stage.setCostumeIndex(2);
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(&stage));

    Thread thread(&target, &m_engineMock, nullptr);
    VirtualMachine *vm = thread.vm();
    vm->setBytecode(bytecode);
    vm->setFunctions(functions);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(0)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 0);

    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);
}

TEST_F(LooksBlocksTest, RandomBackdrop)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::randomBackdrop };

    Target target;

    Stage stage;
    EXPECT_CALL(m_engineMock, stage()).WillRepeatedly(Return(&stage));

    Thread thread(&target, &m_engineMock, nullptr);
    VirtualMachine *vm = thread.vm();
    vm->setBytecode(bytecode);
    vm->setFunctions(functions);

    RandomGeneratorMock rng;
    LooksBlocks::rng = &rng;

    EXPECT_CALL(rng, randint).Times(0);
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);

    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));

    EXPECT_CALL(rng, randint(0, 2)).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(1)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 1);

    stage.addCostume(std::make_shared<Costume>("backdrop4", "b4", "svg"));

    EXPECT_CALL(rng, randint(0, 3)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, startBackdropScripts(stage.costumeAt(2)->broadcast(), &thread, false));
    vm->reset();
    vm->run();

    ASSERT_EQ(vm->registerCount(), 0);
    ASSERT_EQ(stage.costumeIndex(), 2);

    LooksBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(LooksBlocksTest, GoToFrontBack)
{
    Compiler compiler(&m_engineMock);

    // go to [front] layer
    auto block1 = std::make_shared<Block>("a", "looks_gotofrontback");
    addDropdownField(block1, "FRONT_BACK", LooksBlocks::FRONT_BACK, "front", LooksBlocks::Front);

    // go to [back] layer
    auto block2 = std::make_shared<Block>("b", "looks_gotofrontback");
    addDropdownField(block2, "FRONT_BACK", LooksBlocks::FRONT_BACK, "back", LooksBlocks::Back);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::goToFront)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileGoToFrontBack(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::goToBack)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileGoToFrontBack(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, GoToFrontBackImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::goToFront, &LooksBlocks::goToBack };

    Sprite sprite;

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, moveSpriteToFront(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(m_engineMock, moveSpriteToBack(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayers)
{
    Compiler compiler(&m_engineMock);

    // go [forward] (5) layers
    auto block1 = std::make_shared<Block>("a", "looks_goforwardbackwardlayers");
    addDropdownField(block1, "FORWARD_BACKWARD", LooksBlocks::FORWARD_BACKWARD, "forward", LooksBlocks::Forward);
    addValueInput(block1, "NUM", LooksBlocks::NUM, 5);

    // go [backward] (3) layers
    auto block2 = std::make_shared<Block>("b", "looks_goforwardbackwardlayers");
    addDropdownField(block2, "FORWARD_BACKWARD", LooksBlocks::FORWARD_BACKWARD, "backward", LooksBlocks::Backward);
    addValueInput(block2, "NUM", LooksBlocks::NUM, 3);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::goForwardLayers)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileGoForwardBackwardLayers(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::goBackwardLayers)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileGoForwardBackwardLayers(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 5, 3 }));
}

TEST_F(LooksBlocksTest, GoForwardBackwardLayersImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::goForwardLayers, &LooksBlocks::goBackwardLayers };
    static Value constValues[] = { 5, 3 };

    Sprite sprite;

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, moveSpriteForwardLayers(&sprite, 5));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(m_engineMock, moveSpriteBackwardLayers(&sprite, 3));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(LooksBlocksTest, CostumeNumberName)
{
    Compiler compiler(&m_engineMock);

    // costume [number]
    auto block1 = std::make_shared<Block>("a", "looks_costumenumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // costume [name]
    auto block2 = std::make_shared<Block>("b", "looks_costumenumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::costumeNumber)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileCostumeNumberName(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::costumeName)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileCostumeNumberName(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, CostumeNumberNameMonitorName)
{
    // costume [number]
    auto block1 = std::make_shared<Block>("a", "looks_costumenumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // costume [name]
    auto block2 = std::make_shared<Block>("b", "looks_costumenumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    ASSERT_EQ(LooksBlocks::costumeNumberNameMonitorName(block1.get()), "costume number");
    ASSERT_EQ(LooksBlocks::costumeNumberNameMonitorName(block2.get()), "costume name");
}

TEST_F(LooksBlocksTest, CostumeNumberNameImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::costumeNumber, &LooksBlocks::costumeName };

    auto c1 = std::make_shared<Costume>("costume1", "c1", "svg");
    auto c2 = std::make_shared<Costume>("costume2", "c2", "svg");
    auto c3 = std::make_shared<Costume>("costume3", "c3", "svg");

    Target target;
    target.addCostume(c1);
    target.addCostume(c2);
    target.addCostume(c3);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);

    target.setCostumeIndex(1);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume2");

    target.setCostumeIndex(2);

    vm.reset();
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 3);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume3");
}

TEST_F(LooksBlocksTest, BackdropNumberName)
{
    Compiler compiler(&m_engineMock);

    // backdrop [number]
    auto block1 = std::make_shared<Block>("a", "looks_backdropnumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // backdrop [name]
    auto block2 = std::make_shared<Block>("b", "looks_backdropnumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::backdropNumber)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileBackdropNumberName(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::backdropName)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileBackdropNumberName(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, BackdropNumberNameMonitorName)
{
    // backdrop [number]
    auto block1 = std::make_shared<Block>("a", "looks_backdropnumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // backdrop [name]
    auto block2 = std::make_shared<Block>("b", "looks_backdropnumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    ASSERT_EQ(LooksBlocks::backdropNumberNameMonitorName(block1.get()), "backdrop number");
    ASSERT_EQ(LooksBlocks::backdropNumberNameMonitorName(block2.get()), "backdrop name");
}

TEST_F(LooksBlocksTest, BackdropNumberNameImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::backdropNumber, &LooksBlocks::backdropName };

    auto b1 = std::make_shared<Costume>("backdrop1", "b1", "svg");
    auto b2 = std::make_shared<Costume>("backdrop2", "b2", "svg");
    auto b3 = std::make_shared<Costume>("backdrop3", "b3", "svg");

    Target target;

    Stage stage;
    stage.addCostume(b1);
    stage.addCostume(b2);
    stage.addCostume(b3);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    stage.setCostumeIndex(1);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop2");

    stage.setCostumeIndex(2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 3);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop3");
}
