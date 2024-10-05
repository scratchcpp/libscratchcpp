#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/operatorblocks.h"

using namespace libscratchcpp;

class OperatorBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<OperatorBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
