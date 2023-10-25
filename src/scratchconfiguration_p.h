// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "blocks/standardblocks.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace libscratchcpp
{

class IExtension;
class IImageFormatFactory;

struct ScratchConfigurationPrivate
{
        void registerExtension(std::shared_ptr<IExtension> extension);
        IExtension *getExtension(std::string name);

        std::vector<std::shared_ptr<IExtension>> extensions = { std::make_shared<StandardBlocks>() };
        std::unordered_map<std::string, std::shared_ptr<IImageFormatFactory>> imageFormats;
};

} // namespace libscratchcpp
