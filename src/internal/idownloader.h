// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

class IDownloader
{
    public:
        virtual ~IDownloader() { }

        virtual bool download(const std::string &url) = 0;
        virtual const std::string &text() const = 0;
};

} // namespace libscratchcpp
