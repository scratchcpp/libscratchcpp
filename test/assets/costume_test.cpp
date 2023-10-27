#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/broadcast.h>
#include <imageformatfactorymock.h>
#include <imageformatmock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

class CostumeTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_imageFormatFactory = std::make_shared<ImageFormatFactoryMock>();
            m_imageFormat = std::make_shared<ImageFormatMock>();
            ScratchConfiguration::registerImageFormat("test", m_imageFormatFactory);
        }

        void TearDown() override { ScratchConfiguration::removeImageFormat("test"); }

        std::shared_ptr<ImageFormatFactoryMock> m_imageFormatFactory;
        std::shared_ptr<ImageFormatMock> m_imageFormat;
};

TEST_F(CostumeTest, Constructors)
{
    Costume costume("costume1", "a", "svg");
    ASSERT_EQ(costume.id(), "a");
    ASSERT_EQ(costume.name(), "costume1");
    ASSERT_EQ(costume.dataFormat(), "svg");
    ASSERT_EQ(costume.fileName(), "a.svg");
    ASSERT_EQ(costume.bitmapResolution(), 1);
    ASSERT_EQ(costume.rotationCenterX(), 0);
    ASSERT_EQ(costume.rotationCenterY(), 0);
}

TEST_F(CostumeTest, BitmapResolution)
{
    Costume costume("costume1", "a", "svg");

    costume.setBitmapResolution(5.52);
    ASSERT_EQ(costume.bitmapResolution(), 5.52);
}

TEST_F(CostumeTest, RotationCenterX)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterX(240);
    ASSERT_EQ(costume.rotationCenterX(), 240);
}

TEST_F(CostumeTest, RotationCenterY)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterY(180);
    ASSERT_EQ(costume.rotationCenterY(), 180);
}

TEST_F(CostumeTest, WidthAndHeight)
{
    EXPECT_CALL(*m_imageFormatFactory, createInstance()).WillOnce(Return(m_imageFormat));
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(0));
    Costume costume("costume1", "a", "test");

    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(480));
    ASSERT_EQ(costume.width(), 480);

    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(360));
    ASSERT_EQ(costume.height(), 360);

    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(0));
    costume.setScale(3.5);

    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(480));
    ASSERT_EQ(costume.width(), 1680);

    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(360));
    ASSERT_EQ(costume.height(), 1260);
}

TEST_F(CostumeTest, Scale)
{
    Costume costume("costume1", "a", "");
    ASSERT_EQ(costume.scale(), 1);

    costume.setScale(2.894);
    ASSERT_EQ(costume.scale(), 2.894);
}

TEST_F(CostumeTest, MirrorHorizontally)
{
    Costume costume("costume1", "a", "");
    ASSERT_FALSE(costume.mirrorHorizontally());

    costume.setMirrorHorizontally(true);
    ASSERT_TRUE(costume.mirrorHorizontally());
}

TEST_F(CostumeTest, Bitmap)
{
    EXPECT_CALL(*m_imageFormatFactory, createInstance()).WillOnce(Return(m_imageFormat));
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(0));
    Costume costume("costume1", "a", "test");

    // Do not mirror horizontally
    static const char *data = "abcd";
    EXPECT_CALL(*m_imageFormat, setData(data));
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 0, 1)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 0, 1)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 0, 1)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 0, 1)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 1, 1)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 1, 1)).WillOnce(Return(10));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 1, 1)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 1, 1)).WillOnce(Return(8));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 2, 1)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 2, 1)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 2, 1)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 2, 1)).WillOnce(Return(8));
    costume.setData(data);

    auto bitmap = costume.bitmap();
    ASSERT_TRUE(bitmap);
    ASSERT_EQ(bitmap[0][0], 5);
    ASSERT_EQ(bitmap[0][1], 3);
    ASSERT_EQ(bitmap[0][2], 8);
    ASSERT_EQ(bitmap[0][3], 1);

    ASSERT_EQ(bitmap[1][0], 9);
    ASSERT_EQ(bitmap[1][1], 10);
    ASSERT_EQ(bitmap[1][2], 5);
    ASSERT_EQ(bitmap[1][3], 8);

    ASSERT_EQ(bitmap[2][0], 1);
    ASSERT_EQ(bitmap[2][1], 9);
    ASSERT_EQ(bitmap[2][2], 4);
    ASSERT_EQ(bitmap[2][3], 8);

    // Mirror horizontally
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 0, 1)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 0, 1)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 0, 1)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 0, 1)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 1, 1)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 1, 1)).WillOnce(Return(10));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 1, 1)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 1, 1)).WillOnce(Return(8));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 2, 1)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 2, 1)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 2, 1)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 2, 1)).WillOnce(Return(8));
    costume.setMirrorHorizontally(true);

    bitmap = costume.bitmap();
    ASSERT_TRUE(bitmap);
    ASSERT_EQ(bitmap[0][0], 1);
    ASSERT_EQ(bitmap[0][1], 8);
    ASSERT_EQ(bitmap[0][2], 3);
    ASSERT_EQ(bitmap[0][3], 5);

    ASSERT_EQ(bitmap[1][0], 8);
    ASSERT_EQ(bitmap[1][1], 5);
    ASSERT_EQ(bitmap[1][2], 10);
    ASSERT_EQ(bitmap[1][3], 9);

    ASSERT_EQ(bitmap[2][0], 8);
    ASSERT_EQ(bitmap[2][1], 4);
    ASSERT_EQ(bitmap[2][2], 9);
    ASSERT_EQ(bitmap[2][3], 1);
}

