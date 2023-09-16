#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/sensingblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

class SensingBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<SensingBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(SensingBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Sensing");
}

TEST_F(SensingBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(SensingBlocksTest, RegisterBlocks)
{
    m_section->registerBlocks(&m_engineMock);
}
