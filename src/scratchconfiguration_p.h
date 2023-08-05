// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "blocks/standardblocks.h"

#include <memory>
#include <string>
#include <vector>

namespace libscratchcpp
{

class IExtension;

struct ScratchConfigurationPrivate
{
        void registerExtension(std::shared_ptr<IExtension> extension);
        IExtension *getExtension(std::string name);

        std::vector<std::shared_ptr<IExtension>> extensions = { std::make_shared<StandardBlocks>() };
};

} // namespace libscratchcpp
