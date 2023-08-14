#include <scratchcpp/scratchconfiguration.h>

#include "../common.h"
#include "extension1.h"
#include "extension2.h"
#include "extension3.h"

using namespace libscratchcpp;

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
}