TEST_F(CostumeTest, ScaledBitmap)
{
    EXPECT_CALL(*m_imageFormatFactory, createInstance()).WillOnce(Return(m_imageFormat));
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(0));
    Costume costume("costume1", "a", "test");

    // Do not mirror horizontally
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 0, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 0, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 0, 1.5)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 0, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 0, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 0, 1.5)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 1, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 1, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 1, 1.5)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 1, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 1, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 1, 1.5)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 2, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 2, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 2, 1.5)).WillOnce(Return(10));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 2, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 2, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 2, 1.5)).WillOnce(Return(8));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 3, 1.5)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 3, 1.5)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 3, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 3, 1.5)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 3, 1.5)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 3, 1.5)).WillOnce(Return(8));
    costume.setScale(1.5);

    auto bitmap = costume.bitmap();
    ASSERT_TRUE(bitmap);
    ASSERT_EQ(bitmap[0][0], 5);
    ASSERT_EQ(bitmap[0][1], 5);
    ASSERT_EQ(bitmap[0][2], 3);
    ASSERT_EQ(bitmap[0][3], 8);
    ASSERT_EQ(bitmap[0][4], 8);
    ASSERT_EQ(bitmap[0][5], 1);

    ASSERT_EQ(bitmap[1][0], 5);
    ASSERT_EQ(bitmap[1][1], 5);
    ASSERT_EQ(bitmap[1][2], 3);
    ASSERT_EQ(bitmap[1][3], 8);
    ASSERT_EQ(bitmap[1][4], 8);
    ASSERT_EQ(bitmap[1][5], 1);

    ASSERT_EQ(bitmap[2][0], 9);
    ASSERT_EQ(bitmap[2][1], 9);
    ASSERT_EQ(bitmap[2][2], 10);
    ASSERT_EQ(bitmap[2][3], 5);
    ASSERT_EQ(bitmap[2][4], 5);
    ASSERT_EQ(bitmap[2][5], 8);

    ASSERT_EQ(bitmap[3][0], 1);
    ASSERT_EQ(bitmap[3][1], 1);
    ASSERT_EQ(bitmap[3][2], 9);
    ASSERT_EQ(bitmap[3][3], 4);
    ASSERT_EQ(bitmap[3][4], 4);
    ASSERT_EQ(bitmap[3][5], 8);

    // Mirror horizontally
    EXPECT_CALL(*m_imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 0, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 0, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 0, 1.5)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 0, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 0, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 0, 1.5)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 1, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 1, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 1, 1.5)).WillOnce(Return(3));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 1, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 1, 1.5)).WillOnce(Return(8));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 1, 1.5)).WillOnce(Return(1));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 2, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 2, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 2, 1.5)).WillOnce(Return(10));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 2, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 2, 1.5)).WillOnce(Return(5));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 2, 1.5)).WillOnce(Return(8));

    EXPECT_CALL(*m_imageFormat, colorAt(0, 3, 1.5)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(1, 3, 1.5)).WillOnce(Return(1));
    EXPECT_CALL(*m_imageFormat, colorAt(2, 3, 1.5)).WillOnce(Return(9));
    EXPECT_CALL(*m_imageFormat, colorAt(3, 3, 1.5)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(4, 3, 1.5)).WillOnce(Return(4));
    EXPECT_CALL(*m_imageFormat, colorAt(5, 3, 1.5)).WillOnce(Return(8));
    costume.setMirrorHorizontally(true);

    bitmap = costume.bitmap();
    ASSERT_TRUE(bitmap);
    ASSERT_EQ(bitmap[0][0], 1);
    ASSERT_EQ(bitmap[0][1], 8);
    ASSERT_EQ(bitmap[0][2], 8);
    ASSERT_EQ(bitmap[0][3], 3);
    ASSERT_EQ(bitmap[0][4], 5);
    ASSERT_EQ(bitmap[0][5], 5);

    ASSERT_EQ(bitmap[1][0], 1);
    ASSERT_EQ(bitmap[1][1], 8);
    ASSERT_EQ(bitmap[1][2], 8);
    ASSERT_EQ(bitmap[1][3], 3);
    ASSERT_EQ(bitmap[1][4], 5);
    ASSERT_EQ(bitmap[1][5], 5);

    ASSERT_EQ(bitmap[2][0], 8);
    ASSERT_EQ(bitmap[2][1], 5);
    ASSERT_EQ(bitmap[2][2], 5);
    ASSERT_EQ(bitmap[2][3], 10);
    ASSERT_EQ(bitmap[2][4], 9);
    ASSERT_EQ(bitmap[2][5], 9);

    ASSERT_EQ(bitmap[3][0], 8);
    ASSERT_EQ(bitmap[3][1], 4);
    ASSERT_EQ(bitmap[3][2], 4);
    ASSERT_EQ(bitmap[3][3], 9);
    ASSERT_EQ(bitmap[3][4], 1);
    ASSERT_EQ(bitmap[3][5], 1);
}

TEST_F(CostumeTest, Broadcast)
{
    Costume costume("costume1", "a", "svg");

    Broadcast *broadcast = costume.broadcast();
    ASSERT_TRUE(broadcast);
    ASSERT_TRUE(broadcast->id().empty());
    ASSERT_TRUE(broadcast->name().empty());
}
