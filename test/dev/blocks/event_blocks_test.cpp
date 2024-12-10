#include <scratchcpp/project.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/eventblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

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
