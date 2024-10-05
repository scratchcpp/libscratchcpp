#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/customblocks.h"

using namespace libscratchcpp;

class CustomBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<CustomBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
