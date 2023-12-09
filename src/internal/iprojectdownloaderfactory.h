// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IProjectDownloader;

class IProjectDownloaderFactory
{
    public:
        virtual ~IProjectDownloaderFactory() { }

        virtual std::shared_ptr<IProjectDownloader> create() const = 0;
};

} // namespace libscratchcpp
