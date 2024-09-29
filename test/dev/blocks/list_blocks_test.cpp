#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/listblocks.h"

using namespace libscratchcpp;

class ListBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<ListBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
