#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/variableblocks.h"

using namespace libscratchcpp;

class VariableBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<VariableBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
