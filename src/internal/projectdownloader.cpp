// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

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
    m_tokenDownloader->startDownload(PROJECT_META_PREFIX + projectId);
    m_tokenDownloader->wait();

    if (m_tokenDownloader->text().empty()) {
        std::cerr << "Could not fetch project info of " << projectId << std::endl;
        return false;
    }

    if (m_tokenDownloader->isCancelled())
        return false;

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
    m_jsonDownloader->startDownload(PROJECT_JSON_PREFIX + projectId + "?token=" + token);
    m_jsonDownloader->wait();

    if (m_jsonDownloader->text().empty()) {
        std::cerr << "Failed to download project JSON of " << projectId << std::endl;
        return false;
    }

    if (m_jsonDownloader->isCancelled())
        return false;

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
    unsigned int times; // how many times we should download assets simultaneously (in "groups")
    m_assets.clear();
    m_downloadedAssetCount = 0;

    // Calculate number of "groups"
    if (threadCount == 0) {
        times = count;
        threadCount = 1;
    } else
        times = std::ceil(count / static_cast<double>(threadCount));

    std::cout << "Downloading " << count << " asset(s)";

    if (threadCount > 1)
        std::cout << " using " << threadCount << " threads";

    std::cout << std::endl;

    // Create downloaders
    std::vector<std::shared_ptr<IDownloader>> downloaders;

    for (unsigned int i = 0; i < threadCount; i++)
        downloaders.push_back(m_downloaderFactory->create());

    // Download assets
    for (unsigned int i = 0; i < times; i++) {
        unsigned int currentCount = std::min(threadCount, static_cast<unsigned int>(count - i * threadCount));

        for (unsigned int j = 0; j < currentCount; j++)
            downloaders[j]->startDownload(ASSET_PREFIX + assetIds[i * threadCount + j] + ASSET_SUFFIX);

        for (unsigned int j = 0; j < currentCount; j++) {
            downloaders[j]->wait();
            assert(m_assets.size() == i * threadCount + j);

            if (downloaders[j]->isCancelled())
                return false;

            CHECK_CANCEL();

            m_assets.push_back(downloaders[j]->text());
            m_downloadedAssetCount++;
        }
    }

    return true;
}

void ProjectDownloader::cancel()
{
    m_tokenDownloader->cancel();
    m_jsonDownloader->cancel();
    m_cancelMutex.lock();
    m_cancel = true;
    m_cancelMutex.unlock();
    m_downloadedAssetCount = 0;
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
