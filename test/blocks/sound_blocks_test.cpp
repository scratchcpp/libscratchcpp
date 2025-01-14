#include <enginemock.h>

#include "../common.h"
#include "blocks/soundblocks.h"

using namespace libscratchcpp;

class SoundBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<SoundBlocks>(); }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};
