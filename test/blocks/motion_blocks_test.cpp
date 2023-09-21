#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/motionblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class MotionBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<MotionBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(MotionBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Motion");
}

TEST_F(MotionBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(MotionBlocksTest, RegisterBlocks)
{
    m_section->registerBlocks(&m_engineMock);
}
