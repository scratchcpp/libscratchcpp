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

IProjectDownloaderFactory *ProjectPrivate::downloaderFactory = nullptr;

ProjectPrivate::ProjectPrivate() :
    engine(std::make_shared<Engine>())
{
    if (!downloaderFactory)
        downloaderFactory = ProjectDownloaderFactory::instance().get();

    downloader = downloaderFactory->create();
}

ProjectPrivate::ProjectPrivate(const std::string &fileName) :
    ProjectPrivate()
{
    this->fileName = fileName;

    // Auto detect Scratch version
    detectScratchVersion();
}

ProjectPrivate::ProjectPrivate(const std::string &fileName, ScratchVersion scratchVersion) :
    fileName(fileName),
    engine(std::make_shared<Engine>())
{
    setScratchVersion(scratchVersion);
}

bool ProjectPrivate::load()
{
    std::shared_ptr<IProjectReader> reader;
    switch (scratchVersion) {
        case ScratchVersion::Invalid:
            std::cerr << "Could not read the project because version is set to invalid." << std::endl;
            return false;
        case ScratchVersion::Scratch3:
            reader = std::make_shared<Scratch3Reader>();
            break;
    }

    // Load from URL
    ProjectUrl url(fileName);

    if (url.isProjectUrl()) {
        // Download JSON
        if (!downloader->downloadJson(url.projectId())) {
            std::cerr << "Failed to download the project file." << std::endl;
            return false;
        }

        bool ret = reader->loadData(downloader->json());

        if (!ret)
            return false;

        // Get asset file names
        std::vector<std::string> assetNames;
        std::unordered_map<std::string, Asset *> assets;
        const auto &targets = reader->targets();

        for (auto target : targets) {
            const auto &costumes = target->costumes();
            const auto &sounds = target->sounds();

            for (auto costume : costumes) {
                auto it = std::find(assetNames.begin(), assetNames.end(), costume->fileName());
                if (it == assetNames.end()) {
                    assetNames.push_back(costume->fileName());
                    assets[assetNames.back()] = costume.get();
                } else
                    assets[*it] = costume.get();
            }

            for (auto sound : sounds) {
                auto it = std::find(assetNames.begin(), assetNames.end(), sound->fileName());
                if (it == assetNames.end()) {
                    assetNames.push_back(sound->fileName());
                    assets[assetNames.back()] = sound.get();
                } else
                    assets[*it] = sound.get();
            }
        }

        // Download assets
        if (!downloader->downloadAssets(assetNames)) {
            std::cerr << "Failed to download the project assets." << std::endl;
            return false;
        }

        const auto &assetData = downloader->assets();
        assert(assetData.size() == assetNames.size());

        // Load asset data
        for (size_t i = 0; i < assets.size(); i++) {
            const std::string &data = assetData[i];
            assets[assetNames[i]]->setData(data.size(), static_cast<void *>(const_cast<char *>(data.c_str())));
        }

    } else {
        // Load from file
        reader->setFileName(fileName);
        if (!reader->isValid()) {
            scratchVersion = ScratchVersion::Invalid;
            std::cerr << "Could not read the project." << std::endl;
            return false;
        }

        bool ret = reader->load();
        if (!ret)
            return false;
    }

    engine->clear();
    engine->setTargets(reader->targets());
    engine->setBroadcasts(reader->broadcasts());
    engine->setMonitors(reader->monitors());
    engine->setExtensions(reader->extensions());
    engine->compile();
    return true;
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

void ProjectPrivate::detectScratchVersion()
{
    ProjectUrl url(fileName);

    scratchVersion = ScratchVersion::Invalid;
    Scratch3Reader scratch3;

    if (url.isProjectUrl()) {
        if (!downloader->downloadJson(url.projectId())) {
            std::cerr << "Failed to download the project file." << std::endl;
            return;
        }

        scratch3.loadData(downloader->json());
    } else
        scratch3.setFileName(fileName);

    if (scratch3.isValid())
        scratchVersion = ScratchVersion::Scratch3;

    if (scratchVersion == ScratchVersion::Invalid)
        std::cerr << "Unable to determine Scratch version." << std::endl;
}

void ProjectPrivate::setScratchVersion(ScratchVersion version)
{
    // TODO: Use this when more versions become supported
    // if((version >= Version::Scratch3) && (version <= Version::Scratch3))
    if (version == ScratchVersion::Scratch3)
        scratchVersion = version;
    else
        std::cerr << "Unsupported Scratch version: " << static_cast<int>(version) << std::endl;
}

void ProjectPrivate::setDownloadProgressCallback(const std::function<void(unsigned int, unsigned int)> &f)
{
    downloader->setDownloadProgressCallback(f);
}
