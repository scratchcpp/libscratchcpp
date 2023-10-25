#pragma once

#include <scratchcpp/iimageformatfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ImageFormatFactoryMock : public IImageFormatFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<IImageFormat>, createInstance, (), (const, override));
};
