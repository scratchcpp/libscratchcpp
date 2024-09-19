// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iextension.h>
#include <algorithm>

#include "scratchconfiguration_p.h"

using namespace libscratchcpp;

void ScratchConfigurationPrivate::registerExtension(std::shared_ptr<IExtension> extension)
{
    if (std::find(extensions.begin(), extensions.end(), extension) != extensions.cend())
        return;

    extensions.push_back(extension);
}

void ScratchConfigurationPrivate::removeExtension(std::shared_ptr<IExtension> extension)
{
    extensions.erase(std::remove(extensions.begin(), extensions.end(), extension), extensions.end());
}

IExtension *ScratchConfigurationPrivate::getExtension(std::string name)
{
    for (auto ext : extensions) {
        if (ext->name() == name)
            return ext.get();
    }

    return nullptr;
}
