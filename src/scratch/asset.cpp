// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/asset.h>

#include "asset_p.h"

using namespace libscratchcpp;

/*! Constructs Asset. */
Asset::Asset(const std::string &name, const std::string &id, const std::string &format) :
    impl(spimpl::make_impl<AssetPrivate>(name, id, format))
{
}

/*! Returns the MD5 hash of the asset file. */
const std::string &Asset::assetId() const
{
    return impl->assetId;
}

/*! Returns the name of the asset. */
const std::string &Asset::name() const
{
    return impl->name;
}

/*! Returns the name of the asset file. */
const std::string &Asset::md5ext() const
{
    return impl->fileName;
}

/*! Returns the name of the format of the asset file. */
const std::string &Asset::dataFormat() const
{
    return impl->dataFormat;
}
