// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <iostream>

#include "project_p.h"
#include "internal/scratch3reader.h"
#include "internal/projectdownloaderfactory.h"
#include "internal/projectdownloader.h"
#include "internal/projecturl.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

ProjectPrivate::ProjectPrivate() :
    engine(std::make_shared<Engine>())
{
    if (!m_downloaderFactory)
        m_downloaderFactory = ProjectDownloaderFactory::instance().get();

    downloader = m_downloaderFactory->create();
}

ProjectPrivate::ProjectPrivate(const std::string &fileName) :
    fileName(fileName),
    engine(std::make_shared<Engine>())
{
    this->fileName = fileName;
}

bool ProjectPrivate::load()
{
    std::shared_ptr<IProjectReader> reader;

    // Scratch 3
    reader = std::make_shared<Scratch3Reader>();

    if (tryLoad(reader.get()))
        return true;

    std::cerr << "Unsupported Scratch version." << std::endl;
    return false;
}

bool ProjectPrivate::tryLoad(IProjectReader *reader)
{
    stopLoading = false;

    // Load from URL
    ProjectUrl url(fileName);

    if (url.isProjectUrl()) {
        // Download JSON
        if (!downloader->downloadJson(url.projectId())) {
            std::cerr << "Failed to download the project file." << std::endl;
            return false;
        }

        if (stopLoading) {
            loadingAborted();
            return false;
        }

        bool ret = reader->loadData(downloader->json());

        if (stopLoading) {
            loadingAborted();
            return false;
        }

        if (!ret)
            return false;

        // Get asset file names
        std::vector<std::string> assetNames;
        std::unordered_map<std::string, std::vector<Asset *>> assets; // multiple assets can use the same file
        const auto &targets = reader->targets();

        for (auto target : targets) {
            const auto &costumes = target->costumes();
            const auto &sounds = target->sounds();

            for (auto costume : costumes) {
                auto it = std::find(assetNames.begin(), assetNames.end(), costume->fileName());
                if (it == assetNames.end()) {
                    assetNames.push_back(costume->fileName());
                    assets[assetNames.back()] = { costume.get() };
                } else
                    assets[*it].push_back(costume.get());
            }

            for (auto sound : sounds) {
                auto it = std::find(assetNames.begin(), assetNames.end(), sound->fileName());
                if (it == assetNames.end()) {
                    assetNames.push_back(sound->fileName());
                    assets[assetNames.back()] = { sound.get() };
                } else
                    assets[*it].push_back(sound.get());
            }
        }

        // Download assets
        ret = downloader->downloadAssets(assetNames);

        if (stopLoading) {
            loadingAborted();
            return false;
        }

        if (!ret) {
            std::cerr << "Failed to download the project assets." << std::endl;
            return false;
        }

        const auto &assetData = downloader->assets();
        assert(assetData.size() == assetNames.size());

        // Load asset data
        for (size_t i = 0; i < assets.size(); i++) {
            const std::vector<Asset *> &assetList = assets[assetNames[i]];

            for (Asset *asset : assetList) {
                const std::string &data = assetData[i];
                char *ptr = (char *)malloc(data.size() * sizeof(char));
                memcpy(ptr, data.data(), data.size() * sizeof(char));
                asset->setData(data.size(), ptr);
            }
        }

    } else {
        // Load from file
        reader->setFileName(fileName);
        if (!reader->isValid()) {
            std::cerr << "Could not read the project." << std::endl;
            return false;
        }

        if (stopLoading) {
            loadingAborted();
            return false;
        }

        bool ret = reader->load();

        if (stopLoading) {
            loadingAborted();
            return false;
        }

        if (!ret)
            return false;
    }

    engine->clear();
    engine->setTargets(reader->targets());
    engine->setBroadcasts(reader->broadcasts());
    engine->setMonitors(reader->monitors());
    engine->setExtensions(reader->extensions());
    engine->setUserAgent(reader->userAgent());
    engine->compile();

    if (stopLoading) {
        loadingAborted();
        return false;
    }

    return true;
}

void ProjectPrivate::loadingAborted()
{
    std::cout << "Loading aborted." << std::endl;
}

void ProjectPrivate::start()
{
    engine->start();
}

void ProjectPrivate::run()
{
    engine->run();
}

void ProjectPrivate::runEventLoop()
{
    engine->runEventLoop();
}

sigslot::signal<unsigned int, unsigned int> &ProjectPrivate::downloadProgressChanged()
{
    return downloader->downloadProgressChanged();
}

IProjectDownloaderFactory *ProjectPrivate::downloaderFactory()
{
    return m_downloaderFactory;
}

void ProjectPrivate::setDownloaderFactory(IProjectDownloaderFactory *downloaderFactory)
{
    m_downloaderFactory = downloaderFactory;
}
