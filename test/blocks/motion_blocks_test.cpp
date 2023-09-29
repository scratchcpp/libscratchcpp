#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>
#include <randomgeneratormock.h>
#include <clockmock.h>

#include "../common.h"
#include "blocks/motionblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"
#include "engine/internal/randomgenerator.h"
#include "engine/internal/clock.h"

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

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, MotionBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, MotionBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, MotionBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = createMotionBlock(block->id() + "_menu", block->opcode() + "_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, "CLONE_OPTION", static_cast<MotionBlocks::Fields>(-1), selectedValue, static_cast<MotionBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, MotionBlocks::Fields id, const std::string &value, MotionBlocks::FieldValues valueId) const
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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_turnleft", &MotionBlocks::compileTurnLeft));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_pointindirection", &MotionBlocks::compilePointInDirection));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_pointtowards", &MotionBlocks::compilePointTowards));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_gotoxy", &MotionBlocks::compileGoToXY));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_goto", &MotionBlocks::compileGoTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_glidesecstoxy", &MotionBlocks::compileGlideSecsToXY));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_glideto", &MotionBlocks::compileGlideTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_changexby", &MotionBlocks::compileChangeXBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_setx", &MotionBlocks::compileSetX));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_changeyby", &MotionBlocks::compileChangeYBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_sety", &MotionBlocks::compileSetY));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_setrotationstyle", &MotionBlocks::compileSetRotationStyle));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_xposition", &MotionBlocks::compileXPosition));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_yposition", &MotionBlocks::compileYPosition));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "motion_direction", &MotionBlocks::compileDirection));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "STEPS", MotionBlocks::STEPS));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DEGREES", MotionBlocks::DEGREES));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DIRECTION", MotionBlocks::DIRECTION));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "TOWARDS", MotionBlocks::TOWARDS));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "X", MotionBlocks::X));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "Y", MotionBlocks::Y));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "TO", MotionBlocks::TO));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SECS", MotionBlocks::SECS));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DX", MotionBlocks::DX));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DY", MotionBlocks::DY));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "STYLE", MotionBlocks::STYLE));

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

TEST_F(MotionBlocksTest, TurnLeft)
{
    Compiler compiler(&m_engineMock);

    // turn left (12.05) degrees
    auto block = std::make_shared<Block>("a", "motion_turnleft");
    addValueInput(block, "DEGREES", MotionBlocks::DEGREES, 12.05);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::turnLeft)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileTurnLeft(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 12.05);
}

TEST_F(MotionBlocksTest, TurnLeftImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::turnLeft };
    static Value constValues[] = { 12.05 };

    Sprite sprite;
    sprite.setDirection(124.37);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(sprite.direction() * 100) / 100, 112.32);
}

TEST_F(MotionBlocksTest, PointInDirection)
{
    Compiler compiler(&m_engineMock);

    // point in direction (-60.5)
    auto block = std::make_shared<Block>("a", "motion_pointindirection");
    addValueInput(block, "DIRECTION", MotionBlocks::DIRECTION, -60.5);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::pointInDirection)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compilePointInDirection(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), -60.5);
}

TEST_F(MotionBlocksTest, PointInDirectionImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::pointInDirection };
    static Value constValues[] = { -60.5 };

    Sprite sprite;
    sprite.setDirection(50.02);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.direction(), -60.5);
}

