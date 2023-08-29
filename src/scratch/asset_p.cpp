// SPDX-License-Identifier: Apache-2.0

#include "asset_p.h"

using namespace libscratchcpp;

AssetPrivate::AssetPrivate(const std::string &name, const std::string &format) :
    name(name),
    dataFormat(format)
{
}

void AssetPrivate::updateFileName(const std::string &id)
{
    // NOTE: fileName depends on id and dataFormat
    fileName = id + "." + dataFormat;
}
