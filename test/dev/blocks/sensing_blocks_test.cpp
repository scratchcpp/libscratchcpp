#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/sensingblocks.h"

using namespace libscratchcpp;

class SensingBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<SensingBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
