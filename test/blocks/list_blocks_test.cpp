#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/monitor.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/listblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class ListBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<ListBlocks>();
            m_extension->registerBlocks(&m_engine);
        }

        // For any list block
        std::shared_ptr<Block> createListBlock(const std::string &id, const std::string &opcode, std::shared_ptr<List> list) const
        {
            auto block = std::make_shared<Block>(id, opcode);

            auto listField = std::make_shared<Field>("LIST", Value(), list);
            listField->setFieldId(ListBlocks::LIST);
            block->addField(listField);

            return block;
        }

        void addItemInput(std::shared_ptr<Block> block, const Value &item) const
        {
            auto input = std::make_shared<Input>("ITEM", Input::Type::Shadow);
            input->setPrimaryValue(item);
            input->setInputId(ListBlocks::ITEM);
            block->addInput(input);
        }

        void addIndexInput(std::shared_ptr<Block> block, const Value &index) const
        {
            auto input = std::make_shared<Input>("INDEX", Input::Type::Shadow);
            input->setPrimaryValue(index);
            input->setInputId(ListBlocks::INDEX);
            block->addInput(input);
        }

        // For add item, item index and list contains item
        std::shared_ptr<Block> createListItemBlock(const std::string &id, const std::string &opcode, std::shared_ptr<List> list, const Value &item) const
        {
            auto block = createListBlock(id, opcode, list);
            addItemInput(block, item);

            return block;
        }

        // For delete index and item of list
        std::shared_ptr<Block> createListIndexBlock(const std::string &id, const std::string &opcode, std::shared_ptr<List> list, const Value &index) const
        {
            auto block = createListBlock(id, opcode, list);
            addIndexInput(block, index);

            return block;
        }

        // For insert and replace
        std::shared_ptr<Block> createListItemIndexBlock(const std::string &id, const std::string &opcode, std::shared_ptr<List> list, const Value &item, const Value &index) const
        {
            auto block = createListBlock(id, opcode, list);
            addItemInput(block, item);
            addIndexInput(block, index);

            return block;
        }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(ListBlocksTest, Name)
{
    ASSERT_EQ(m_extension->name(), "Lists");
}

TEST_F(ListBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_listcontents", &ListBlocks::compileListContents)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_addtolist", &ListBlocks::compileAddToList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_deleteoflist", &ListBlocks::compileDeleteFromList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_deletealloflist", &ListBlocks::compileDeleteAllOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_insertatlist", &ListBlocks::compileInsertToList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_replaceitemoflist", &ListBlocks::compileReplaceItemOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_itemoflist", &ListBlocks::compileItemOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_itemnumoflist", &ListBlocks::compileItemNumberInList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_lengthoflist", &ListBlocks::compileLengthOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_listcontainsitem", &ListBlocks::compileListContainsItem)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_showlist", &ListBlocks::compileShowList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "data_hidelist", &ListBlocks::compileHideList)).Times(1);

    // Monitor names
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_extension.get(), "data_listcontents", &ListBlocks::listContentsMonitorName));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "ITEM", ListBlocks::ITEM));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "INDEX", ListBlocks::INDEX));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_extension.get(), "LIST", ListBlocks::LIST));

    m_extension->registerBlocks(&m_engineMock);
}

