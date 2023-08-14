#pragma once

#include "extensionbase.h"

namespace libscratchcpp
{

class Extension1 : public ExtensionBase
{
    public:
        std::string name() const override;
};

} // namespace libscratchcpp