TEST_F(MotionBlocksTest, PointTowards)
{
    Compiler compiler(&m_engineMock);

    // point towards (mouse-pointer)
    auto block1 = std::make_shared<Block>("a", "motion_pointtowards");
    addDropdownInput(block1, "TOWARDS", MotionBlocks::TOWARDS, "_mouse_");

    // point towards (random position)
    auto block2 = std::make_shared<Block>("b", "motion_pointtowards");
    addDropdownInput(block2, "TOWARDS", MotionBlocks::TOWARDS, "_random_");

    // point towards (Sprite2)
    auto block3 = std::make_shared<Block>("c", "motion_pointtowards");
    addDropdownInput(block3, "TOWARDS", MotionBlocks::TOWARDS, "Sprite2");

    // point towards (join "" "")
    auto joinBlock = std::make_shared<Block>("e", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block4 = std::make_shared<Block>("d", "motion_pointtowards");
    addDropdownInput(block4, "TOWARDS", MotionBlocks::TOWARDS, "", joinBlock);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::pointTowardsMousePointer)).WillOnce(Return(0));
    compiler.setBlock(block1);
    MotionBlocks::compilePointTowards(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::pointTowardsRandomPosition)).WillOnce(Return(1));
    compiler.setBlock(block2);
    MotionBlocks::compilePointTowards(&compiler);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(5));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::pointTowardsByIndex)).WillOnce(Return(2));
    compiler.setBlock(block3);
    MotionBlocks::compilePointTowards(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::pointTowards)).WillOnce(Return(3));
    compiler.setBlock(block4);
    MotionBlocks::compilePointTowards(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_NULL, vm::OP_STR_CONCAT, vm::OP_EXEC, 3, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
}

TEST_F(MotionBlocksTest, PointTowardsImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_EXEC, 3, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::pointTowards, &MotionBlocks::pointTowardsByIndex, &MotionBlocks::pointTowardsMousePointer, &MotionBlocks::pointTowardsRandomPosition };
    static Value constValues[] = { "_mouse_", "_random_", "Sprite2", 3 };

    Sprite sprite, anotherSprite;
    sprite.setX(70.1);
    sprite.setY(-100.025);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    RandomGeneratorMock rng;
    MotionBlocks::rng = &rng;

    static const std::vector<std::pair<double, double>> positions = { { -45.12, -123.48 }, { 125.23, -3.21 }, { 30.15, -100.025 }, { 70.1, -100.025 }, { 150.9, -100.025 } };
    static const std::vector<double> results = { -101.51, 29.66, -90, 90, 90 };
    static const std::vector<double> intPosResults = { -101.31, 29.55, -90, 90, 90 };

    // point towards (join "_mouse_" "")
    for (int i = 0; i < positions.size(); i++) {
        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(positions[i].first));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(positions[i].second));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode1);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, results[i]);
    }

    // point towards (join "_random_" "")
    sprite.setX(std::round(sprite.x()));
    sprite.setY(std::round(sprite.y()));

    for (int i = 0; i < positions.size(); i++) {
        EXPECT_CALL(rng, randint(-240, 240)).WillOnce(Return(std::round(positions[i].first)));
        EXPECT_CALL(rng, randint(-180, 180)).WillOnce(Return(std::round(positions[i].second)));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode2);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, intPosResults[i]);
    }

    sprite.setX(70.1);
    sprite.setY(-100.025);

    // point towards (join "Sprite2" "")
    for (int i = 0; i < positions.size(); i++) {
        anotherSprite.setX(positions[i].first);
        anotherSprite.setY(positions[i].second);

        EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
        EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode3);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, results[i]);
    }

    // point towards (Sprite2)
    for (int i = 0; i < positions.size(); i++) {
        anotherSprite.setX(positions[i].first);
        anotherSprite.setY(positions[i].second);

        EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode4);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, results[i]);
    }

    // point towards (mouse-pointer)
    for (int i = 0; i < positions.size(); i++) {
        EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(positions[i].first));
        EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(positions[i].second));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode5);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, results[i]);
    }

    // point towards (random position)
    sprite.setX(std::round(sprite.x()));
    sprite.setY(std::round(sprite.y()));

    for (int i = 0; i < positions.size(); i++) {
        EXPECT_CALL(rng, randint(-240, 240)).WillOnce(Return(std::round(positions[i].first)));
        EXPECT_CALL(rng, randint(-180, 180)).WillOnce(Return(std::round(positions[i].second)));

        // TODO: Move setBytecode() out of the loop and use reset() after task #215 is completed
        vm.setBytecode(bytecode6);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(std::round(sprite.direction() * 100) / 100, intPosResults[i]);
    }

    MotionBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(MotionBlocksTest, GoToXY)
{
    Compiler compiler(&m_engineMock);

    // go to x: (95.2) y: (-175.9)
    auto block = std::make_shared<Block>("a", "motion_gotoxy");
    addValueInput(block, "X", MotionBlocks::X, 95.2);
    addValueInput(block, "Y", MotionBlocks::Y, -175.9);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::goToXY)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileGoToXY(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 95.2, -175.9 }));
}

