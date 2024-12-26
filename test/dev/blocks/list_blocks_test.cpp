#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/listblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class ListBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<ListBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(ListBlocksTest, AddToList)
{
    auto target = std::make_shared<Sprite>();
    auto list1 = std::make_shared<List>("", "");
    target->addList(list1);
    auto list2 = std::make_shared<List>("", "");
    target->addList(list2);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("data_addtolist");
    builder.addValueInput("ITEM", "test");
    builder.addEntityField("LIST", list1);

    builder.addBlock("data_addtolist");
    builder.addValueInput("ITEM", true);
    builder.addEntityField("LIST", list1);

    builder.addBlock("data_addtolist");
    builder.addValueInput("ITEM", 123);
    builder.addEntityField("LIST", list2);

    builder.addBlock("data_addtolist");
    builder.addValueInput("ITEM", "Hello world");
    builder.addEntityField("LIST", list2);

    builder.build();

    builder.run();
    ASSERT_EQ(list1->toString(), "test true");
    ASSERT_EQ(list2->toString(), "123 Hello world");
}
