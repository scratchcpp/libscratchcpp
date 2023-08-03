// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct AssetPrivate
{
        AssetPrivate(const std::string &name, const std::string &id, const std::string &format);

        void updateFileName();

        std::string assetId;
        std::string name;
        std::string dataFormat;
        std::string fileName;
};

} // namespace libscratchcpp
