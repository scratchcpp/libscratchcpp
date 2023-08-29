// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/asset.h>

#include "asset_p.h"

using namespace libscratchcpp;

/*! Constructs Asset. */
Asset::Asset(const std::string &name, const std::string &id, const std::string &format) :
    Entity(id),
    impl(spimpl::make_unique_impl<AssetPrivate>(name, format))
{
    impl->updateFileName(id);
}

/*! Sets the ID (MD5 hash) of the asset file. */
void Asset::setId(const std::string &id)
{
    Entity::setId(id);
    impl->updateFileName(id);
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