TEST_F(MotionBlocksTest, GoToXYImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::goToXY };
    static Value constValues[] = { 95.2, -175.9 };

    Sprite sprite;

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), 95.2);
    ASSERT_EQ(sprite.y(), -175.9);
}

TEST_F(MotionBlocksTest, GoTo)
{
    Compiler compiler(&m_engineMock);

    // go to (mouse-pointer)
    auto block1 = std::make_shared<Block>("a", "motion_goto");
    addDropdownInput(block1, "TO", MotionBlocks::TO, "_mouse_");

    // go to (random position)
    auto block2 = std::make_shared<Block>("b", "motion_goto");
    addDropdownInput(block2, "TO", MotionBlocks::TO, "_random_");

    // go to (Sprite2)
    auto block3 = std::make_shared<Block>("c", "motion_goto");
    addDropdownInput(block3, "TO", MotionBlocks::TO, "Sprite2");

    // go to (join "" "")
    auto joinBlock = std::make_shared<Block>("e", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block4 = std::make_shared<Block>("d", "motion_goto");
    addDropdownInput(block4, "TO", MotionBlocks::TO, "", joinBlock);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::goToMousePointer)).WillOnce(Return(0));
    compiler.setBlock(block1);
    MotionBlocks::compileGoTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::goToRandomPosition)).WillOnce(Return(1));
    compiler.setBlock(block2);
    MotionBlocks::compileGoTo(&compiler);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(5));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::goToByIndex)).WillOnce(Return(2));
    compiler.setBlock(block3);
    MotionBlocks::compileGoTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::goTo)).WillOnce(Return(3));
    compiler.setBlock(block4);
    MotionBlocks::compileGoTo(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_NULL, vm::OP_STR_CONCAT, vm::OP_EXEC, 3, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
}

TEST_F(MotionBlocksTest, GoToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_EXEC, 3, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::goTo, &MotionBlocks::goToByIndex, &MotionBlocks::goToMousePointer, &MotionBlocks::goToRandomPosition };
    static Value constValues[] = { "_mouse_", "_random_", "Sprite2", 3 };

    Sprite sprite;
    sprite.setX(70.1);
    sprite.setY(-100.025);

    Sprite anotherSprite;
    anotherSprite.setX(-195.25);
    anotherSprite.setY(148.02);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    RandomGeneratorMock rng;
    MotionBlocks::rng = &rng;

    // go to (join "_mouse_" "")
    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(70.56));
    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(45.2));

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), 70.56);
    ASSERT_EQ(sprite.y(), 45.2);

    // go to (join "_random_" "")
    EXPECT_CALL(rng, randint(-240, 240)).WillOnce(Return(-158));
    EXPECT_CALL(rng, randint(-180, 180)).WillOnce(Return(65));

    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), -158);
    ASSERT_EQ(sprite.y(), 65);

    // go to (join "Sprite2" "")
    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));

    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), anotherSprite.x());
    ASSERT_EQ(sprite.y(), anotherSprite.y());

    // go to (Sprite2)
    EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));

    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), anotherSprite.x());
    ASSERT_EQ(sprite.y(), anotherSprite.y());

    // go to (mouse-pointer)
    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-239.92));
    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-170.6));

    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), -239.92);
    ASSERT_EQ(sprite.y(), -170.6);

    // go to (random position)
    EXPECT_CALL(rng, randint(-240, 240)).WillOnce(Return(220));
    EXPECT_CALL(rng, randint(-180, 180)).WillOnce(Return(-16));

    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), 220);
    ASSERT_EQ(sprite.y(), -16);

    MotionBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(MotionBlocksTest, GlideSecsToXY)
{
    Compiler compiler(&m_engineMock);

    // glide (2.5) secs to x: (95.2) y: (-175.9)
    auto block = std::make_shared<Block>("a", "motion_gotoxy");
    addValueInput(block, "SECS", MotionBlocks::SECS, 2.5);
    addValueInput(block, "X", MotionBlocks::X, 95.2);
    addValueInput(block, "Y", MotionBlocks::Y, -175.9);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::startGlideSecsTo)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::glideSecsTo)).WillOnce(Return(1));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileGlideSecsToXY(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2.5, 95.2, -175.9 }));
}

