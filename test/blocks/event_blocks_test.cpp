#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>
#include <audioinputmock.h>
#include <audioloudnessmock.h>
#include <timermock.h>

#include "../common.h"
#include "blocks/eventblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class EventBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<EventBlocks>();
            m_section->registerBlocks(&m_engine);

            m_broadcast = std::make_shared<Broadcast>("", "test");
            m_engine.setBroadcasts({ m_broadcast });
        }

        // For any event block
        std::shared_ptr<Block> createEventBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        void addBroadcastInput(std::shared_ptr<Block> block, const std::string &name, EventBlocks::Inputs id, std::shared_ptr<Broadcast> broadcast) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(broadcast->name());
            input->primaryValue()->setValuePtr(broadcast);
            input->primaryValue()->setType(InputValue::Type::Broadcast);
            input->setInputId(id);
            block->addInput(input);
        }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, EventBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, EventBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        void addValueField(std::shared_ptr<Block> block, const std::string &name, EventBlocks::Fields id, const std::string &value, int valueId = -1) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        void addBroadcastField(std::shared_ptr<Block> block, const std::string &name, EventBlocks::Fields id, std::shared_ptr<Broadcast> broadcast) const
        {
            auto field = std::make_shared<Field>(name, Value(), broadcast);
            field->setFieldId(id);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
        std::shared_ptr<Broadcast> m_broadcast;
};

TEST_F(EventBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Events");
}

TEST_F(EventBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(EventBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenflagclicked", &EventBlocks::compileWhenFlagClicked));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenthisspriteclicked", &EventBlocks::compileWhenThisSpriteClicked));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenstageclicked", &EventBlocks::compileWhenStageClicked));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_broadcast", &EventBlocks::compileBroadcast));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_broadcastandwait", &EventBlocks::compileBroadcastAndWait));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenbroadcastreceived", &EventBlocks::compileWhenBroadcastReceived));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenbackdropswitchesto", &EventBlocks::compileWhenBackdropSwitchesTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whengreaterthan", &EventBlocks::compileWhenGreaterThan));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "event_whenkeypressed", &EventBlocks::compileWhenKeyPressed));

    // Hat predicates
    EXPECT_CALL(m_engineMock, addHatPredicateCompileFunction(m_section.get(), "event_whengreaterthan", &EventBlocks::compileWhenGreaterThanPredicate));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "BROADCAST_INPUT", EventBlocks::BROADCAST_INPUT));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VALUE", EventBlocks::VALUE));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "BROADCAST_OPTION", EventBlocks::BROADCAST_OPTION));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "BACKDROP", EventBlocks::BACKDROP));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "WHENGREATERTHANMENU", EventBlocks::WHENGREATERTHANMENU));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "KEY_OPTION", EventBlocks::KEY_OPTION));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "LOUDNESS", EventBlocks::Loudness));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "TIMER", EventBlocks::Timer));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(EventBlocksTest, WhenFlagClicked)
{
    Compiler compiler(&m_engineMock);

    auto block = createEventBlock("a", "event_whenflagclicked");

    compiler.init();

    EXPECT_CALL(m_engineMock, addGreenFlagScript(block));
    compiler.setBlock(block);
    EventBlocks::compileWhenFlagClicked(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenThisSpriteClicked)
{
    Compiler compiler(&m_engineMock);

    auto block = createEventBlock("a", "event_whenthisspriteclicked");

    compiler.init();

    EXPECT_CALL(m_engineMock, addTargetClickScript(block));
    compiler.setBlock(block);
    EventBlocks::compileWhenThisSpriteClicked(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenStageClicked)
{
    Compiler compiler(&m_engineMock);

    auto block = createEventBlock("a", "event_whenstageclicked");

    compiler.init();

    EXPECT_CALL(m_engineMock, addTargetClickScript(block));
    compiler.setBlock(block);
    EventBlocks::compileWhenStageClicked(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, Broadcast)
{
    Compiler compiler(&m_engineMock);

    // broadcast "test"
    auto block1 = createEventBlock("a", "event_broadcast");
    addBroadcastInput(block1, "BROADCAST_INPUT", EventBlocks::BROADCAST_INPUT, m_broadcast);

    // broadcast <not <>>
    auto block2 = createEventBlock("b", "event_broadcast");
    auto notBlock = std::make_shared<Block>("c", "operator_not");
    notBlock->setCompileFunction(&OperatorBlocks::compileNot);
    addObscuredInput(block2, "BROADCAST_INPUT", EventBlocks::BROADCAST_INPUT, notBlock);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::broadcastByIndex)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::broadcast)).WillOnce(Return(1));

    compiler.init();
    compiler.setBlock(block1);
    EventBlocks::compileBroadcast(&compiler);
    compiler.setBlock(block2);
    EventBlocks::compileBroadcast(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_NOT, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 0);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, BroadcastImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &EventBlocks::broadcast, &EventBlocks::broadcastByIndex };
    static Value constValues[] = { "test", 2 };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, broadcast(1)).Times(1);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(m_engineMock, broadcast(2)).Times(1);

    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(EventBlocksTest, BroadcastAndWait)
{
    Compiler compiler(&m_engineMock);

    // broadcast "test" and wait
    auto block1 = createEventBlock("a", "event_broadcastandwait");
    addBroadcastInput(block1, "BROADCAST_INPUT", EventBlocks::BROADCAST_INPUT, m_broadcast);

    // broadcast <not <>> and wait
    auto block2 = createEventBlock("b", "event_broadcastandwait");
    auto notBlock = std::make_shared<Block>("c", "operator_not");
    notBlock->setCompileFunction(&OperatorBlocks::compileNot);
    addObscuredInput(block2, "BROADCAST_INPUT", EventBlocks::BROADCAST_INPUT, notBlock);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::broadcastByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::checkBroadcastByIndex)).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::broadcastAndWait)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::checkBroadcast)).WillOnce(Return(3));

    compiler.init();
    compiler.setBlock(block1);
    EventBlocks::compileBroadcastAndWait(&compiler);
    compiler.setBlock(block2);
    EventBlocks::compileBroadcastAndWait(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_NULL, vm::OP_NOT, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_NOT, vm::OP_EXEC, 3, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 0 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, BroadcastAndWaitImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 3, vm::OP_HALT };
    static BlockFunc functions[] = { &EventBlocks::broadcastAndWait, &EventBlocks::checkBroadcast, &EventBlocks::broadcastByIndexAndWait, &EventBlocks::checkBroadcastByIndex };
    static Value constValues[] = { "test", 3 };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, broadcast(1)).Times(1);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, broadcastRunning(2)).WillOnce(Return(true));

    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.atEnd(), false);

    EXPECT_CALL(m_engineMock, findBroadcast("test")).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, broadcastRunning(2)).WillOnce(Return(false));

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(vm.atEnd(), true);

    EXPECT_CALL(m_engineMock, broadcast(3)).Times(1);

    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(m_engineMock, broadcastRunning(3)).WillOnce(Return(true));

    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.atEnd(), false);

    EXPECT_CALL(m_engineMock, broadcastRunning(3)).WillOnce(Return(false));

    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(vm.atEnd(), true);
}

