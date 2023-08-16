#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class TestExtension : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        bool includeByDefault() const override;

        void registerSections(IEngine *engine) override;
};

} // namespace libscratchcpp
