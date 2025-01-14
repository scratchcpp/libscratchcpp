#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/customblocks.h"
#include "util.h"

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
            registerBlocks(m_engine, m_extension.get());
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

TEST_F(CustomBlocksTest, CallWithArguments)
{
    const std::string procCode = "procedure %s %b";
    std::vector<std::string> argumentIds = { "a", "b" };
    std::vector<std::string> argumentNames = { "string or number", "boolean" };
    auto target = std::make_shared<Sprite>();

    // Create definition
    ScriptBuilder builder1(m_extension.get(), m_engine, target, false);
    builder1.addBlock("procedures_prototype");
    auto prototypeBlock = builder1.takeBlock();
    BlockPrototype *prototype = prototypeBlock->mutationPrototype();
    prototype->setProcCode(procCode);
    prototype->setArgumentIds(argumentIds);
    prototype->setArgumentNames(argumentNames);

    builder1.addBlock("procedures_definition");
    builder1.addObscuredInput("custom_block", prototypeBlock);
    builder1.currentBlock();

    // Print first arg
    builder1.addBlock("argument_reporter_string_number");
    builder1.addDropdownField("VALUE", "string or number");
    auto argBlock = builder1.takeBlock();

    builder1.addBlock("test_print");
    builder1.addObscuredInput("STRING", argBlock);

    // Print second arg
    builder1.addBlock("argument_reporter_boolean");
    builder1.addDropdownField("VALUE", "boolean");
    argBlock = builder1.takeBlock();

    builder1.addBlock("test_print");
    builder1.addObscuredInput("STRING", argBlock);

    builder1.build();

    // Call the procedure
    ScriptBuilder builder2(m_extension.get(), m_engine, target);
    auto block = std::make_shared<Block>("", "procedures_call");
    prototype = block->mutationPrototype();
    prototype->setProcCode(procCode);
    prototype->setArgumentIds(argumentIds);
    prototype->setArgumentNames(argumentNames);
    builder2.addBlock(block);
    builder2.addValueInput("a", "Hello world");
    builder2.addValueInput("b", true);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    testing::internal::CaptureStdout();
    builder2.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Hello world\ntrue\n");
}
