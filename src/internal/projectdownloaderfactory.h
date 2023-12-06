// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iprojectdownloaderfactory.h"

namespace libscratchcpp
{

class ProjectDownloaderFactory : public IProjectDownloaderFactory
{
    public:
        ProjectDownloaderFactory();

        static std::shared_ptr<ProjectDownloaderFactory> instance();
        std::shared_ptr<IProjectDownloader> create() const override;

    private:
        static std::shared_ptr<ProjectDownloaderFactory> m_instance;
};

} // namespace libscratchcpp
