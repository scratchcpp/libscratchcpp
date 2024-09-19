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

TEST_F(IExtensionTest, RegisterBlocks)
{
    EXPECT_CALL(m_engine, clear);
    m_extension->registerBlocks(&m_engine);
}

TEST_F(IExtensionTest, OnInit)
{
    EXPECT_CALL(m_engine, clear);
    m_extension->onInit(&m_engine);
}
