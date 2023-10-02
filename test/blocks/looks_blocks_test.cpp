#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/looksblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<LooksBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any looks block
        std::shared_ptr<Block> createLooksBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = createLooksBlock(block->id() + "_menu", block->opcode() + "_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<LooksBlocks::Fields>(-1), selectedValue, static_cast<LooksBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Fields id, const std::string &value, LooksBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
            block->updateFieldMap();
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(LooksBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Looks");
}

TEST_F(LooksBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(LooksBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_show", &LooksBlocks::compileShow));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(LooksBlocksTest, Show)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_show");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::show)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileShow(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, ShowImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::show };

    Sprite sprite;
    sprite.setVisible(false);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());
}
