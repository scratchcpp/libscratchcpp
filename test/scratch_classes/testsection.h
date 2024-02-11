#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class TestSection : public IBlockSection
{
    public:
        std::string name() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
