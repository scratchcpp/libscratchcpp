#include <imageformats/stub/imageformatstub.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ImageFormatStubTest, Width)
{
    ImageFormatStub img;
    ASSERT_EQ(img.width(), 0);
}

TEST(ImageFormatStubTest, Height)
{
    ImageFormatStub img;
    ASSERT_EQ(img.height(), 0);
}

TEST(ImageFormatStubTest, ColorAt)
{
    ImageFormatStub img;
    ASSERT_EQ(img.colorAt(0, 0, 1), 0);
    ASSERT_EQ(img.colorAt(0, 1, 1), 0);
    ASSERT_EQ(img.colorAt(0, 2, 1), 0);
    ASSERT_EQ(img.colorAt(1, 0, 1), 0);
    ASSERT_EQ(img.colorAt(1, 1, 1), 0);
    ASSERT_EQ(img.colorAt(1, 2, 1), 0);
    ASSERT_EQ(img.colorAt(2, 0, 1), 0);
    ASSERT_EQ(img.colorAt(2, 1, 1), 0);
    ASSERT_EQ(img.colorAt(2, 2, 1), 0);
    ASSERT_EQ(img.colorAt(2, 2, 2.5), 0);
}
