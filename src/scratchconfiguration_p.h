// SPDX-License-Identifier: Apache-2.0

#pragma once

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

        std::vector<std::shared_ptr<IExtension>> extensions;
};

} // namespace libscratchcpp
