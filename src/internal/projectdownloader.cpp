// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "projectdownloader.h"
#include "downloaderfactory.h"
#include "idownloader.h"

using namespace libscratchcpp;

static const std::string PROJECT_META_PREFIX = "https://api.scratch.mit.edu/projects/";
static const std::string PROJECT_JSON_PREFIX = "https://projects.scratch.mit.edu/";
static const std::string ASSET_PREFIX = "https://assets.scratch.mit.edu/internalapi/asset/";
static const std::string ASSET_SUFFIX = "/get";

#define CHECK_CANCEL()                                                                                                                                                                                 \
    m_cancelMutex.lock();                                                                                                                                                                              \
    if (m_cancel) {                                                                                                                                                                                    \
        m_downloadedAssetCount = 0;                                                                                                                                                                    \
        m_cancelMutex.unlock();                                                                                                                                                                        \
        std::cout << "Download aborted!" << std::endl;                                                                                                                                                 \
        return false;                                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
    m_cancelMutex.unlock()

ProjectDownloader::ProjectDownloader(IDownloaderFactory *downloaderFactory) :
    m_downloaderFactory(downloaderFactory)
{
    if (!m_downloaderFactory)
        m_downloaderFactory = DownloaderFactory::instance().get();

    m_tokenDownloader = m_downloaderFactory->create();
    m_jsonDownloader = m_downloaderFactory->create();
}

bool ProjectDownloader::downloadJson(const std::string &projectId)
{
    m_cancelMutex.lock();
    m_cancel = false;
    m_cancelMutex.unlock();

    // Get project token
    std::cout << "Fetching project info of " << projectId << std::endl;
    bool ret = m_tokenDownloader->download(PROJECT_META_PREFIX + projectId);

    if (!ret) {
        std::cerr << "Could not fetch project info of " << projectId << std::endl;
        return false;
    }

    CHECK_CANCEL();

    nlohmann::json json;

    try {
        json = nlohmann::json::parse(m_tokenDownloader->text());
    } catch (std::exception &e) {
        std::cerr << "Could not parse project info of " << projectId << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    std::string token;

    try {
        token = json["project_token"];
    } catch (std::exception &e) {
        std::cerr << "Could not read project token of " << projectId << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Download project JSON
    std::cout << "Downloading project JSON of " << projectId << std::endl;
    ret = m_jsonDownloader->download(PROJECT_JSON_PREFIX + projectId + "?token=" + token);

    if (!ret) {
        std::cerr << "Failed to download project JSON of " << projectId << std::endl;
        return false;
    }

    CHECK_CANCEL();

    return true;
}

bool ProjectDownloader::downloadAssets(const std::vector<std::string> &assetIds)
{
    m_cancelMutex.lock();
    m_cancel = false;
    m_cancelMutex.unlock();

    auto count = assetIds.size();
    unsigned int threadCount = std::thread::hardware_concurrency();

    // Thread count: number of assets / 5, limited to maximum number of threads
    threadCount = std::max(1u, std::min(threadCount, static_cast<unsigned int>(std::ceil(count / 5.0))));

    m_assets.clear();
    m_assets.reserve(count);
    m_downloadedAssetCount = 0;

    for (unsigned int i = 0; i < count; i++)
        m_assets.push_back(std::string());

    std::cout << "Downloading " << count << " asset(s)";

    if (threadCount > 1)
        std::cout << " using " << threadCount << " threads";

    std::cout << std::endl;

    // Create downloaders
    std::vector<std::shared_ptr<IDownloader>> downloaders;

    for (unsigned int i = 0; i < threadCount; i++)
        downloaders.push_back(m_downloaderFactory->create());

    // Download assets
    auto f = [this, &downloaders, &assetIds, count, threadCount](unsigned int thread) {
        auto downloader = downloaders[thread];
        unsigned int n = std::ceil(count / static_cast<double>(threadCount));

        for (unsigned int i = 0; i < n; i++) {
            unsigned int index = thread * n + i;

            if (index < count) {
                m_cancelMutex.lock();

                if (m_cancel)
                    return;

                m_cancelMutex.unlock();

                bool ret = downloader->download(ASSET_PREFIX + assetIds[index] + ASSET_SUFFIX);

                if (!ret) {
                    std::cerr << "Failed to download asset: " << assetIds[index] << std::endl;
                    m_cancelMutex.lock();
                    m_cancel = true;
                    m_cancelMutex.unlock();
                    return;
                }

                m_assetsMutex.lock();
                m_assets[index] = downloader->text();
                m_downloadedAssetCount++;
                std::cout << "Downloaded assets: " << m_downloadedAssetCount << " of " << count << std::endl;
                m_downloadProgressChanged(m_downloadedAssetCount, count);
                m_assetsMutex.unlock();
            }
        }
    };

    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < threadCount; i++)
        threads.emplace_back(std::thread(f, i));

    for (unsigned int i = 0; i < threadCount; i++)
        threads[i].join();

    CHECK_CANCEL();

    return true;
}

void ProjectDownloader::cancel()
{
    m_cancelMutex.lock();
    m_cancel = true;
    m_cancelMutex.unlock();
    m_downloadedAssetCount = 0;
}

sigslot::signal<unsigned int, unsigned int> &ProjectDownloader::downloadProgressChanged()
{
    return m_downloadProgressChanged;
}

const std::string &ProjectDownloader::json() const
{
    return m_jsonDownloader->text();
}

const std::vector<std::string> &ProjectDownloader::assets() const
{
    return m_assets;
}

unsigned int ProjectDownloader::downloadedAssetCount() const
{
    return m_downloadedAssetCount;
}
