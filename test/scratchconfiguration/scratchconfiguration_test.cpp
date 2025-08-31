#include <scratchcpp/scratchconfiguration.h>
#include <graphicseffectmock.h>

#include "../common.h"
#include "extension1.h"
#include "extension2.h"
#include "extension3.h"

using namespace libscratchcpp;

using ::testing::Return;

class ScratchConfigurationTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension1 = std::make_shared<Extension1>();
            m_extension2 = std::make_shared<Extension2>();
            m_extension3 = std::make_shared<Extension3>();
        }

        std::shared_ptr<IExtension> m_extension1;
        std::shared_ptr<IExtension> m_extension2;
        std::shared_ptr<IExtension> m_extension3;
};

TEST_F(ScratchConfigurationTest, Extensions)
{
    ScratchConfiguration::registerExtension(m_extension1);
    ScratchConfiguration::registerExtension(m_extension2);

    ASSERT_EQ(ScratchConfiguration::getExtension("ext 1"), m_extension1.get());
    ASSERT_EQ(ScratchConfiguration::getExtension("ext 2"), m_extension2.get());
    ASSERT_EQ(ScratchConfiguration::getExtension("ext 3"), nullptr);

    ASSERT_EQ(ScratchConfiguration::getExtension<Extension1>(), m_extension1.get());
    ASSERT_EQ(ScratchConfiguration::getExtension<Extension2>(), m_extension2.get());
    ASSERT_EQ(ScratchConfiguration::getExtension<Extension3>(), nullptr);

    ScratchConfiguration::removeExtension(m_extension1);

    ASSERT_EQ(ScratchConfiguration::getExtension("ext 1"), nullptr);
    ASSERT_EQ(ScratchConfiguration::getExtension("ext 2"), m_extension2.get());

    ASSERT_EQ(ScratchConfiguration::getExtension<Extension1>(), nullptr);
    ASSERT_EQ(ScratchConfiguration::getExtension<Extension2>(), m_extension2.get());
}

TEST_F(ScratchConfigurationTest, GraphicsEffects)
{
    auto effect1 = std::make_shared<GraphicsEffectMock>();
    auto effect2 = std::make_shared<GraphicsEffectMock>();

    EXPECT_CALL(*effect1, name()).WillOnce(Return("effect1"));
    EXPECT_CALL(*effect2, name()).WillOnce(Return("effect2"));
    ScratchConfiguration::registerGraphicsEffect(effect1);
    ScratchConfiguration::registerGraphicsEffect(effect2);

    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect1"), effect1.get());
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect2"), effect2.get());
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect3"), nullptr);

    ScratchConfiguration::removeGraphicsEffect("effect2");
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect1"), effect1.get());
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect2"), nullptr);

    ScratchConfiguration::removeGraphicsEffect("effect1");
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect1"), nullptr);
}

TEST_F(ScratchConfigurationTest, GraphicsEffects_CaseInsensitive)
{
    auto effect1 = std::make_shared<GraphicsEffectMock>();
    auto effect2 = std::make_shared<GraphicsEffectMock>();

    EXPECT_CALL(*effect1, name()).WillOnce(Return("eFfEct1"));
    EXPECT_CALL(*effect2, name()).WillOnce(Return("eFFeCT2"));
    ScratchConfiguration::registerGraphicsEffect(effect1);
    ScratchConfiguration::registerGraphicsEffect(effect2);

    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("EFFECT1"), effect1.get());
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effEct2"), effect2.get());
    ASSERT_EQ(ScratchConfiguration::getGraphicsEffect("effect3"), nullptr);

    ScratchConfiguration::removeGraphicsEffect("effect1");
    ScratchConfiguration::removeGraphicsEffect("effect2");
}
