// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

class Target;

struct AssetPrivate
{
        AssetPrivate(const std::string &name, const std::string &format);
        AssetPrivate(const AssetPrivate &) = delete;

        void updateFileName(const std::string &id);

        std::string name;
        std::string dataFormat;
        std::string fileName;
        const void *data = nullptr;
        unsigned int dataSize = 0;
        Target *target = nullptr;
};

} // namespace libscratchcpp
