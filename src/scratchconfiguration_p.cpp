// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iextension.h>

#include "scratchconfiguration_p.h"

using namespace libscratchcpp;

void ScratchConfigurationPrivate::registerExtension(std::shared_ptr<IExtension> extension)
{
    extensions.push_back(extension);
}

IExtension *ScratchConfigurationPrivate::getExtension(std::string name)
{
    for (auto ext : extensions) {
        if (ext->name() == name)
            return ext.get();
    }

    return nullptr;
}