TEST_F(MotionBlocksTest, GlideSecsToXYImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::startGlideSecsTo, &MotionBlocks::glideSecsTo };
    static Value constValues[] = { 2.5, 95.2, -175.9 };

    static const double startX = 100.32;
    static const double startY = -50.12;

    Sprite sprite;
    sprite.setX(startX);
    sprite.setY(startY);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    ClockMock clock;
    MotionBlocks::clock = &clock;

    std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(1000));
    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) != MotionBlocks::m_timeMap.cend());
    ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) != MotionBlocks::m_glideMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(sprite.x(), startX);
    ASSERT_EQ(sprite.y(), startY);

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(3456));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) != MotionBlocks::m_timeMap.cend());
    ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) != MotionBlocks::m_glideMap.cend());
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 95.29);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -173.69);

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(3500));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(0);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(0);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) == MotionBlocks::m_timeMap.cend());
    ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) == MotionBlocks::m_glideMap.cend());
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 95.2);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -175.9);

    MotionBlocks::clock = Clock::instance().get();
}

TEST_F(MotionBlocksTest, GlideTo)
{
    Compiler compiler(&m_engineMock);

    // glide (3.25) secs to (mouse-pointer)
    auto block1 = std::make_shared<Block>("a", "motion_glideto");
    addValueInput(block1, "SECS", MotionBlocks::SECS, 3.25);
    addDropdownInput(block1, "TO", MotionBlocks::TO, "_mouse_");

    // glide (2.5) secs to (random position)
    auto block2 = std::make_shared<Block>("b", "motion_glideto");
    addValueInput(block2, "SECS", MotionBlocks::SECS, 2.5);
    addDropdownInput(block2, "TO", MotionBlocks::TO, "_random_");

    // glide (5.01) secs to (Sprite2)
    auto block3 = std::make_shared<Block>("c", "motion_glideto");
    addValueInput(block3, "SECS", MotionBlocks::SECS, 3.25);
    addDropdownInput(block3, "TO", MotionBlocks::TO, "Sprite2");

    // glide (6.5) secs to (join "" "")
    auto joinBlock = std::make_shared<Block>("e", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block4 = std::make_shared<Block>("d", "motion_glideto");
    addValueInput(block4, "SECS", MotionBlocks::SECS, 6.5);
    addDropdownInput(block4, "TO", MotionBlocks::TO, "", joinBlock);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::startGlideToMousePointer)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::glideSecsTo)).WillOnce(Return(4));
    compiler.setBlock(block1);
    MotionBlocks::compileGlideTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::startGlideToRandomPosition)).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::glideSecsTo)).WillOnce(Return(4));
    compiler.setBlock(block2);
    MotionBlocks::compileGlideTo(&compiler);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(5));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::startGlideToByIndex)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::glideSecsTo)).WillOnce(Return(4));
    compiler.setBlock(block3);
    MotionBlocks::compileGlideTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::startGlideTo)).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::glideSecsTo)).WillOnce(Return(4));
    compiler.setBlock(block4);
    MotionBlocks::compileGlideTo(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              4,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              1,
              vm::OP_EXEC,
              4,
              vm::OP_CONST,
              2,
              vm::OP_CONST,
              3,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              4,
              vm::OP_CONST,
              4,
              vm::OP_NULL,
              vm::OP_NULL,
              vm::OP_STR_CONCAT,
              vm::OP_EXEC,
              3,
              vm::OP_EXEC,
              4,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 3.25, 2.5, 3.25, 5, 6.5 }));
}

