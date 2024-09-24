#include <scratchcpp/drawable.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(DrawableTest, IsTarget)
{
    Drawable drawable;
    ASSERT_FALSE(drawable.isTarget());
}

TEST(DrawableTest, IsTextBubble)
{
    Drawable drawable;
    ASSERT_FALSE(drawable.isTextBubble());
}

TEST(DrawableTest, LayerOrder)
{
    Drawable drawable;
    ASSERT_EQ(drawable.layerOrder(), 0);
    drawable.setLayerOrder(2);
    ASSERT_EQ(drawable.layerOrder(), 2);
}

TEST(TargetTest, Engine)
{
    Drawable drawable;
    ASSERT_EQ(drawable.engine(), nullptr);

    EngineMock engine;
    drawable.setEngine(&engine);
    ASSERT_EQ(drawable.engine(), &engine);
}