TEST_F(EventBlocksTest, WhenBroadcastReceived)
{
    Compiler compiler(&m_engineMock);

    // when I receive "test"
    auto block1 = createEventBlock("a", "event_whenbroadcastreceived");
    addBroadcastField(block1, "BROADCAST_OPTION", EventBlocks::BROADCAST_OPTION, m_broadcast);

    EXPECT_CALL(m_engineMock, addBroadcastScript(block1, EventBlocks::BROADCAST_OPTION, m_broadcast.get())).Times(1);

    compiler.init();
    compiler.setBlock(block1);
    EventBlocks::compileWhenBroadcastReceived(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenBackdropSwitchesTo)
{
    Compiler compiler(&m_engineMock);
    Stage stage;

    // when backdrop switches to "backdrop2"
    auto block1 = createEventBlock("a", "event_whenbackdropswitchesto");
    addValueField(block1, "BACKDROP", EventBlocks::BACKDROP, "backdrop2");

    auto backdrop = std::make_shared<Costume>("backdrop2", "a", "svg");
    stage.addCostume(backdrop);

    EXPECT_CALL(m_engineMock, addBackdropChangeScript(block1, EventBlocks::BACKDROP));

    compiler.init();
    compiler.setBlock(block1);
    EventBlocks::compileWhenBackdropSwitchesTo(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenGreaterThanPredicate)
{
    Compiler compiler(&m_engineMock);

    // when [loudness] > (8.9)
    auto block1 = createEventBlock("a", "event_whengreaterthan");
    addValueField(block1, "WHENGREATERTHANMENU", EventBlocks::WHENGREATERTHANMENU, "LOUDNESS", EventBlocks::Loudness);
    addValueInput(block1, "VALUE", EventBlocks::VALUE, 8.9);

    // when [timer] > (10.2)
    auto block2 = createEventBlock("b", "event_whengreaterthan");
    addValueField(block2, "WHENGREATERTHANMENU", EventBlocks::WHENGREATERTHANMENU, "TIMER", EventBlocks::Timer);
    addValueInput(block2, "VALUE", EventBlocks::VALUE, 10.2);

    // when [invalid value] > (0)
    auto block3 = createEventBlock("c", "event_whengreaterthan");
    addValueField(block3, "WHENGREATERTHANMENU", EventBlocks::WHENGREATERTHANMENU, "invalid value", -1);
    addValueInput(block3, "VALUE", EventBlocks::VALUE, 0);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenLoudnessGreaterThanPredicate)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenTimerGreaterThanPredicate)).Times(0);
    compiler.setBlock(block1);
    EventBlocks::compileWhenGreaterThanPredicate(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenLoudnessGreaterThanPredicate)).Times(0);
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenTimerGreaterThanPredicate)).WillOnce(Return(0));
    compiler.setBlock(block2);
    EventBlocks::compileWhenGreaterThanPredicate(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenLoudnessGreaterThanPredicate)).Times(0);
    EXPECT_CALL(m_engineMock, functionIndex(&EventBlocks::whenTimerGreaterThanPredicate)).Times(0);
    compiler.setBlock(block3);
    EventBlocks::compileWhenGreaterThanPredicate(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 8.9, 10.2 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenGreaterThanPredicateImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &EventBlocks::whenLoudnessGreaterThanPredicate, &EventBlocks::whenTimerGreaterThanPredicate };
    static Value constValues[] = { 22.7, 23, 23.5 };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    AudioInputMock audioInput;
    auto audioLoudness = std::make_shared<AudioLoudnessMock>();
    EventBlocks::audioInput = &audioInput;
    EXPECT_CALL(audioInput, audioLoudness()).Times(3).WillRepeatedly(Return(audioLoudness));
    EXPECT_CALL(*audioLoudness, getLoudness()).Times(3).WillRepeatedly(Return(23));

    vm.setBytecode(bytecode1);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_TRUE(vm.getInput(0, 1)->toBool());

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.getInput(0, 1)->toBool());

    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.getInput(0, 1)->toBool());

    EventBlocks::audioInput = nullptr;

    TimerMock timer;
    EXPECT_CALL(m_engineMock, timer()).Times(3).WillRepeatedly(Return(&timer));
    EXPECT_CALL(timer, value()).Times(3).WillRepeatedly(Return(23));

    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_TRUE(vm.getInput(0, 1)->toBool());

    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.getInput(0, 1)->toBool());

    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_FALSE(vm.getInput(0, 1)->toBool());
}

