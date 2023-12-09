// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/project.h>
#include <string>

namespace libscratchcpp
{

class IEngine;
class IProjectDownloaderFactory;
class IProjectDownloader;

struct ProjectPrivate
{
        ProjectPrivate();
        ProjectPrivate(const std::string &fileName);
        ProjectPrivate(const std::string &fileName, ScratchVersion scratchVersion);
        ProjectPrivate(const ProjectPrivate &) = delete;

        bool load();

        void start();
        void run();
        void runEventLoop();

        void detectScratchVersion();
        void setScratchVersion(ScratchVersion version);

        void setDownloadProgressCallback(const std::function<void(unsigned int, unsigned int)> &f);

        ScratchVersion scratchVersion = ScratchVersion::Invalid;
        std::string fileName;
        std::shared_ptr<IEngine> engine = nullptr;

        static IProjectDownloaderFactory *downloaderFactory;
        std::shared_ptr<IProjectDownloader> downloader;
};

}; // namespace libscratchcpp
