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

/*! Destroys Asset. */
Asset::~Asset()
{
    if (impl->data && !impl->dataCloned) {
        free(impl->data);
        impl->data = nullptr;
    }
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
const std::string &Asset::fileName() const
{
    return impl->fileName;
}

/*! Returns the name of the format of the asset file. */
const std::string &Asset::dataFormat() const
{
    return impl->dataFormat;
}

/*! Returns the asset data. */
const void *Asset::data() const
{
    return impl->data;
}

/*! Returns the size of the asset data. */
unsigned int Asset::dataSize() const
{
    return impl->dataSize;
}

/*! Sets the asset data (will be deallocated when the object is destroyed). */
void Asset::setData(unsigned int size, void *data)
{
    if (impl->data)
        free(impl->data);

    impl->dataSize = size;
    impl->data = data;
    impl->dataCloned = isClone();
    processData(size, data);
}

/*! Returns the sprite or stage this asset belongs to. */
Target *Asset::target() const
{
    return impl->target;
}

/*! Sets the sprite or stage this asset belongs to. */
void Asset::setTarget(Target *target)
{
    impl->target = target;
}
