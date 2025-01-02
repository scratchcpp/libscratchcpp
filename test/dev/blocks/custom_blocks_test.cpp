#include <scratchcpp/project.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/customblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class CustomBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<CustomBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(CustomBlocksTest, Definition)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("procedures_definition");
    auto block = builder.currentBlock();

    Compiler compiler(&m_engineMock, target.get());
    compiler.compile(block);
    ASSERT_TRUE(compiler.unsupportedBlocks().empty());
}
