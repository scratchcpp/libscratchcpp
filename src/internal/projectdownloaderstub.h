// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iprojectdownloader.h"

namespace libscratchcpp
{

class ProjectDownloaderStub : public IProjectDownloader
{
    public:
        ProjectDownloaderStub();

        bool downloadJson(const std::string &) override;
        bool downloadAssets(const std::vector<std::string> &) override;
        void cancel() override;

        sigslot::signal<unsigned int, unsigned int> &downloadProgressChanged() override;

        const std::string &json() const override;
        const std::vector<std::string> &assets() const override;
        unsigned int downloadedAssetCount() const override;

    private:
        sigslot::signal<unsigned int, unsigned int> m_downloadProgressChanged;
};

} // namespace libscratchcpp
