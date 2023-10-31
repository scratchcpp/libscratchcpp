#include <scratchcpp/rect.h>
#include <cmath>

#include "../common.h"

using namespace libscratchcpp;

TEST(RectTest, Constructors)
{
    {
        Rect rect;
        ASSERT_EQ(rect.left(), 0);
        ASSERT_EQ(rect.top(), 0);
        ASSERT_EQ(rect.right(), 0);
        ASSERT_EQ(rect.bottom(), 0);
        ASSERT_EQ(rect.width(), 0);
        ASSERT_EQ(rect.height(), 0);
    }

    {
        Rect rect(8.4, 150.78, 145.89, -179.99);
        ASSERT_EQ(rect.left(), 8.4);
        ASSERT_EQ(rect.top(), 150.78);
        ASSERT_EQ(rect.right(), 145.89);
        ASSERT_EQ(rect.bottom(), -179.99);
        ASSERT_EQ(std::round(rect.width() * 100) / 100, 137.49);
        ASSERT_EQ(rect.height(), 330.77);
    }
}

TEST(RectTest, Left)
{
    Rect rect;

    rect.setLeft(-78.05);
    ASSERT_EQ(rect.left(), -78.05);
}

TEST(RectTest, Top)
{
    Rect rect;

    rect.setTop(-22.89);
    ASSERT_EQ(rect.top(), -22.89);
}

TEST(RectTest, Right)
{
    Rect rect;

    rect.setRight(100.512);
    ASSERT_EQ(rect.right(), 100.512);
}

TEST(RectTest, Bottom)
{
    Rect rect;

    rect.setBottom(-58.162);
    ASSERT_EQ(rect.bottom(), -58.162);
}

TEST(RectTest, Width)
{
    Rect rect;

    rect.setLeft(-78.05);
    rect.setRight(100.512);
    ASSERT_EQ(rect.width(), 178.562);
}

TEST(RectTest, Height)
{
    Rect rect;

    rect.setTop(-22.89);
    rect.setBottom(-58.162);
    ASSERT_EQ(rect.height(), 35.272);
}
