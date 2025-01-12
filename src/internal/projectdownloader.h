// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <atomic>
#include <mutex>

#include "iprojectdownloader.h"

namespace libscratchcpp
{

class IDownloaderFactory;
class IDownloader;

class ProjectDownloader : public IProjectDownloader
{
    public:
        ProjectDownloader(IDownloaderFactory *downloaderFactory = nullptr);

        bool downloadJson(const std::string &projectId) override;
        bool downloadAssets(const std::vector<std::string> &assetIds) override;
        void cancel() override;

        sigslot::signal<unsigned int, unsigned int> &downloadProgressChanged() override;

        const std::string &json() const override;
        const std::vector<std::string> &assets() const override;
        unsigned int downloadedAssetCount() const override;

    private:
        IDownloaderFactory *m_downloaderFactory = nullptr;
        std::shared_ptr<IDownloader> m_tokenDownloader;
        std::shared_ptr<IDownloader> m_jsonDownloader;
        std::vector<std::string> m_assets;
        std::mutex m_assetsMutex;
        std::atomic<unsigned int> m_downloadedAssetCount = 0;
        std::atomic<bool> m_cancel = false;
        sigslot::signal<unsigned int, unsigned int> m_downloadProgressChanged;
};

} // namespace libscratchcpp
