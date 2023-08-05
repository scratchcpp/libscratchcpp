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

        /*!
         * Returns true if the extension is hidden from the block palette
         * and should be available in a Scratch project by default
         */
        virtual bool includeByDefault() const { return false; }

        /*! Registers block sections. \see <a href="blockSections.html">Block sections</a> */
        virtual void registerSections(IEngine *engine) = 0;
};

} // namespace libscratchcpp
