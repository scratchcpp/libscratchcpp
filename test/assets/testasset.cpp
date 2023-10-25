#include "testasset.h"

using namespace libscratchcpp;

TestAsset::TestAsset() :
    Asset("", "", "")
{
}

void TestAsset::processData(const char *data)
{
    processedData = data;
    callCount++;
}
