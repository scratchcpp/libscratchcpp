// SPDX-License-Identifier: Apache-2.0

#include "projectdownloaderstub.h"

using namespace libscratchcpp;

ProjectDownloaderStub::ProjectDownloaderStub()
{
}

bool ProjectDownloaderStub::downloadJson(const std::string &)
{
    return false;
}

bool ProjectDownloaderStub::downloadAssets(const std::vector<std::string> &)
{
    return false;
}

void ProjectDownloaderStub::cancel()
{
}

sigslot::signal<unsigned int, unsigned int> &ProjectDownloaderStub::downloadProgressChanged()
{
    return m_downloadProgressChanged;
}

const std::string &ProjectDownloaderStub::json() const
{
    static const std::string empty;
    return empty;
}

const std::vector<std::string> &ProjectDownloaderStub::assets() const
{
    static const std::vector<std::string> empty;
    return empty;
}

unsigned int ProjectDownloaderStub::downloadedAssetCount() const
{
    return 0;
}
