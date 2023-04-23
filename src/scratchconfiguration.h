// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "engine/iextension.h"
#include "libscratchcpp_global.h"
#include <memory>
#include <vector>

namespace libscratchcpp
{

/*! \brief The ScratchConfiguration class provides methods for adding custom extensions. */
class LIBSCRATCHCPP_EXPORT ScratchConfiguration
{
    public:
        ScratchConfiguration() = delete;

        static void registerExtension(std::shared_ptr<IExtension> extension);
        static IExtension *getExtension(std::string name);

        /*! Finds the extension of class T. Returns nullptr if it isn't registered. */
        template<class T>
        static IExtension *getExtension()
        {
            for (auto ext : m_extensions) {
                if (dynamic_cast<T *>(ext.get()))
                    return ext.get();
            }

            return nullptr;
        };

    private:
        static std::vector<std::shared_ptr<IExtension>> m_extensions;
};

} // namespace libscratchcpp
