#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/motionblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class MotionBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<MotionBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any motion block
        std::shared_ptr<Block> createMotionBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, MotionBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(MotionBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Motion");
}

TEST_F(MotionBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(MotionBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_movesteps", &MotionBlocks::compileMoveSteps));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_turnright", &MotionBlocks::compileTurnRight));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "STEPS", MotionBlocks::STEPS));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DEGREES", MotionBlocks::DEGREES));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(MotionBlocksTest, MoveSteps)
{
    Compiler compiler(&m_engineMock);

    // move (30.25) steps
    auto block = std::make_shared<Block>("a", "motion_movesteps");
    addValueInput(block, "STEPS", MotionBlocks::STEPS, 30.25);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::moveSteps)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileMoveSteps(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 30.25);
}

TEST_F(MotionBlocksTest, MoveStepsImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::moveSteps };
    static Value constValues[] = { 30.25 };

    Sprite sprite;
    sprite.setX(5.2);
    sprite.setY(-0.25);
    sprite.setDirection(-61.42);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, -21.36);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, 14.22);
}

TEST_F(MotionBlocksTest, TurnRight)
{
    Compiler compiler(&m_engineMock);

    // turn right (12.05) degrees
    auto block = std::make_shared<Block>("a", "motion_turnright");
    addValueInput(block, "DEGREES", MotionBlocks::DEGREES, 12.05);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::turnRight)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileTurnRight(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 12.05);
}

TEST_F(MotionBlocksTest, TurnRightImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::turnRight };
    static Value constValues[] = { 12.05 };

    Sprite sprite;
    sprite.setDirection(124.37);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(sprite.direction() * 100) / 100, 136.42);
}
