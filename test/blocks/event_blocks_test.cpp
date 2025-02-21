#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <enginemock.h>
#include <targetmock.h>
#include <audioinputmock.h>
#include <audioloudnessmock.h>
#include <timermock.h>

#include "../common.h"
#include "blocks/eventblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class EventBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<EventBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(EventBlocksTest, WhenTouchingObject)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target, false);

    builder.addBlock("event_whentouchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "Sprite1");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addWhenTouchingObjectScript(block));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenTouchingObjectPredicate)
{
    auto target = std::make_shared<TargetMock>();
    target->setEngine(&m_engineMock);
    Sprite sprite;

    // "Sprite1"
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whentouchingobject");
        builder.addDropdownInput("TOUCHINGOBJECTMENU", "Sprite1");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("Sprite1")).WillOnce(Return(3));
        EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&sprite));
        EXPECT_CALL(*target, touchingClones).WillOnce(Return(false));
        ASSERT_FALSE(thread.runPredicate());

        EXPECT_CALL(m_engineMock, findTarget("Sprite1")).WillOnce(Return(3));
        EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&sprite));
        EXPECT_CALL(*target, touchingClones).WillOnce(Return(true));
        ASSERT_TRUE(thread.runPredicate());
    }

    // "_mouse_"
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whentouchingobject");
        builder.addDropdownInput("TOUCHINGOBJECTMENU", "_mouse_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(24.5));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-16.04));
        EXPECT_CALL(*target, touchingPoint(24.5, -16.04)).WillOnce(Return(false));
        ASSERT_FALSE(thread.runPredicate());

        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(24.5));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-16.04));
        EXPECT_CALL(*target, touchingPoint(24.5, -16.04)).WillOnce(Return(true));
        ASSERT_TRUE(thread.runPredicate());
    }

    // "_edge_"
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whentouchingobject");
        builder.addDropdownInput("TOUCHINGOBJECTMENU", "_edge_");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(10));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(10));
        EXPECT_CALL(*target, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
        ASSERT_FALSE(thread.runPredicate());

        EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(0));
        EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(0));
        EXPECT_CALL(*target, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
        ASSERT_TRUE(thread.runPredicate());
    }

    // "_stage_"
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whentouchingobject");
        builder.addDropdownInput("TOUCHINGOBJECTMENU", "");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("")).WillOnce(Return(-1));
        EXPECT_CALL(m_engineMock, targetAt(-1)).WillOnce(Return(nullptr));
        ASSERT_FALSE(thread.runPredicate());
    }

    // ""
    {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whentouchingobject");
        builder.addDropdownInput("TOUCHINGOBJECTMENU", "");
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findTarget("")).WillOnce(Return(-1));
        EXPECT_CALL(m_engineMock, targetAt(-1)).WillOnce(Return(nullptr));
        ASSERT_FALSE(thread.runPredicate());
    }
}

TEST_F(EventBlocksTest, WhenFlagClicked)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenflagclicked");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addGreenFlagScript(block));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenThisSpriteClicked)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenthisspriteclicked");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addTargetClickScript(block));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenStageClicked)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenstageclicked");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addTargetClickScript(block));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenBroadcastReceived)
{
    auto broadcast = std::make_shared<Broadcast>("", "");
    m_engine->setBroadcasts({ broadcast });

    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenbroadcastreceived");
    builder.addEntityField("BROADCAST_OPTION", broadcast);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addBroadcastScript(block, block->fieldAt(0).get(), broadcast.get()));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenBackdropSwitchesTo)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenbackdropswitchesto");
    builder.addDropdownField("BACKDROP", "backdrop2");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addBackdropChangeScript(block, block->fieldAt(0).get()));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenGreaterThan)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target, false);

    builder.addBlock("event_whengreaterthan");
    builder.addDropdownField("WHENGREATERTHANMENU", "LOUDNESS");
    builder.addValueInput("VALUE", 8.9);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addWhenGreaterThanScript(block));
    compiler.compile(block);
}

