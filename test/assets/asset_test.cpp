#include <scratchcpp/asset.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(AssetTest, Constructors)
{
    Asset asset("costume1", "a", "svg");
    ASSERT_EQ(asset.assetId(), "a");
    ASSERT_EQ(asset.name(), "costume1");
    ASSERT_EQ(asset.dataFormat(), "svg");
    ASSERT_EQ(asset.md5ext(), "a.svg");
}
