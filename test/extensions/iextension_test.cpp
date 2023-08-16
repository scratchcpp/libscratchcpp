#include <enginemock.h>

#include "../common.h"
#include "testextension.h"

using namespace libscratchcpp;

class IExtensionTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<TestExtension>(); };

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engine;
};

TEST_F(IExtensionTest, Name)
{
    ASSERT_EQ(m_extension->name(), "Test");
}

TEST_F(IExtensionTest, Description)
{
    ASSERT_EQ(m_extension->description(), "Test extension");
}

TEST_F(IExtensionTest, IncludeByDefault)
{
    ASSERT_FALSE(m_extension->includeByDefault());
}

TEST_F(IExtensionTest, RegisterSections)
{
    std::shared_ptr<IBlockSection> section = nullptr;
    EXPECT_CALL(m_engine, registerSection(section)).Times(1);
    m_extension->registerSections(&m_engine);
}
