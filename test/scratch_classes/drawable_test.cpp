#include <scratchcpp/drawable.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(DrawableTest, LayerOrder)
{
    Drawable drawable;
    ASSERT_EQ(drawable.layerOrder(), 0);
    drawable.setLayerOrder(2);
    ASSERT_EQ(drawable.layerOrder(), 2);
}
