// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/igraphicseffect.h>
#include <iostream>

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

/*! Registers the given graphics effect. */
void ScratchConfiguration::registerGraphicsEffect(std::shared_ptr<IGraphicsEffect> effect)
{
    if (!effect)
        return;

    impl->graphicsEffects[effect->name()] = effect;
}

/*! Removes the given graphics effect. */
void ScratchConfiguration::removeGraphicsEffect(const std::string &name)
{
    impl->graphicsEffects.erase(name);
}

/*! Returns the graphics effect with the given name, or nullptr if it isn't registered. */
IGraphicsEffect *ScratchConfiguration::getGraphicsEffect(const std::string &name)
{
    auto it = impl->graphicsEffects.find(name);

    if (it == impl->graphicsEffects.cend())
        return nullptr;
    else
        return it->second.get();
}

/*! Returns the version string of the library. */
const std::string &ScratchConfiguration::version()
{
    static const std::string ret = LIBSCRATCHCPP_VERSION;
    return ret;
}

/*! Returns the major version of the library. */
int ScratchConfiguration::majorVersion()
{
    return LIBSCRATCHCPP_VERSION_MAJOR;
}

/*! Returns the minor version of the library. */
int ScratchConfiguration::minorVersion()
{
    return LIBSCRATCHCPP_VERSION_MINOR;
}

/*! Returns the patch version of the library. */
int ScratchConfiguration::patchVersion()
{
    return LIBSCRATCHCPP_VERSION_PATCH;
}

const std::vector<std::shared_ptr<IExtension>> ScratchConfiguration::getExtensions()
{
    return impl->extensions;
}
