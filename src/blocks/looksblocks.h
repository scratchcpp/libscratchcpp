// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class LooksBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;
};

} // namespace libscratchcpp
