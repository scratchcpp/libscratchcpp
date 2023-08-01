// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "global.h"

namespace libscratchcpp
{

class IExtension;
class ScratchConfigurationPrivate;

/*! \brief The ScratchConfiguration class provides methods for adding custom extensions. */
class LIBSCRATCHCPP_EXPORT ScratchConfiguration
{
    public:
        ScratchConfiguration() = delete;

        static void registerExtension(std::shared_ptr<IExtension> extension);
        static IExtension *getExtension(const std::string &name);

        /*! Finds the extension of class T. Returns nullptr if it isn't registered. */
        template<class T>
        static IExtension *getExtension()
        {
            auto &extensions = getExtensions();
            for (auto ext : extensions) {
                if (dynamic_cast<T *>(ext.get()))
                    return ext.get();
            }

            return nullptr;
        };

    private:
        static const std::vector<std::shared_ptr<IExtension>> getExtensions();

        static std::unique_ptr<ScratchConfigurationPrivate> impl;
};

} // namespace libscratchcpp