TEST_F(MotionBlocksTest, GlideToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 3, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int *scripts[] = { bytecode1, bytecode2, bytecode3, bytecode4 };
    static BlockFunc functions
        [] = { &MotionBlocks::startGlideTo, &MotionBlocks::startGlideToByIndex, &MotionBlocks::startGlideToMousePointer, &MotionBlocks::startGlideToRandomPosition, &MotionBlocks::glideSecsTo };
    static Value constValues[] = { 2.5, "Sprite2", 3 };

    static const double startX = 100.32;
    static const double startY = -50.12;
    static const double endX = 95.2;
    static const double endY = -175.9;

    Sprite sprite;
    Sprite anotherSprite;
    anotherSprite.setX(endX);
    anotherSprite.setY(endY);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    ClockMock clock;
    RandomGeneratorMock rng;
    MotionBlocks::clock = &clock;
    MotionBlocks::rng = &rng;
    int i = 0;

    for (auto script : scripts) {
        sprite.setX(startX);
        sprite.setY(startY);

        switch (i) {
            case 0:
                EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
                EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));
                break;

            case 1:
                EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&anotherSprite));
                break;

            case 2:
                EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(endX));
                EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(endY));
                break;
            case 3:
                EXPECT_CALL(rng, randint(-240, 240)).WillOnce(Return(std::round(endX)));
                EXPECT_CALL(rng, randint(-180, 180)).WillOnce(Return(std::round(endY)));
            default:
                break;
        }

        vm.setBytecode(script);
        std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(1000));
        EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
        EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
        EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) != MotionBlocks::m_timeMap.cend());
        ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) != MotionBlocks::m_glideMap.cend());
        ASSERT_FALSE(vm.atEnd());
        ASSERT_EQ(sprite.x(), startX);
        ASSERT_EQ(sprite.y(), startY);

        std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(3456));
        EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
        EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
        EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) != MotionBlocks::m_timeMap.cend());
        ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) != MotionBlocks::m_glideMap.cend());
        ASSERT_FALSE(vm.atEnd());

        if (i == 3) {
            ASSERT_EQ(std::round(sprite.x() * 100) / 100, 95.09);
            ASSERT_EQ(std::round(sprite.y() * 100) / 100, -173.78);
        } else {
            ASSERT_EQ(std::round(sprite.x() * 100) / 100, 95.29);
            ASSERT_EQ(std::round(sprite.y() * 100) / 100, -173.69);
        }

        std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(3500));
        EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
        EXPECT_CALL(m_engineMock, lockFrame()).Times(0);
        EXPECT_CALL(m_engineMock, breakFrame()).Times(0);
        vm.run();

        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_TRUE(MotionBlocks::m_timeMap.find(&vm) == MotionBlocks::m_timeMap.cend());
        ASSERT_TRUE(MotionBlocks::m_glideMap.find(&vm) == MotionBlocks::m_glideMap.cend());
        ASSERT_TRUE(vm.atEnd());

        if (i == 3) {
            ASSERT_EQ(sprite.x(), std::round(endX));
            ASSERT_EQ(sprite.y(), std::round(endY));
        } else {
            ASSERT_EQ(std::round(sprite.x() * 100) / 100, endX);
            ASSERT_EQ(std::round(sprite.y() * 100) / 100, endY);
        }

        i++;
    }

    MotionBlocks::clock = Clock::instance().get();
    MotionBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(MotionBlocksTest, ChangeXBy)
{
    Compiler compiler(&m_engineMock);

    // change x by (56.54)
    auto block = std::make_shared<Block>("a", "motion_changexby");
    addValueInput(block, "DX", MotionBlocks::DX, 56.54);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::changeXBy)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileChangeXBy(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 56.54);
}

TEST_F(MotionBlocksTest, ChangeXByImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::changeXBy };
    static Value constValues[] = { 56.54 };

    Sprite sprite;
    sprite.setX(-23.4);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), 33.14);
}

