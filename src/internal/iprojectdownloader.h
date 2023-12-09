// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <functional>

namespace libscratchcpp
{

class IProjectDownloader
{
    public:
        virtual ~IProjectDownloader() { }

        virtual bool downloadJson(const std::string &projectId) = 0;
        virtual bool downloadAssets(const std::vector<std::string> &assetIds) = 0;
        virtual void cancel() = 0;

        virtual void setDownloadProgressCallback(const std::function<void(unsigned int, unsigned int)> &f) = 0;

        virtual const std::string &json() const = 0;
        virtual const std::vector<std::string> &assets() const = 0;
        virtual unsigned int downloadedAssetCount() const = 0;
};

} // namespace libscratchcpp
