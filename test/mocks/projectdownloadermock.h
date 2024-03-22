#pragma once

#include <internal/iprojectdownloader.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ProjectDownloaderMock : public IProjectDownloader
{
    public:
        MOCK_METHOD(bool, downloadJson, (const std::string &), (override));
        MOCK_METHOD(bool, downloadAssets, (const std::vector<std::string> &), (override));
        MOCK_METHOD(void, cancel, (), (override));

        MOCK_METHOD((sigslot::signal<unsigned int, unsigned int> &), downloadProgressChanged, (), (override));

        MOCK_METHOD(const std::string &, json, (), (const, override));
        MOCK_METHOD(const std::vector<std::string> &, assets, (), (const, override));
        MOCK_METHOD(unsigned int, downloadedAssetCount, (), (const, override));
};
