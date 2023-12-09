#pragma once

#include <internal/idownloader.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class DownloaderMock : public IDownloader
{
    public:
        MOCK_METHOD(bool, download, (const std::string &), (override));
        MOCK_METHOD(const std::string &, text, (), (const, override));
};
