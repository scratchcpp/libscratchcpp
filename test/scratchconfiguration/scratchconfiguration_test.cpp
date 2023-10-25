#include <scratchcpp/scratchconfiguration.h>
#include <imageformatfactorymock.h>

#include "../common.h"
#include "imageformats/stub/imageformatstub.h"
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
            m_imageFormatMock1 = std::make_shared<ImageFormatFactoryMock>();
            m_imageFormatMock2 = std::make_shared<ImageFormatFactoryMock>();
        }

        std::shared_ptr<IExtension> m_extension1;
        std::shared_ptr<IExtension> m_extension2;
        std::shared_ptr<IExtension> m_extension3;
        std::shared_ptr<ImageFormatFactoryMock> m_imageFormatMock1;
        std::shared_ptr<ImageFormatFactoryMock> m_imageFormatMock2;
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

TEST_F(ScratchConfigurationTest, ImageFormats)
{
    ScratchConfiguration::registerImageFormat("format1", m_imageFormatMock1);
    ScratchConfiguration::registerImageFormat("format2", m_imageFormatMock2);
    auto format1 = std::make_shared<ImageFormatStub>();
    auto format2 = std::make_shared<ImageFormatStub>();

    EXPECT_CALL(*m_imageFormatMock1, createInstance()).WillOnce(Return(format1));
    ASSERT_EQ(ScratchConfiguration::createImageFormat("format1"), format1);

    EXPECT_CALL(*m_imageFormatMock2, createInstance()).WillOnce(Return(format2));
    ASSERT_EQ(ScratchConfiguration::createImageFormat("format2"), format2);

    auto format = ScratchConfiguration::createImageFormat("format3");
    IImageFormat *unsupportedFormat = format.get();
    ASSERT_TRUE(unsupportedFormat);
    ASSERT_TRUE(dynamic_cast<ImageFormatStub *>(unsupportedFormat));

    ScratchConfiguration::removeImageFormat("format1");
    format = ScratchConfiguration::createImageFormat("format1");
    unsupportedFormat = format.get();
    ASSERT_TRUE(unsupportedFormat);
    ASSERT_TRUE(dynamic_cast<ImageFormatStub *>(unsupportedFormat));

    EXPECT_CALL(*m_imageFormatMock2, createInstance()).WillOnce(Return(format2));
    ASSERT_EQ(ScratchConfiguration::createImageFormat("format2"), format2);

    ScratchConfiguration::removeImageFormat("format2");
    format = ScratchConfiguration::createImageFormat("format2");
    unsupportedFormat = format.get();
    ASSERT_TRUE(unsupportedFormat);
    ASSERT_TRUE(dynamic_cast<ImageFormatStub *>(unsupportedFormat));

    testing::Mock::AllowLeak(m_imageFormatMock1.get());
    testing::Mock::AllowLeak(m_imageFormatMock2.get());
}
