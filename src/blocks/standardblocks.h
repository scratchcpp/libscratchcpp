// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

#include "../libscratchcpp_global.h"

namespace libscratchcpp
{

/*! \brief The StandardBlocks class provides the default Scratch blocks. */
class LIBSCRATCHCPP_EXPORT StandardBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        bool includeByDefault() const override;

        void registerSections(IEngine *engine) override;
};

} // namespace libscratchcpp
