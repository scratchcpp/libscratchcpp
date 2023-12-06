// SPDX-License-Identifier: Apache-2.0

#include "projectdownloaderfactory.h"
#ifdef LIBSCRATCHCPP_NETWORK_SUPPORT
#include "projectdownloader.h"
#else
#include "projectdownloaderstub.h"
#endif

using namespace libscratchcpp;

std::shared_ptr<ProjectDownloaderFactory> ProjectDownloaderFactory::m_instance = std::make_shared<ProjectDownloaderFactory>();

ProjectDownloaderFactory::ProjectDownloaderFactory()
{
}

std::shared_ptr<ProjectDownloaderFactory> ProjectDownloaderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<IProjectDownloader> ProjectDownloaderFactory::create() const
{
#ifdef LIBSCRATCHCPP_NETWORK_SUPPORT
    return std::make_shared<ProjectDownloader>();
#else
    return std::make_shared<ProjectDownloaderStub>();
#endif
}
