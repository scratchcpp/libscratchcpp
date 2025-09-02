// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/project.h>
#include <string>

#include "test_export.h"

namespace libscratchcpp
{

class IEngine;
class IProjectReader;
class IProjectDownloaderFactory;
class IProjectDownloader;

class LIBSCRATCHCPP_TEST_EXPORT ProjectPrivate
{
    public:
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

        std::shared_ptr<IProjectDownloader> downloader;

        static IProjectDownloaderFactory *downloaderFactory();
        static void setDownloaderFactory(IProjectDownloaderFactory *downloaderFactory);

    private:
        static inline IProjectDownloaderFactory *m_downloaderFactory = nullptr;
};

}; // namespace libscratchcpp
