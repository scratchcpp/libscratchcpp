#pragma once

#include <internal/iprojectdownloaderfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ProjectDownloaderFactoryMock : public IProjectDownloaderFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<IProjectDownloader>, create, (), (const, override));
};