TEST_F(EventBlocksTest, WhenGreaterThan)
{
    Compiler compiler(&m_engineMock);

    // when [loudness] > (8.9)
    auto block = createEventBlock("a", "event_whengreaterthan");
    addValueField(block, "WHENGREATERTHANMENU", EventBlocks::WHENGREATERTHANMENU, "LOUDNESS", EventBlocks::Loudness);
    addValueInput(block, "VALUE", EventBlocks::VALUE, 8.9);

    EXPECT_CALL(m_engineMock, addWhenGreaterThanScript(block));
    compiler.init();
    compiler.setBlock(block);
    EventBlocks::compileWhenGreaterThan(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(EventBlocksTest, WhenKeyPressed)
{
    Compiler compiler(&m_engineMock);

    // when key "a" pressed
    auto block1 = createEventBlock("a", "event_whenkeypressed");
    addValueField(block1, "KEY_OPTION", EventBlocks::KEY_OPTION, "a");

    // when key "left arrow" pressed
    auto block2 = createEventBlock("b", "event_whenkeypressed");
    addValueField(block2, "KEY_OPTION", EventBlocks::KEY_OPTION, "left arrow");

    compiler.init();

    EXPECT_CALL(m_engineMock, addKeyPressScript(block1, EventBlocks::KEY_OPTION));
    compiler.setBlock(block1);
    EventBlocks::compileWhenKeyPressed(&compiler);

    EXPECT_CALL(m_engineMock, addKeyPressScript(block2, EventBlocks::KEY_OPTION));
    compiler.setBlock(block2);
    EventBlocks::compileWhenKeyPressed(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}
