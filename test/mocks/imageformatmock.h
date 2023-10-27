#pragma once

#include <scratchcpp/iimageformat.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ImageFormatMock : public IImageFormat
{
    public:
        MOCK_METHOD(void, setData, (unsigned int, void *), (override));

        MOCK_METHOD(unsigned int, width, (), (const, override));
        MOCK_METHOD(unsigned int, height, (), (const, override));

        MOCK_METHOD(Rgb, colorAt, (unsigned int, unsigned int, double), (const, override));
};
