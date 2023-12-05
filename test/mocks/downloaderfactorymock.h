#pragma once

#include <internal/idownloaderfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class DownloaderFactoryMock : public IDownloaderFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<IDownloader>, create, (), (const, override));
};
