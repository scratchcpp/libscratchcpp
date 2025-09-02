// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iprojectdownloaderfactory.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT ProjectDownloaderFactory : public IProjectDownloaderFactory
{
    public:
        ProjectDownloaderFactory();

        static std::shared_ptr<ProjectDownloaderFactory> instance();
        std::shared_ptr<IProjectDownloader> create() const override;

    private:
        static std::shared_ptr<ProjectDownloaderFactory> m_instance;
};

} // namespace libscratchcpp
