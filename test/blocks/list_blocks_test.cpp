#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/executablecode.h>
#include <enginemock.h>
#include <randomgeneratormock.h>
#include <monitorhandlermock.h>

#include "../common.h"
#include "util.h"
#include "blocks/listblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

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

TEST_F(ListBlocksTest, ListContents)
{
    auto target = std::make_shared<Sprite>();
    auto list1 = std::make_shared<List>("", "");
    target->addList(list1);
    auto list2 = std::make_shared<List>("", "");
    target->addList(list2);

    list1->append(1);
    list1->append(2);
    list1->append(4);

    list2->append("Lorem");
    list2->append("ipsum");
    list2->append("dolor");
    list2->append("sit");
    list2->append("amet");

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("data_listcontents");
    builder.addEntityField("LIST", list1);
    builder.captureBlockReturnValue();

    builder.addBlock("data_listcontents");
    builder.addEntityField("LIST", list2);
    builder.captureBlockReturnValue();

    builder.build();

    builder.run();
    List *list = builder.capturedValues();
    ValueData *data = list->data();
    ASSERT_EQ(list->size(), 2);
    ASSERT_EQ(Value(data[0]).toString(), "124");
    ASSERT_EQ(Value(data[1]).toString(), "Lorem ipsum dolor sit amet");
}

TEST_F(ListBlocksTest, ListMonitor)
{
    auto target = std::make_shared<Sprite>();
    auto list1 = std::make_shared<List>("", "list1");
    target->addList(list1);
    auto list2 = std::make_shared<List>("", "list2");
    target->addList(list2);

    MonitorHandlerMock iface1, iface2;
    EXPECT_CALL(iface1, init);
    EXPECT_CALL(iface2, init);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_listcontents");
    auto monitor2 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor1->block()->setTarget(target.get());
    monitor1->setInterface(&iface1);
    monitor2->block()->setTarget(target.get());
    monitor2->setInterface(&iface2);
    m_engine->setMonitors({ monitor1, monitor2 });

    ScriptBuilder builder1(m_extension.get(), m_engine, target);
    builder1.addBlock(monitor1->block());
    builder1.addEntityField("LIST", list1);

    ScriptBuilder builder2(m_extension.get(), m_engine, target);
    builder2.addBlock(monitor2->block());
    builder2.addEntityField("LIST", list2);

    m_engine->compile();
    ASSERT_EQ(monitor1->name(), list1->name());
    ASSERT_EQ(monitor2->name(), list2->name());

    EXPECT_CALL(iface1, onValueChanged(_)).WillOnce(Invoke([list1](const Value &value) { ASSERT_EQ(value.toPointer(), list1.get()); }));
    EXPECT_CALL(iface2, onValueChanged(_)).WillOnce(Invoke([list2](const Value &value) { ASSERT_EQ(value.toPointer(), list2.get()); }));
    m_engine->updateMonitors();
}

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

    addTest("Last", list2);
    addTest("raNdom", list2);
    addTest("aNY", list2);

    addTest("aLl", list3);

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

    addTest("lAsT", "lorem", list2);
    addTest("raNDom", "ipsum", list2);
    addTest("Any", "dolor", list2);

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

    addTest("LasT", "lorem", list2);
    addTest("rAndOm", "ipsum", list2);
    addTest("AnY", "dolor", list2);

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

    addTest("laSt", list);
    addTest("RAndom", list);
    addTest("aNy", list);

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
        "Lorem\n"
        "0\n"
        "0\n"
        "0\n";

    EXPECT_CALL(m_rng, randint(1, 5)).WillOnce(Return(4)).WillOnce(Return(1));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_EQ(list->toString(), "Lorem ipsum dolor 123 true");
}

TEST_F(ListBlocksTest, ItemNumOfList)
{
    auto target = std::make_shared<Sprite>();

    auto list = std::make_shared<List>("list", "");
    list->append("Lorem");
    list->append("ipsum");
    list->append("dolor");
    list->append(123);
    list->append(true);
    list->append("dolor");
    target->addList(list);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &item, std::shared_ptr<List> list) {
        builder.addBlock("data_itemnumoflist");
        builder.addValueInput("ITEM", item);
        builder.addEntityField("LIST", list);
        auto block = builder.takeBlock();

        builder.addBlock("test_print");
        builder.addObscuredInput("STRING", block);
        return builder.currentBlock();
    };

    auto block = addTest("dolor", list);
    addTest(true, list);
    addTest("nonexistent", list);

    builder.build();

    static const std::string expected =
        "3\n"
        "5\n"
        "0\n";

    testing::internal::CaptureStdout();
    builder.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_EQ(list->toString(), "Lorem ipsum dolor 123 true dolor");
}

TEST_F(ListBlocksTest, LengthOfList)
{
    auto target = std::make_shared<Sprite>();

    auto list1 = std::make_shared<List>("list1", "");
    list1->append("Lorem");
    list1->append("ipsum");
    list1->append("dolor");
    list1->append(123);
    list1->append(true);
    target->addList(list1);

    auto list2 = std::make_shared<List>("list2", "");
    list2->append(1);
    list2->append(false);
    target->addList(list2);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](std::shared_ptr<List> list) {
        builder.addBlock("data_lengthoflist");
        builder.addEntityField("LIST", list);
        auto block = builder.takeBlock();

        builder.addBlock("test_print");
        builder.addObscuredInput("STRING", block);
        return builder.currentBlock();
    };

    auto block = addTest(list1);
    addTest(list2);

    builder.build();

    static const std::string expected =
        "5\n"
        "2\n";

    testing::internal::CaptureStdout();
    builder.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_EQ(list1->toString(), "Lorem ipsum dolor 123 true");
    ASSERT_EQ(list2->toString(), "1 false");
}

