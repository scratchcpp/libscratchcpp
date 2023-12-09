// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "idownloaderfactory.h"

namespace libscratchcpp
{

class DownloaderFactory : public IDownloaderFactory
{
    public:
        DownloaderFactory();

        static std::shared_ptr<DownloaderFactory> instance();
        std::shared_ptr<IDownloader> create() const override;

    private:
        static std::shared_ptr<DownloaderFactory> m_instance;
};

} // namespace libscratchcpp
