// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "global.h"

namespace libscratchcpp
{

class IEngine;

/*!
 * \brief The IExtension class is an interface for extensions.
 *
 * \see <a href="extensions.html">Extensions</a>
 */
class LIBSCRATCHCPP_EXPORT IExtension
{
    public:
        virtual ~IExtension() { }

        /*! Returns the name of the extension. */
        virtual std::string name() const = 0;

        /*! Returns the description of the extension. */
        virtual std::string description() const = 0;

        /*! Override this method to register blocks. */
        virtual void registerBlocks(IEngine *engine) = 0;

        /*! This method is called when a project is loaded. */
        virtual void onInit(IEngine *engine) { }
};

} // namespace libscratchcpp
