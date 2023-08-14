#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class ExtensionBase : public IExtension
{
    public:
        std::string description() const override;

        void registerSections(IEngine *engine) override;
};

} // namespace libscratchcpp
