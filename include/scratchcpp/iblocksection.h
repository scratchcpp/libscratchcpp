// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <map>
#include <spimpl.h>

#include "global.h"

namespace libscratchcpp
{

class BlockSectionPrivate;
class IEngine;

/*!
 * \brief The IBlockSection class is an interface for block sections/categories.
 *
 * \see <a href="blockSections.html">Block sections</a>
 */
class LIBSCRATCHCPP_EXPORT IBlockSection
{
    public:
        virtual ~IBlockSection() { }

        /*! Override this method to return the name of the block section. */
        virtual std::string name() const = 0;

        /*!
         * Override this method to configure whether the category in the block palette should be visible.\n
         * For example, there isn't a list category in the Scratch user interface.
         * \note The default implementation returns true.
         */
        virtual bool categoryVisible() const { return true; }

        /*! Override this method to register blocks. */
        virtual void registerBlocks(IEngine *engine) = 0;
};

} // namespace libscratchcpp
