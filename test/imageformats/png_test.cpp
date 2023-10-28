#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/iimageformat.h>

#include "../common.h"

using namespace libscratchcpp;

class PngTest : public testing::Test
{
    public:
        void SetUp() override
        {
            img1 = ScratchConfiguration::createImageFormat("png");
            img2 = ScratchConfiguration::createImageFormat("png");

            struct stat stat_buf;

            FILE *f1 = fopen("image1.png", "r");
            ASSERT_TRUE(f1);
            ASSERT_EQ(fstat(fileno(f1), &stat_buf), 0);
            file1 = malloc(stat_buf.st_size);
            ASSERT_TRUE(file1);
            ASSERT_EQ(fread(file1, 1, stat_buf.st_size, f1), stat_buf.st_size);
            img1->setData(stat_buf.st_size, file1);

            FILE *f2 = fopen("image2.png", "r");
            ASSERT_TRUE(f2);
            ASSERT_EQ(fstat(fileno(f2), &stat_buf), 0);
            file2 = malloc(stat_buf.st_size);
            ASSERT_TRUE(file2);
            ASSERT_EQ(fread(file2, 1, stat_buf.st_size, f2), stat_buf.st_size);
            img2->setData(stat_buf.st_size, file2);
        }

        void TearDown() override
        {
            if (file1)
                free(file1);

            if (file2)
                free(file2);
        }

        std::shared_ptr<IImageFormat> img1;
        std::shared_ptr<IImageFormat> img2;
        void *file1 = nullptr;
        void *file2 = nullptr;
};

TEST_F(PngTest, Width)
{
    ASSERT_EQ(img1->width(), 6);
    ASSERT_EQ(img2->width(), 4);
}

TEST_F(PngTest, Height)
{
    ASSERT_EQ(img1->height(), 3);
    ASSERT_EQ(img2->height(), 6);
}

TEST_F(PngTest, ColorAt)
{
    // image1 - without scaling
    ASSERT_EQ(img1->colorAt(0, 0, 1), rgb(192, 192, 192));
    ASSERT_EQ(img1->colorAt(3, 0, 1), rgb(255, 128, 0));
    ASSERT_EQ(img1->colorAt(5, 0, 1), rgb(0, 255, 0));

    ASSERT_EQ(img1->colorAt(0, 1, 1), rgb(0, 255, 255));
    ASSERT_EQ(img1->colorAt(3, 1, 1), rgb(255, 128, 128));
    ASSERT_EQ(img1->colorAt(5, 1, 1), rgb(255, 255, 255));

    ASSERT_EQ(img1->colorAt(0, 2, 1), rgb(0, 0, 0));
    ASSERT_EQ(img1->colorAt(3, 2, 1), rgb(128, 128, 0));
    ASSERT_EQ(img1->colorAt(5, 2, 1), rgb(0, 128, 128));

    // image1 - with scaling
    ASSERT_EQ(img1->colorAt(0, 0, 2.5), rgb(192, 192, 192));
    ASSERT_EQ(img1->colorAt(8, 0, 2.5), rgb(255, 128, 0));
    ASSERT_EQ(img1->colorAt(13, 0, 2.5), rgb(0, 255, 0));

    ASSERT_EQ(img1->colorAt(0, 3, 2.5), rgb(0, 255, 255));
    ASSERT_EQ(img1->colorAt(8, 3, 2.5), rgb(255, 128, 128));
    ASSERT_EQ(img1->colorAt(13, 3, 2.5), rgb(255, 255, 255));

    ASSERT_EQ(img1->colorAt(0, 5, 2.5), rgb(0, 0, 0));
    ASSERT_EQ(img1->colorAt(8, 5, 2.5), rgb(128, 128, 0));
    ASSERT_EQ(img1->colorAt(13, 5, 2.5), rgb(0, 128, 128));

    // image2 - without scaling
    ASSERT_EQ(img2->colorAt(1, 0, 1), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(3, 1, 1), rgb(255, 128, 128));
    ASSERT_EQ(img2->colorAt(3, 2, 1), rgba(149, 255, 149, 148));
    ASSERT_EQ(img2->colorAt(2, 2, 1), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(0, 3, 1), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(2, 4, 1), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(1, 5, 1), rgba(0, 0, 0, 0));

    // image2 - with scaling
    ASSERT_EQ(img2->colorAt(3, 0, 2.5), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(8, 3, 2.5), rgb(255, 128, 128));
    ASSERT_EQ(img2->colorAt(8, 5, 2.5), rgba(149, 255, 149, 148));
    ASSERT_EQ(img2->colorAt(5, 5, 2.5), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(0, 8, 2.5), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(5, 10, 2.5), rgba(0, 0, 0, 0));
    ASSERT_EQ(img2->colorAt(3, 13, 2.5), rgba(0, 0, 0, 0));
}