TEST_F(ListBlocksTest, ListContents)
{
    Compiler compiler(&m_engine);

    // [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListBlock("a", "data_listcontents", list1);

    // [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListBlock("c", "data_listcontents", list2);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileListContents(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileListContents(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ListContentsMonitorName)
{
    // [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListBlock("a", "data_listcontents", list1);

    // [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListBlock("c", "data_listcontents", list2);

    ASSERT_EQ(ListBlocks::listContentsMonitorName(block1.get()), "list1");
    ASSERT_EQ(ListBlocks::listContentsMonitorName(block2.get()), "list2");
}

TEST_F(ListBlocksTest, AddToList)
{
    Compiler compiler(&m_engine);

    // add "test" to [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListItemBlock("a", "data_addtolist", list1, "test");

    // add 10.25 to [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListItemBlock("c", "data_addtolist", list2, 10.25);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileAddToList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileAddToList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_APPEND, 0, vm::OP_CONST, 1, vm::OP_LIST_APPEND, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test", 10.25 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, DeleteOfList)
{
    Compiler compiler(&m_engine);

    // delete 2 of [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListIndexBlock("a", "data_deleteoflist", list1, 2);

    // delete "last" of [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListIndexBlock("c", "data_deleteoflist", list2, "last");

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileDeleteFromList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileDeleteFromList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_DEL, 0, vm::OP_CONST, 1, vm::OP_LIST_DEL, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, "last" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, DeleteAllOfList)
{
    Compiler compiler(&m_engine);

    // delete all of [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListBlock("a", "data_deletealloflist", list1);

    // delete all of [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListBlock("c", "data_deletealloflist", list2);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileDeleteAllOfList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileDeleteAllOfList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_LIST_DEL_ALL, 0, vm::OP_LIST_DEL_ALL, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, InsertAtList)
{
    Compiler compiler(&m_engine);

    // insert "test" at 3 of [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListItemIndexBlock("a", "data_insertatlist", list1, "test", 3);

    // insert 5.35 at "random" of [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListItemIndexBlock("c", "data_insertatlist", list2, 5.35, "random");

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileInsertToList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileInsertToList(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_LIST_INSERT, 0, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_LIST_INSERT, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test", 3, 5.35, "random" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ReplaceItemOfList)
{
    Compiler compiler(&m_engine);

    // replace item 1 of [list1] with "test"
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListItemIndexBlock("a", "data_replaceitemoflist", list1, "test", 1);

    // replace item "last" of [list2] with 100
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListItemIndexBlock("c", "data_replaceitemoflist", list2, 100, "last");

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileReplaceItemOfList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileReplaceItemOfList(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_LIST_REPLACE, 0, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_LIST_REPLACE, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, "test", "last", 100 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ItemOfList)
{
    Compiler compiler(&m_engine);

    // item 50 of [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListIndexBlock("a", "data_itemoflist", list1, 50);

    // item "random" of [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListIndexBlock("c", "data_itemoflist", list2, "random");

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileItemOfList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileItemOfList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_GET_ITEM, 0, vm::OP_CONST, 1, vm::OP_LIST_GET_ITEM, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 50, "random" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ItemNumOfList)
{
    Compiler compiler(&m_engine);

    // item # of "test" in [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListItemBlock("a", "data_itemnumoflist", list1, "test");

    // item # of 53.154 in [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListItemBlock("c", "data_itemnumoflist", list2, 53.154);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileItemNumberInList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileItemNumberInList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_INDEX_OF, 0, vm::OP_CONST, 1, vm::OP_LIST_INDEX_OF, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test", 53.154 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, LengthOfList)
{
    Compiler compiler(&m_engine);

    // length of [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListBlock("a", "data_lengthoflist", list1);

    // length of [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListBlock("c", "data_lengthoflist", list2);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileLengthOfList(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileLengthOfList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_LIST_LENGTH, 0, vm::OP_LIST_LENGTH, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ListContainsItem)
{
    Compiler compiler(&m_engine);

    // [list1] contains "hello world"
    auto list1 = std::make_shared<List>("b", "list1");
    auto block1 = createListItemBlock("a", "data_listcontainsitem", list1, "hello world");

    // [list2] contains -Infinity
    auto list2 = std::make_shared<List>("d", "list2");
    auto block2 = createListItemBlock("c", "data_listcontainsitem", list2, SpecialValue::NegativeInfinity);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileListContainsItem(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileListContainsItem(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_CONTAINS, 0, vm::OP_CONST, 1, vm::OP_LIST_CONTAINS, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "hello world", Value(SpecialValue::NegativeInfinity) }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}

TEST_F(ListBlocksTest, ShowList)
{
    Compiler compiler(&m_engineMock);
    Stage stage;
    Target target;

    // show list [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    list1->setTarget(&stage);
    auto block1 = createListBlock("a", "data_showlist", list1);

    // show list [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    list2->setTarget(&target);
    auto block2 = createListBlock("c", "data_showlist", list2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&ListBlocks::showGlobalList)).WillOnce(Return(0));
    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileShowList(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&ListBlocks::showList)).WillOnce(Return(1));
    compiler.setBlock(block2);
    ListBlocks::compileShowList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "b", "d" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ListBlocksTest, ShowListImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &ListBlocks::showGlobalList, &ListBlocks::showList };
    static Value constValues[] = { "a", "b", "c", "d" };

    auto list1 = std::make_shared<List>("b", "");
    Monitor monitor1("b", "");
    monitor1.setVisible(false);
    list1->setMonitor(&monitor1);

    auto list2 = std::make_shared<List>("d", "");
    Monitor monitor2("d", "");
    monitor2.setVisible(false);
    list2->setMonitor(&monitor2);

    Stage stage;
    stage.addList(list1);

    Sprite sprite;
    sprite.addList(list2);

    Engine fakeEngine;
    sprite.setEngine(&fakeEngine);
    auto clone = sprite.clone();

    // Global
    VirtualMachine vm1(&stage, &m_engineMock, nullptr);
    vm1.setBytecode(bytecode1);
    vm1.setFunctions(functions);
    vm1.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.reset();
    vm1.setBytecode(bytecode2);
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    monitor1.setVisible(false);

    // Local
    VirtualMachine vm2(&sprite, &m_engineMock, nullptr);
    vm2.setBytecode(bytecode3);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    vm2.reset();
    vm2.setBytecode(bytecode4);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    // Local - clone
    monitor2.setVisible(false);
    VirtualMachine vm3(clone.get(), &m_engineMock, nullptr);
    vm3.setBytecode(bytecode3);
    vm3.setFunctions(functions);
    vm3.setConstValues(constValues);
    vm3.run();

    ASSERT_EQ(vm3.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    vm3.reset();
    vm3.setBytecode(bytecode4);
    vm3.run();

    ASSERT_EQ(vm3.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());
}

TEST_F(ListBlocksTest, HideList)
{
    Compiler compiler(&m_engineMock);
    Stage stage;
    Target target;

    // hide list [list1]
    auto list1 = std::make_shared<List>("b", "list1");
    list1->setTarget(&stage);
    auto block1 = createListBlock("a", "data_hidelist", list1);

    // hide list [list2]
    auto list2 = std::make_shared<List>("d", "list2");
    list2->setTarget(&target);
    auto block2 = createListBlock("c", "data_hidelist", list2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&ListBlocks::hideGlobalList)).WillOnce(Return(0));
    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileHideList(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&ListBlocks::hideList)).WillOnce(Return(1));
    compiler.setBlock(block2);
    ListBlocks::compileHideList(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "b", "d" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ListBlocksTest, HideListImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &ListBlocks::hideGlobalList, &ListBlocks::hideList };
    static Value constValues[] = { "a", "b", "c", "d" };

    auto list1 = std::make_shared<List>("b", "");
    Monitor monitor1("b", "");
    monitor1.setVisible(true);
    list1->setMonitor(&monitor1);

    auto list2 = std::make_shared<List>("d", "");
    Monitor monitor2("d", "");
    monitor2.setVisible(true);
    list2->setMonitor(&monitor2);

    Stage stage;
    stage.addList(list1);

    Sprite sprite;
    sprite.addList(list2);

    Engine fakeEngine;
    sprite.setEngine(&fakeEngine);
    auto clone = sprite.clone();

    // Global
    VirtualMachine vm1(&stage, &m_engineMock, nullptr);
    vm1.setBytecode(bytecode1);
    vm1.setFunctions(functions);
    vm1.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.reset();
    vm1.setBytecode(bytecode2);
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    monitor1.setVisible(true);

    // Local
    VirtualMachine vm2(&sprite, &m_engineMock, nullptr);
    vm2.setBytecode(bytecode3);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    vm2.reset();
    vm2.setBytecode(bytecode4);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    // Local - clone
    monitor2.setVisible(true);
    VirtualMachine vm3(clone.get(), &m_engineMock, nullptr);
    vm3.setBytecode(bytecode3);
    vm3.setFunctions(functions);
    vm3.setConstValues(constValues);
    vm3.run();

    ASSERT_EQ(vm3.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    vm3.reset();
    vm3.setBytecode(bytecode4);
    vm3.run();

    ASSERT_EQ(vm3.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());
}
