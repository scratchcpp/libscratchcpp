#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/list.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/listblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

class ListBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<ListBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any list block
        std::shared_ptr<Block> createListBlock(const std::string &id, const std::string &opcode, std::shared_ptr<List> list) const
        {
            auto block = std::make_shared<Block>(id, opcode);

            auto listField = std::make_shared<Field>("LIST", Value(), list);
            listField->setFieldId(ListBlocks::LIST);
            block->addField(listField);
            block->updateFieldMap();

            return block;
        }

        void addItemInput(std::shared_ptr<Block> block, const Value &item) const
        {
            auto input = std::make_shared<Input>("ITEM", Input::Type::Shadow);
            input->setPrimaryValue(item);
            input->setInputId(ListBlocks::ITEM);
            block->addInput(input);
            block->updateInputMap();
        }

        void addIndexInput(std::shared_ptr<Block> block, const Value &index) const
        {
            auto input = std::make_shared<Input>("INDEX", Input::Type::Shadow);
            input->setPrimaryValue(index);
            input->setInputId(ListBlocks::INDEX);
            block->addInput(input);
            block->updateInputMap();
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

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(ListBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Lists");
}

TEST_F(ListBlocksTest, CategoryVisible)
{
    ASSERT_FALSE(m_section->categoryVisible());
}

TEST_F(ListBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_addtolist", &ListBlocks::compileAddToList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_deleteoflist", &ListBlocks::compileDeleteFromList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_deletealloflist", &ListBlocks::compileDeleteAllOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_insertatlist", &ListBlocks::compileInsertToList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_replaceitemoflist", &ListBlocks::compileReplaceItemOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_itemoflist", &ListBlocks::compileItemOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_itemnumoflist", &ListBlocks::compileItemNumberInList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_lengthoflist", &ListBlocks::compileLengthOfList)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_listcontainsitem", &ListBlocks::compileListContainsItem)).Times(1);

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "ITEM", ListBlocks::ITEM));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "INDEX", ListBlocks::INDEX));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "LIST", ListBlocks::LIST));

    m_section->registerBlocks(&m_engineMock);
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
    auto block2 = createListItemBlock("c", "data_listcontainsitem", list2, Value::SpecialValue::NegativeInfinity);

    compiler.init();
    compiler.setBlock(block1);
    ListBlocks::compileListContainsItem(&compiler);
    compiler.setBlock(block2);
    ListBlocks::compileListContainsItem(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_LIST_CONTAINS, 0, vm::OP_CONST, 1, vm::OP_LIST_CONTAINS, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "hello world", Value(Value::SpecialValue::NegativeInfinity) }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_EQ(
        compiler.lists(),
        std::vector<List *>({
            list1.get(),
            list2.get(),
        }));
}
