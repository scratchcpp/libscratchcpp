#pragma once

#include <scratchcpp/asset.h>

namespace libscratchcpp
{

class TestAsset : public Asset
{
    public:
        TestAsset();

        const char *processedData = nullptr;
        unsigned int callCount = 0;

    protected:
        void processData(const char *data) override;
};

} // namespace libscratchcpp
