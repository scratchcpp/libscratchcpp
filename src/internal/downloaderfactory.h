// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "idownloaderfactory.h"

#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT DownloaderFactory : public IDownloaderFactory
{
    public:
        DownloaderFactory();

        static std::shared_ptr<DownloaderFactory> instance();
        std::shared_ptr<IDownloader> create() const override;

    private:
        static std::shared_ptr<DownloaderFactory> m_instance;
};

} // namespace libscratchcpp
