#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/eventblocks.h"

using namespace libscratchcpp;

class EventBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<EventBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
