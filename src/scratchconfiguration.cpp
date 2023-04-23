// SPDX-License-Identifier: Apache-2.0

#include "scratchconfiguration.h"
#include "blocks/standardblocks.h"

using namespace libscratchcpp;

std::vector<std::shared_ptr<IExtension>> ScratchConfiguration::m_extensions = { std::make_shared<StandardBlocks>() };

/*! Registers the given extension. */
void ScratchConfiguration::registerExtension(std::shared_ptr<IExtension> extension)
{
    m_extensions.push_back(extension);
}

/*! Returns the extension with the given name, or nullptr if it isn't registered. */
IExtension *ScratchConfiguration::getExtension(std::string name)
{
    for (auto ext : m_extensions) {
        if (ext->name() == name)
            return ext.get();
    }

    return nullptr;
}
