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

TEST(RectTest, Intersects)
{
    Rect rect1(-50, 25, 150, -75);
    Rect rect1_ydown(-50, -75, 150, 25);
    ASSERT_TRUE(rect1.intersects(rect1));
    ASSERT_TRUE(rect1_ydown.intersects(rect1_ydown));

    // left
    {
        Rect rect2(-75, 0, 125, -50);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-100, 10, -50, -90);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-150, 10, -100, -90);
        ASSERT_FALSE(rect1.intersects(rect2));
        ASSERT_FALSE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_FALSE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_FALSE(rect2_ydown.intersects(rect1_ydown));
    }

    // top
    {
        Rect rect2(-25, 50, 125, 10);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-100, 50, 200, 25);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-100, 75, 200, 50);
        ASSERT_FALSE(rect1.intersects(rect2));
        ASSERT_FALSE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_FALSE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_FALSE(rect2_ydown.intersects(rect1_ydown));
    }

    // right
    {
        Rect rect2(125, 0, 200, -50);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(150, 10, 200, -90);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(175, 10, 200, -90);
        ASSERT_FALSE(rect1.intersects(rect2));
        ASSERT_FALSE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_FALSE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_FALSE(rect2_ydown.intersects(rect1_ydown));
    }

    // bottom
    {
        Rect rect2(-25, -50, 125, -100);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-100, -75, 200, -100);
        ASSERT_TRUE(rect1.intersects(rect2));
        ASSERT_TRUE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_TRUE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_TRUE(rect2_ydown.intersects(rect1_ydown));
    }

    {
        Rect rect2(-100, -100, 200, -125);
        ASSERT_FALSE(rect1.intersects(rect2));
        ASSERT_FALSE(rect2.intersects(rect1));

        Rect rect2_ydown(rect2.left(), rect2.bottom(), rect2.right(), rect2.top());
        ASSERT_FALSE(rect1_ydown.intersects(rect2_ydown));
        ASSERT_FALSE(rect2_ydown.intersects(rect1_ydown));
    }
}
