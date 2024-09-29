#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/controlblocks.h"

using namespace libscratchcpp;

class ControlBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<ControlBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
