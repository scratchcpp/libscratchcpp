// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/project.h>
#include <string>

namespace libscratchcpp
{

class IEngine;
class IProjectReader;
class IProjectDownloaderFactory;
class IProjectDownloader;

struct ProjectPrivate
{
        ProjectPrivate();
        ProjectPrivate(const std::string &fileName);
        ProjectPrivate(const ProjectPrivate &) = delete;

        bool load();
        bool tryLoad(IProjectReader *reader);
        void loadingAborted();

        void start();
        void run();
        void runEventLoop();

        sigslot::signal<unsigned int, unsigned int> &downloadProgressChanged();

        std::string fileName;
        std::atomic<bool> stopLoading = false;
        std::shared_ptr<IEngine> engine = nullptr;

        static IProjectDownloaderFactory *downloaderFactory;
        std::shared_ptr<IProjectDownloader> downloader;
};

}; // namespace libscratchcpp
