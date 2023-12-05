#pragma once

#include <internal/idownloader.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class DownloaderMock : public IDownloader
{
    public:
        MOCK_METHOD(void, startDownload, (const std::string &), (override));
        MOCK_METHOD(void, cancel, (), (override));
        MOCK_METHOD(void, wait, (), (override));

        MOCK_METHOD(bool, isCancelled, (), (const, override));
        MOCK_METHOD(const std::string &, text, (), (const, override));
};
