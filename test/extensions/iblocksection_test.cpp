#include <enginemock.h>

#include "../common.h"
#include "testsection.h"

using namespace libscratchcpp;

class IBlockSectionTest : public testing::Test
{
    public:
        void SetUp() override { m_section = std::make_unique<TestSection>(); };

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engine;
};

TEST_F(IBlockSectionTest, Name)
{
    ASSERT_EQ(m_section->name(), "Test");
}

TEST_F(IBlockSectionTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(IBlockSectionTest, RegisterBlocks)
{
    EXPECT_CALL(m_engine, addCompileFunction(m_section.get(), "", nullptr)).Times(1);
    m_section->registerBlocks(&m_engine);
}
