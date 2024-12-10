#include <scratchcpp/project.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/sprite.h>
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
