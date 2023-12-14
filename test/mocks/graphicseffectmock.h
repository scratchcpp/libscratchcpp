#pragma once

#include <scratchcpp/igraphicseffect.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class GraphicsEffectMock : public IGraphicsEffect
{
    public:
        MOCK_METHOD(std::string, name, (), (const, override));
};
