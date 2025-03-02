#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/textbubble.h>
#include <enginemock.h>
#include <graphicseffectmock.h>

#include "../common.h"
#include "blocks/looksblocks.h"

using namespace libscratchcpp;

using ::testing::Return;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<LooksBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            m_extension->onInit(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(LooksBlocksTest, StopProject)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    GraphicsEffectMock effect;
    m_engine->setTargets({ stage, sprite });

    stage->bubble()->setText("abc");
    sprite->bubble()->setText("def");
    EXPECT_CALL(effect, clamp(10)).WillOnce(Return(10));
    sprite->setGraphicsEffectValue(&effect, 10);
    EXPECT_CALL(effect, clamp(2.5)).WillOnce(Return(2.5));
    stage->setGraphicsEffectValue(&effect, 2.5);

    m_engine->stop();
    ASSERT_TRUE(stage->bubble()->text().empty());
    ASSERT_TRUE(sprite->bubble()->text().empty());
    ASSERT_EQ(stage->graphicsEffectValue(&effect), 0);
    ASSERT_EQ(sprite->graphicsEffectValue(&effect), 0);
}