TEST_F(ListBlocksTest, ListContainsItem)
{
    auto target = std::make_shared<Sprite>();

    auto list = std::make_shared<List>("list", "");
    list->append("Lorem");
    list->append("ipsum");
    list->append("dolor");
    list->append(123);
    list->append(true);
    list->append("dolor");
    target->addList(list);

    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addTest = [&builder](const Value &item, std::shared_ptr<List> list) {
        builder.addBlock("data_listcontainsitem");
        builder.addEntityField("LIST", list);
        builder.addValueInput("ITEM", item);
        auto block = builder.takeBlock();

        builder.addBlock("test_print");
        builder.addObscuredInput("STRING", block);
        return builder.currentBlock();
    };

    auto block = addTest("dolor", list);
    addTest(true, list);
    addTest("nonexistent", list);

    builder.build();

    static const std::string expected =
        "true\n"
        "true\n"
        "false\n";

    testing::internal::CaptureStdout();
    builder.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_EQ(list->toString(), "Lorem ipsum dolor 123 true dolor");
}

TEST_F(ListBlocksTest, ShowList_Global_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto list1 = std::make_shared<List>("a", "list1");
    list1->append("item1");
    list1->append("item2");
    stage->addList(list1);
    auto list2 = std::make_shared<List>("b", "list2");
    list2->append("Hello");
    list2->append("world");
    stage->addList(list2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor1->block()->addField(std::make_shared<Field>("LIST", list1->name(), list1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor2->block()->addField(std::make_shared<Field>("LIST", list2->name(), list2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    list1->setMonitor(monitor1.get());
    list2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_showlist");
    builder1.addEntityField("LIST", list1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_showlist");
    builder2.addEntityField("LIST", list2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(ListBlocksTest, ShowList_Global_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto list1 = std::make_shared<List>("a", "list1");
    list1->append("item1");
    list1->append("item2");
    stage->addList(list1);
    auto list2 = std::make_shared<List>("b", "list2");
    list2->append("Hello");
    list2->append("world");
    stage->addList(list2);

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_showlist");
    builder1.addEntityField("LIST", list1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_showlist");
    builder2.addEntityField("LIST", list2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // Missing monitors should be created
    builder1.run();

    Monitor *monitor1 = list1->monitor();
    ASSERT_TRUE(monitor1);
    ASSERT_TRUE(monitor1->visible());

    builder2.run();

    Monitor *monitor2 = list2->monitor();

    ASSERT_TRUE(monitor2);
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(ListBlocksTest, ShowList_Local_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto list1 = std::make_shared<List>("a", "list1");
    list1->append("item1");
    list1->append("item2");
    sprite->addList(list1);
    auto list2 = std::make_shared<List>("b", "list2");
    list2->append("Hello");
    list2->append("world");
    sprite->addList(list2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor1->block()->addField(std::make_shared<Field>("LIST", list1->name(), list1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor2->block()->addField(std::make_shared<Field>("LIST", list2->name(), list2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    list1->setMonitor(monitor1.get());
    list2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_showlist");
    builder1.addEntityField("LIST", list1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_showlist");
    builder2.addEntityField("LIST", list2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(ListBlocksTest, ShowList_Local_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto list1 = std::make_shared<List>("a", "list1");
    list1->append("item1");
    list1->append("item2");
    sprite->addList(list1);
    auto list2 = std::make_shared<List>("b", "list2");
    list2->append("Hello");
    list2->append("world");
    sprite->addList(list2);

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_showlist");
    builder1.addEntityField("LIST", list1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_showlist");
    builder2.addEntityField("LIST", list2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });
    m_engine->run();

    // Missing monitors should be created
    builder1.run();

    Monitor *monitor1 = list1->monitor();
    ASSERT_TRUE(monitor1);
    ASSERT_TRUE(monitor1->visible());

    builder2.run();

    Monitor *monitor2 = list2->monitor();

    ASSERT_TRUE(monitor2);
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(ListBlocksTest, ShowList_Local_FromClone)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto list1 = std::make_shared<List>("a", "list1");
    list1->append("item1");
    list1->append("item2");
    sprite->addList(list1);
    auto list2 = std::make_shared<List>("b", "list2");
    list2->append("Hello");
    list2->append("world");
    sprite->addList(list2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor1->block()->addField(std::make_shared<Field>("LIST", list1->name(), list1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_listcontents");
    monitor2->block()->addField(std::make_shared<Field>("LIST", list2->name(), list2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    list1->setMonitor(monitor1.get());
    list2->setMonitor(monitor2.get());

    sprite->setEngine(m_engine);
    auto clone = sprite->clone();

    m_engine->setTargets({ stage, sprite, clone });

    ScriptBuilder builder1(m_extension.get(), m_engine, clone);
    builder1.addBlock("data_showlist");
    builder1.addEntityField("LIST", list1);
    Block *hat1 = builder1.currentBlock()->parent();

    ScriptBuilder builder2(m_extension.get(), m_engine, clone);
    builder2.addBlock("data_showlist");
    builder2.addEntityField("LIST", list2);
    Block *hat2 = builder2.currentBlock()->parent();

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // The clone root lists should be used
    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());

    ASSERT_FALSE(clone->listAt(0)->monitor());
    ASSERT_FALSE(clone->listAt(1)->monitor());
}
