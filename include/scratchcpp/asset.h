// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <spimpl.h>

#include "global.h"

namespace libscratchcpp
{

class AssetPrivate;

/*! \brief The Asset class represents a Scratch asset, for example a Costume or a Sound. */
class LIBSCRATCHCPP_EXPORT Asset
{
    public:
        Asset(std::string name, std::string id, std::string format);

        const std::string &assetId() const;

        const std::string &name() const;

        const std::string &md5ext() const;

        const std::string &dataFormat() const;

    private:
        spimpl::impl_ptr<AssetPrivate> impl;
};

} // namespace libscratchcpp
