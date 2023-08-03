// SPDX-License-Identifier: Apache-2.0

#include "asset_p.h"

using namespace libscratchcpp;

AssetPrivate::AssetPrivate(const std::string &name, const std::string &id, const std::string &format) :
    name(name),
    assetId(id),
    dataFormat(format)
{
    updateFileName();
}

void AssetPrivate::updateFileName()
{
    // NOTE: fileName depends on assetId and dataFormat
    fileName = assetId + "." + dataFormat;
}