TEST_F(MotionBlocksTest, SetX)
{
    Compiler compiler(&m_engineMock);

    // set x to (-63.4)
    auto block = std::make_shared<Block>("a", "motion_setx");
    addValueInput(block, "X", MotionBlocks::X, -63.4);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::setX)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileSetX(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), -63.4);
}

TEST_F(MotionBlocksTest, SetXImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::setX };
    static Value constValues[] = { -63.4 };

    Sprite sprite;
    sprite.setX(239.98);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.x(), -63.4);
}

TEST_F(MotionBlocksTest, ChangeYBy)
{
    Compiler compiler(&m_engineMock);

    // change y by (135.2)
    auto block = std::make_shared<Block>("a", "motion_changeyby");
    addValueInput(block, "DY", MotionBlocks::DY, 135.2);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::changeYBy)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileChangeYBy(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 135.2);
}

TEST_F(MotionBlocksTest, ChangeYByImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::changeYBy };
    static Value constValues[] = { 135.2 };

    Sprite sprite;
    sprite.setY(200.657);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(std::round(sprite.y() * 1000) / 1000, 335.857);
}

TEST_F(MotionBlocksTest, SetY)
{
    Compiler compiler(&m_engineMock);

    // set y to (189.42)
    auto block = std::make_shared<Block>("a", "motion_sety");
    addValueInput(block, "Y", MotionBlocks::Y, 189.42);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::setY)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileSetY(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 189.42);
}

TEST_F(MotionBlocksTest, SetYImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::setY };
    static Value constValues[] = { 189.42 };

    Sprite sprite;
    sprite.setY(-15.056);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.y(), 189.42);
}

TEST_F(MotionBlocksTest, SetRotationStyle)
{
    Compiler compiler(&m_engineMock);

    // set rotation style [left-right]
    auto block1 = std::make_shared<Block>("a", "motion_setrotationstyle");
    addDropdownField(block1, "STYLE", MotionBlocks::STYLE, "left-right", MotionBlocks::LeftRight);

    // set rotation style [don't rotate]
    auto block2 = std::make_shared<Block>("b", "motion_setrotationstyle");
    addDropdownField(block2, "STYLE", MotionBlocks::STYLE, "don't rotate", MotionBlocks::DoNotRotate);

    // set rotation style [all around]
    auto block3 = std::make_shared<Block>("c", "motion_setrotationstyle");
    addDropdownField(block3, "STYLE", MotionBlocks::STYLE, "all around", MotionBlocks::AllAround);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::setLeftRightRotationStyle)).WillOnce(Return(0));
    compiler.setBlock(block1);
    MotionBlocks::compileSetRotationStyle(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::setDoNotRotateRotationStyle)).WillOnce(Return(1));
    compiler.setBlock(block2);
    MotionBlocks::compileSetRotationStyle(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::setAllAroundRotationStyle)).WillOnce(Return(2));
    compiler.setBlock(block3);
    MotionBlocks::compileSetRotationStyle(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(MotionBlocksTest, SetRotationStyleImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::setLeftRightRotationStyle, &MotionBlocks::setDoNotRotateRotationStyle, &MotionBlocks::setAllAroundRotationStyle };

    Sprite sprite;

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setFunctions(functions);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);

    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);

    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
}

TEST_F(MotionBlocksTest, XPosition)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "motion_xposition");

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::xPosition)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileXPosition(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(MotionBlocksTest, XPositionImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::xPosition };

    Sprite sprite;
    sprite.setX(198.0874);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 198.0874);
}

TEST_F(MotionBlocksTest, YPosition)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "motion_yposition");

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::yPosition)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileYPosition(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(MotionBlocksTest, YPositionImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::yPosition };

    Sprite sprite;
    sprite.setY(-68.5408);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -68.5408);
}

TEST_F(MotionBlocksTest, Direction)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "motion_direction");

    EXPECT_CALL(m_engineMock, functionIndex(&MotionBlocks::direction)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    MotionBlocks::compileDirection(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(MotionBlocksTest, DirectionImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &MotionBlocks::direction };

    Sprite sprite;
    sprite.setDirection(179.605);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 179.605);
}
