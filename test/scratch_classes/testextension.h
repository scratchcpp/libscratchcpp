#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class TestExtension : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
