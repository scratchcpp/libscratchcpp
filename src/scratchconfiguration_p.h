// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace libscratchcpp
{

class IExtension;
class IImageFormatFactory;
class IGraphicsEffect;

struct ScratchConfigurationPrivate
{
        void registerExtension(std::shared_ptr<IExtension> extension);
        void removeExtension(std::shared_ptr<IExtension> extension);
        IExtension *getExtension(std::string name);

        std::vector<std::shared_ptr<IExtension>> extensions;
        std::unordered_map<std::string, std::shared_ptr<IGraphicsEffect>> graphicsEffects;
};

} // namespace libscratchcpp
