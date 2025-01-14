#include <enginemock.h>

#include "../common.h"
#include "blocks/motionblocks.h"

using namespace libscratchcpp;

class MotionBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<MotionBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
