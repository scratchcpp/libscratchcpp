#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/motionblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class MotionBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<MotionBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(MotionBlocksTest, MoveSteps)
{
    {
        auto sprite = std::make_shared<Sprite>();
        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock("motion_movesteps");
        builder.addValueInput("STEPS", 30.25);

        sprite->setX(5.2);
        sprite->setY(-0.25);
        sprite->setDirection(-61.42);

        builder.build();
        builder.run();
        ASSERT_EQ(std::round(sprite->x() * 100) / 100, -21.36);
        ASSERT_EQ(std::round(sprite->y() * 100) / 100, 14.22);
    }

    m_engine->clear();

    {
        auto stage = std::make_shared<Stage>();
        ScriptBuilder builder(m_extension.get(), m_engine, stage);

        builder.addBlock("motion_movesteps");
        builder.addValueInput("STEPS", 30.25);

        builder.build();
        builder.run();
    }
}
