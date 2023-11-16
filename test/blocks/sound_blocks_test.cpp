#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/target.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/soundblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class SoundBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<SoundBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = std::make_shared<Block>(block->id() + "_menu", block->opcode() + "_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<SoundBlocks::Fields>(-1), selectedValue, static_cast<SoundBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, SoundBlocks::Fields id, const std::string &value, SoundBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        Engine m_engine;
        EngineMock m_engineMock;
};

TEST_F(SoundBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Sound");
}

TEST_F(SoundBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(SoundBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sound_changevolumeby", &SoundBlocks::compileChangeVolumeBy));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VOLUME", SoundBlocks::VOLUME));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(SoundBlocksTest, ChangeVolumeBy)
{
    Compiler compiler(&m_engineMock);

    // change volume by (53.05)
    auto block1 = std::make_shared<Block>("a", "sound_changevolumeby");
    addValueInput(block1, "VOLUME", SoundBlocks::VOLUME, 53.05);

    // change volume by (-2.13)
    auto block2 = std::make_shared<Block>("b", "sound_changevolumeby");
    addValueInput(block2, "VOLUME", SoundBlocks::VOLUME, -2.13);

    EXPECT_CALL(m_engineMock, functionIndex(&SoundBlocks::changeVolumeBy)).Times(2).WillRepeatedly(Return(0));

    compiler.init();

    compiler.setBlock(block1);
    SoundBlocks::compileChangeVolumeBy(&compiler);

    compiler.setBlock(block2);
    SoundBlocks::compileChangeVolumeBy(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 53.05, -2.13 }));
}

TEST_F(SoundBlocksTest, ChangeVolumeByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SoundBlocks::changeVolumeBy };
    static Value constValues[] = { 53.05, -2.13 };

    Target target;
    target.setVolume(42.4);

    VirtualMachine vm(&target, nullptr, nullptr);

    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.volume() * 100) / 100, 95.45);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(target.volume() * 100) / 100, 93.32);
}