TEST_F(EventBlocksTest, WhenGreaterThanPredicate)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target, false);

    const std::vector<Value> values = { 22.7, 23, 23.5 };
    const std::vector<bool> retValues = { true, false, false };

    AudioInputMock audioInput;
    auto audioLoudness = std::make_shared<AudioLoudnessMock>();
    EventBlocks::audioInput = &audioInput;
    EXPECT_CALL(audioInput, audioLoudness()).Times(3).WillRepeatedly(Return(audioLoudness));
    EXPECT_CALL(*audioLoudness, getLoudness()).Times(3).WillRepeatedly(Return(23));

    // when [loudness] > x
    for (int i = 0; i < values.size(); i++) {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whengreaterthan");
        builder.addDropdownField("WHENGREATERTHANMENU", "LOUDNESS");
        builder.addValueInput("VALUE", values[i]);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        ASSERT_EQ(thread.runPredicate(), retValues[i]);
    }

    EventBlocks::audioInput = nullptr;

    TimerMock timer;
    EXPECT_CALL(m_engineMock, timer()).Times(3).WillRepeatedly(Return(&timer));
    EXPECT_CALL(timer, value()).Times(3).WillRepeatedly(Return(23));

    // when [timer] > x
    for (int i = 0; i < values.size(); i++) {
        ScriptBuilder builder(m_extension.get(), m_engine, target, false);
        builder.addBlock("event_whengreaterthan");
        builder.addDropdownField("WHENGREATERTHANMENU", "TIMER");
        builder.addValueInput("VALUE", values[i]);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block, Compiler::CodeType::HatPredicate);
        Script script(target.get(), block, &m_engineMock);
        script.setHatPredicateCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        ASSERT_EQ(thread.runPredicate(), retValues[i]);
    }
}

TEST_F(EventBlocksTest, Broadcast)
{
    auto broadcast = std::make_shared<Broadcast>("", "test");
    m_engine->setBroadcasts({ broadcast });

    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_broadcast");
    builder.addEntityInput("BROADCAST_INPUT", "test", InputValue::Type::Broadcast, broadcast);
    auto block1 = builder.currentBlock();

    builder.addBlock("event_broadcast");
    builder.addNullObscuredInput("BROADCAST_INPUT");
    auto block2 = builder.currentBlock();

    block1->setNext(nullptr);
    block2->setParent(nullptr);

    {
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block1);
        Script script(target.get(), block1, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findBroadcasts("test")).WillOnce(Return(std::vector<int>({ 1, 4 })));
        EXPECT_CALL(m_engineMock, broadcast(1, &thread, false));
        EXPECT_CALL(m_engineMock, broadcast(4, &thread, false));
        thread.run();
        ASSERT_TRUE(thread.isFinished());
        ASSERT_FALSE(thread.promise());
    }

    {
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block2);
        Script script(target.get(), block2, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findBroadcasts("0")).WillOnce(Return(std::vector<int>({ 5, 7, 8 })));
        EXPECT_CALL(m_engineMock, broadcast(5, &thread, false));
        EXPECT_CALL(m_engineMock, broadcast(7, &thread, false));
        EXPECT_CALL(m_engineMock, broadcast(8, &thread, false));
        thread.run();
        ASSERT_TRUE(thread.isFinished());
        ASSERT_FALSE(thread.promise());
    }
}

TEST_F(EventBlocksTest, BroadcastAndWait)
{
    auto broadcast = std::make_shared<Broadcast>("", "test");
    m_engine->setBroadcasts({ broadcast });

    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_broadcastandwait");
    builder.addEntityInput("BROADCAST_INPUT", "test", InputValue::Type::Broadcast, broadcast);
    auto block1 = builder.currentBlock();

    builder.addBlock("event_broadcastandwait");
    builder.addNullObscuredInput("BROADCAST_INPUT");
    auto block2 = builder.currentBlock();

    block1->setNext(nullptr);
    block2->setParent(nullptr);

    {
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block1);
        Script script(target.get(), block1, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findBroadcasts("test")).WillOnce(Return(std::vector<int>({ 1, 4 })));
        EXPECT_CALL(m_engineMock, broadcast(1, &thread, true));
        EXPECT_CALL(m_engineMock, broadcast(4, &thread, true));
        thread.run();
        ASSERT_FALSE(thread.isFinished());
        ASSERT_TRUE(thread.promise());
    }

    {
        Compiler compiler(&m_engineMock, target.get());
        auto code = compiler.compile(block2);
        Script script(target.get(), block2, &m_engineMock);
        script.setCode(code);
        Thread thread(target.get(), &m_engineMock, &script);

        EXPECT_CALL(m_engineMock, findBroadcasts("0")).WillOnce(Return(std::vector<int>({ 5, 7, 8 })));
        EXPECT_CALL(m_engineMock, broadcast(5, &thread, true));
        EXPECT_CALL(m_engineMock, broadcast(7, &thread, true));
        EXPECT_CALL(m_engineMock, broadcast(8, &thread, true));
        thread.run();
        ASSERT_FALSE(thread.isFinished());
        ASSERT_TRUE(thread.promise());
    }
}

TEST_F(EventBlocksTest, WhenKeyPressed)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whenkeypressed");
    builder.addDropdownField("KEY_OPTION", "a");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addKeyPressScript(block, block->fieldAt(0).get()));
    compiler.compile(block);
}
