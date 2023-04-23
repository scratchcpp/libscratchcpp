#pragma once

#include "../engine/iextension.h"
// SPDX-License-Identifier: Apache-2.0

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

        void registerSections(Engine *engine) override;
};

} // namespace libscratchcpp
