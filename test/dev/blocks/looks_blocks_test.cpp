#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/looksblocks.h"

using namespace libscratchcpp;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<LooksBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
