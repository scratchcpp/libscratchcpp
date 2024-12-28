#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/dev/executablecode.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "../common.h"
#include "dev/blocks/listblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

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
        RandomGeneratorMock m_rng;
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

TEST_F(ListBlocksTest, DeleteOfList)
{
    auto target = std::make_shared<Sprite>();

    auto list1 = std::make_shared<List>("", "");
    list1->append("Lorem");
    list1->append("ipsum");
    list1->append("dolor");
    list1->append(123);
    list1->append(true);
    target->addList(list1);

    auto list2 = std::make_shared<List>("", "");
    list2->append("Hello");
    list2->append("world");
    list2->append(false);
    list2->append(-543.5);
    list2->append("abc");
    list2->append(52.4);
    target->addList(list2);

    auto list3 = std::make_shared<List>("", "");
    list3->append(1);
    list3->append(2);
    list3->append(3);
    target->addList(list3);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &index, std::shared_ptr<List> list) {
        builder.addBlock("data_deleteoflist");
        builder.addValueInput("INDEX", index);
        builder.addEntityField("LIST", list);
        return builder.currentBlock();
    };

    auto block = addTest(1, list1);
    addTest(3, list1);
    addTest(2, list1);
    addTest(0, list1);
    addTest(3, list1);

    addTest("last", list2);
    addTest("random", list2);
    addTest("any", list2);

    addTest("all", list3);

    builder.build();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    EXPECT_CALL(m_rng, randint(1, 5)).WillOnce(Return(2));
    EXPECT_CALL(m_rng, randint(1, 4)).WillOnce(Return(3));
    code->run(ctx.get());
    ASSERT_EQ(list1->toString(), "ipsum true");
    ASSERT_EQ(list2->toString(), "Hello false abc");
    ASSERT_TRUE(list3->empty());
}

TEST_F(ListBlocksTest, DeleteAllOfList)
{
    auto target = std::make_shared<Sprite>();

    auto list = std::make_shared<List>("", "");
    list->append("Lorem");
    list->append("ipsum");
    list->append("dolor");
    list->append(123);
    list->append(true);
    target->addList(list);

    ScriptBuilder builder(m_extension.get(), m_engine, target);
    builder.addBlock("data_deletealloflist");
    builder.addEntityField("LIST", list);
    builder.build();

    builder.run();
    ASSERT_TRUE(list->empty());
}
