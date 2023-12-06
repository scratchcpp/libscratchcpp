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

        const std::string &json() const override;
        const std::vector<std::string> &assets() const override;
        unsigned int downloadedAssetCount() const override;

    private:
        IDownloaderFactory *m_downloaderFactory = nullptr;
        std::shared_ptr<IDownloader> m_tokenDownloader;
        std::shared_ptr<IDownloader> m_jsonDownloader;
        std::vector<std::string> m_assets;
        std::atomic<unsigned int> m_downloadedAssetCount = 0;
        bool m_cancel = false;
        std::mutex m_cancelMutex;
};

} // namespace libscratchcpp
