#include <scratchcpp/asset.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(AssetTest, Constructors)
{
    Asset asset("costume1", "a", "svg");
    ASSERT_EQ(asset.id(), "a");
    ASSERT_EQ(asset.name(), "costume1");
    ASSERT_EQ(asset.dataFormat(), "svg");
    ASSERT_EQ(asset.fileName(), "a.svg");
}

TEST(AssetTest, Id)
{
    Asset asset("costume1", "a", "svg");

    asset.setId("b");
    ASSERT_EQ(asset.id(), "b");
    ASSERT_EQ(asset.fileName(), "b.svg");
}
