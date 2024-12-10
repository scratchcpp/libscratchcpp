#include <scratchcpp/project.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/dev/executablecode.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/eventblocks.h"

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

// TODO: Add test for when touching object hat predicate

TEST_F(EventBlocksTest, WhenTouchingObject)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whentouchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "Sprite1");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addWhenTouchingObjectScript(block));
    compiler.compile(block);
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

// TODO: Add test for when greater than hat predicate

TEST_F(EventBlocksTest, WhenGreaterThan)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("event_whengreaterthan");
    builder.addDropdownField("WHENGREATERTHANMENU", "LOUDNESS");
    builder.addValueInput("VALUE", 8.9);
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    EXPECT_CALL(m_engineMock, addWhenGreaterThanScript(block));
    compiler.compile(block);
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
    }
}
