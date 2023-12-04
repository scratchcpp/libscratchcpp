// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IDownloader;

class IDownloaderFactory
{
    public:
        virtual ~IDownloaderFactory() { }

        virtual std::shared_ptr<IDownloader> create() const = 0;
};

} // namespace libscratchcpp
