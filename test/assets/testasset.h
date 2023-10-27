#pragma once

#include <scratchcpp/asset.h>

namespace libscratchcpp
{

class TestAsset : public Asset
{
    public:
        TestAsset();

        void *processedData = nullptr;
        unsigned int size = 0;
        unsigned int callCount = 0;

    protected:
        void processData(unsigned int size, void *data) override;
};

} // namespace libscratchcpp
