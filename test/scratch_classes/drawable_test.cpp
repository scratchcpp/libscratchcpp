#include <scratchcpp/drawable.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

class LayerOrderMock
{
    public:
        MOCK_METHOD(void, f, (int));
};

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

    LayerOrderMock spy;
    drawable.layerOrderChanged().connect(&LayerOrderMock::f, &spy);

    EXPECT_CALL(spy, f(2));
    drawable.setLayerOrder(2);
    ASSERT_EQ(drawable.layerOrder(), 2);

    EXPECT_CALL(spy, f(0));
    drawable.setLayerOrder(0);
    ASSERT_EQ(drawable.layerOrder(), 0);
}

TEST(TargetTest, Engine)
{
    Drawable drawable;
    ASSERT_EQ(drawable.engine(), nullptr);

    EngineMock engine;
    drawable.setEngine(&engine);
    ASSERT_EQ(drawable.engine(), &engine);
}
