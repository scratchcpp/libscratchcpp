#include <scratchcpp/asset.h>
#include <scratchcpp/target.h>

#include "../common.h"
#include "testasset.h"

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

TEST(AssetTest, Data)
{
    TestAsset asset;
    ASSERT_EQ(asset.data(), nullptr);

    static char data[5] = "abcd";
    asset.setData(5, data);
    ASSERT_EQ(asset.data(), data);
    ASSERT_EQ(asset.size, 5);
    ASSERT_EQ(asset.processedData, data);
    ASSERT_EQ(asset.callCount, 1);
}

TEST(AssetTest, Target)
{
    Asset asset("sound1", "a", "wav");
    ASSERT_EQ(asset.target(), nullptr);

    Target target;
    asset.setTarget(&target);
    ASSERT_EQ(asset.target(), &target);
}
