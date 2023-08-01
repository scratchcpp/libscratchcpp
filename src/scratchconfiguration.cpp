// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>

#include "scratchconfiguration_p.h"
#include "blocks/standardblocks.h"

using namespace libscratchcpp;

std::unique_ptr<ScratchConfigurationPrivate> ScratchConfiguration::impl = std::make_unique<ScratchConfigurationPrivate>();

/*! Registers the given extension. */
void ScratchConfiguration::registerExtension(std::shared_ptr<IExtension> extension)
{
    impl->registerExtension(extension);
}

/*! Returns the extension with the given name, or nullptr if it isn't registered. */
IExtension *ScratchConfiguration::getExtension(const std::string &name)
{
    return impl->getExtension(name);
}

const std::vector<std::shared_ptr<IExtension>> ScratchConfiguration::getExtensions()
{
    return impl->extensions;
}
