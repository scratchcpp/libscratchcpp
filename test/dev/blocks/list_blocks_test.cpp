#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/dev/executablecode.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "../common.h"
#include "util.h"
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
            registerBlocks(m_engine, m_extension.get());
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

TEST_F(ListBlocksTest, InsertAtList)
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

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &index, const Value &item, std::shared_ptr<List> list) {
        builder.addBlock("data_insertatlist");
        builder.addValueInput("ITEM", item);
        builder.addValueInput("INDEX", index);
        builder.addEntityField("LIST", list);
        return builder.currentBlock();
    };

    auto block = addTest(4, "sit", list1);
    addTest(7, false, list1);
    addTest(0, "test", list1);
    addTest(9, "test", list1);

    addTest("last", "lorem", list2);
    addTest("random", "ipsum", list2);
    addTest("any", "dolor", list2);

    builder.build();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    EXPECT_CALL(m_rng, randint(1, 8)).WillOnce(Return(8));
    EXPECT_CALL(m_rng, randint(1, 9)).WillOnce(Return(3));
    code->run(ctx.get());
    ASSERT_EQ(list1->toString(), "Lorem ipsum dolor sit 123 true false");
    ASSERT_EQ(list2->toString(), "Hello world dolor false -543.5 abc 52.4 lorem ipsum");
}

TEST_F(ListBlocksTest, ReplaceItemOfList)
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

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &index, const Value &item, std::shared_ptr<List> list) {
        builder.addBlock("data_replaceitemoflist");
        builder.addValueInput("INDEX", index);
        builder.addEntityField("LIST", list);
        builder.addValueInput("ITEM", item);
        return builder.currentBlock();
    };

    auto block = addTest(4, "sit", list1);
    addTest(5, -53.18, list1);
    addTest(0, "test", list1);
    addTest(6, "test", list1);

    addTest("last", "lorem", list2);
    addTest("random", "ipsum", list2);
    addTest("any", "dolor", list2);

    builder.build();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    EXPECT_CALL(m_rng, randint(1, 6)).WillOnce(Return(4)).WillOnce(Return(1));
    code->run(ctx.get());
    ASSERT_EQ(list1->toString(), "Lorem ipsum dolor sit -53.18");
    ASSERT_EQ(list2->toString(), "dolor world false ipsum abc lorem");
}

TEST_F(ListBlocksTest, ItemOfList)
{
    auto target = std::make_shared<Sprite>();

    auto list = std::make_shared<List>("list", "");
    list->append("Lorem");
    list->append("ipsum");
    list->append("dolor");
    list->append(123);
    list->append(true);
    target->addList(list);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &index, std::shared_ptr<List> list) {
        builder.addBlock("test_const_string");
        builder.addValueInput("STRING", index);
        auto valueBlock = builder.takeBlock();

        builder.addBlock("data_itemoflist");
        builder.addObscuredInput("INDEX", valueBlock);
        builder.addEntityField("LIST", list);
        auto block = builder.takeBlock();

        builder.addBlock("test_print");
        builder.addObscuredInput("STRING", block);
        return builder.currentBlock();
    };

    auto block = addTest(3, list);
    addTest(5, list);
    addTest(0, list);
    addTest(6, list);

    addTest("last", list);
    addTest("random", list);
    addTest("any", list);

    builder.build();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    static const std::string expected =
        "dolor\n"
        "true\n"
        "0\n"
        "0\n"
        "true\n"
        "123\n"
        "Lorem\n";

    EXPECT_CALL(m_rng, randint(1, 5)).WillOnce(Return(4)).WillOnce(Return(1));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_EQ(list->toString(), "Lorem ipsum dolor 123 true");
}
