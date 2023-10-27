#include "testasset.h"

using namespace libscratchcpp;

TestAsset::TestAsset() :
    Asset("", "", "")
{
}

void TestAsset::processData(unsigned int size, void *data)
{
    this->size = size;
    processedData = data;
    callCount++;
}
