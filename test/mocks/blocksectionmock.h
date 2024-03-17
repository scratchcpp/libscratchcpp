#pragma once

#include <scratchcpp/iblocksection.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class BlockSectionMock : public IBlockSection
{
    public:
        MOCK_METHOD(std::string, name, (), (const, override));
        MOCK_METHOD(bool, categoryVisible, (), (const, override));

        MOCK_METHOD(void, registerBlocks, (IEngine *), (override));
        MOCK_METHOD(void, onInit, (IEngine *), (override));
};
