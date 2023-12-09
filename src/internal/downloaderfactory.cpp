// SPDX-License-Identifier: Apache-2.0

#include "downloaderfactory.h"
#include "downloader.h"

using namespace libscratchcpp;

std::shared_ptr<DownloaderFactory> DownloaderFactory::m_instance = std::make_shared<DownloaderFactory>();

DownloaderFactory::DownloaderFactory()
{
}

std::shared_ptr<DownloaderFactory> DownloaderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<IDownloader> DownloaderFactory::create() const
{
    return std::make_shared<Downloader>();
}
