#pragma once

#include "extensionbase.h"

namespace libscratchcpp
{

class Extension2 : public ExtensionBase
{
    public:
        std::string name() const override;
        Rgb color() const override;
};

} // namespace libscratchcpp
