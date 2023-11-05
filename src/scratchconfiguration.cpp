// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/iimageformatfactory.h>
#include <scratchcpp/igraphicseffect.h>
#include <iostream>

#include "scratchconfiguration_p.h"
#include "blocks/standardblocks.h"
#include "imageformats/stub/imageformatstub.h"

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

/*! Registers the given image format. */
void ScratchConfiguration::registerImageFormat(const std::string &name, std::shared_ptr<IImageFormatFactory> formatFactory)
{
    impl->imageFormats[name] = formatFactory;
}

/*! Removes the given image format. */
void ScratchConfiguration::removeImageFormat(const std::string &name)
{
    impl->imageFormats.erase(name);
}

/*! Creates an instance of the given image format. If the format doesn't exist, a "stub" format will be created. */
std::shared_ptr<IImageFormat> ScratchConfiguration::createImageFormat(const std::string &name)
{
    auto it = impl->imageFormats.find(name);

    if (it == impl->imageFormats.cend()) {
        std::cerr << "Unsupported image format: " << name << std::endl;

        // Use stub if the format doesn't exist
        return std::make_shared<ImageFormatStub>();
    } else
        return it->second->createInstance();
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

const std::vector<std::shared_ptr<IExtension>> ScratchConfiguration::getExtensions()
{
    return impl->extensions;
}
