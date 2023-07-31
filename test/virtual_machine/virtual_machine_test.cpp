#include "engine/virtualmachine.h"
#include "engine/engine.h"
#include "../common.h"

using namespace libscratchcpp;
using namespace vm;

TEST(VirtualMachineTest, MinimalScript)
{
    static unsigned int bytecode[] = { OP_START, OP_HALT };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_TRUE(vm.atEnd());
}

TEST(VirtualMachineTest, OP_CONST)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_HALT };
    static Value constValues[] = { "test" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "test");
}

TEST(VirtualMachineTest, OP_NULL)
{
    static unsigned int bytecode[] = { OP_START, OP_NULL, OP_HALT };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.run();
    ASSERT_EQ(*vm.getInput(0, 1), Value());
}

TEST(VirtualMachineTest, OP_CHECKPOINT)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_PRINT, OP_CHECKPOINT, OP_CONST, 1, OP_PRINT, OP_HALT };
    static Value constValues[] = { "test1", "test2" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test1\ntest2\n");

    testing::internal::CaptureStdout();
    vm.moveToLastCheckpoint();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test2\n");
}

TEST(VirtualMachineTest, If)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_IF, OP_CONST, 1, OP_PRINT, OP_ENDIF, OP_CONST, 2, OP_IF, OP_CONST, 3, OP_PRINT, OP_ENDIF, OP_HALT };
    static Value constValues[] = { true, "true!", false, "false!" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "true!\n");
}

TEST(VirtualMachineTest, IfElse)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_IF, OP_CONST, 1, OP_PRINT, OP_ELSE, OP_CONST, 3, OP_PRINT, OP_ENDIF, OP_CONST, 2, OP_IF, OP_CONST, 1, OP_PRINT, OP_ELSE, OP_CONST, 3, OP_PRINT, OP_ENDIF, OP_HALT
    };
    static Value constValues[] = { true, "true!", false, "false!" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "true!\nfalse!\n");
}

TEST(VirtualMachineTest, OP_FOREVER_LOOP)
{
    static unsigned int bytecode[] = { OP_START, OP_FOREVER_LOOP, OP_BREAK_ATOMIC, OP_LOOP_END, OP_HALT };

    Engine engine;
    VirtualMachine vm(nullptr, &engine, nullptr);
    vm.setBytecode(bytecode);

    ASSERT_FALSE(engine.breakingCurrentFrame());

    for (int i = 0; i < 100; i++) {
        vm.run();
        ASSERT_TRUE(engine.breakingCurrentFrame());
        ASSERT_FALSE(vm.atEnd());
    }
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_CONST, 1, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3, "test" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\ntest\n");
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP_INDEX)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_REPEAT_LOOP_INDEX, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n1\n2\n");
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP_INDEX1)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_REPEAT_LOOP_INDEX1, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
}

TEST(VirtualMachineTest, OP_UNTIL_LOOP)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_SET_VAR, 0, OP_UNTIL_LOOP, OP_READ_VAR, 0, OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP, OP_CONST, 2, OP_CHANGE_VAR, 0, OP_READ_VAR, 0, OP_PRINT, OP_LOOP_END, OP_HALT
    };
    static Value constValues[] = { 0, 3, 1 };
    Value var;
    Value *variables[] = { &var };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
}

// TODO: Add operator tests

TEST(VirtualMachineTest, OP_ROUND)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_ROUND, OP_CONST, 1, OP_ROUND, OP_CONST, 2,  OP_ROUND, OP_CONST, 3,  OP_ROUND, OP_CONST, 4,  OP_ROUND, OP_CONST, 5,  OP_ROUND, OP_CONST, 6, OP_ROUND, OP_CONST, 7,
        OP_ROUND, OP_CONST, 8, OP_ROUND, OP_CONST, 9, OP_ROUND, OP_CONST, 10, OP_ROUND, OP_CONST, 11, OP_ROUND, OP_CONST, 12, OP_ROUND, OP_CONST, 13, OP_ROUND, OP_HALT
    };
    static Value constValues[] = { 5, 5.4, 5.5, 5.6, -5, -5.4, -5.5, -5.52, 0, "NaN", "Infinity", "-Infinity", 999999999999999999.1, -999999999999999999.1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(1, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(2, 14)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(3, 14)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(4, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(5, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(6, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(7, 14)->toDouble(), -6);
    ASSERT_EQ(vm.getInput(8, 14)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(9, 14)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(9, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(10, 14)->isInfinity());
    ASSERT_TRUE(vm.getInput(11, 14)->isNegativeInfinity());
    ASSERT_EQ(vm.getInput(12, 14)->toDouble(), 999999999999999999L);
    ASSERT_EQ(vm.getInput(13, 14)->toDouble(), -999999999999999999L);
}

TEST(VirtualMachineTest, OP_ABS)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,      OP_ABS,   OP_CONST, 1,      OP_ABS,   OP_CONST, 2,      OP_ABS,   OP_CONST, 3,      OP_ABS,   OP_CONST, 4,      OP_ABS,
        OP_CONST, 5,        OP_ABS, OP_CONST, 6,        OP_ABS, OP_CONST, 7,        OP_ABS, OP_CONST, 8,        OP_ABS, OP_CONST, 9,        OP_ABS, OP_HALT
    };
    static Value constValues[] = { 0, 2.5, 5, -2.5, -5, "NaN", "Infinity", "-Infinity", 999999999999999999.1, -999999999999999999.1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 10)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(1, 10)->toDouble(), 2.5);
    ASSERT_EQ(vm.getInput(2, 10)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(3, 10)->toDouble(), 2.5);
    ASSERT_EQ(vm.getInput(4, 10)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(5, 10)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(5, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(6, 10)->isInfinity());
    ASSERT_TRUE(vm.getInput(7, 10)->isInfinity());
    ASSERT_EQ(vm.getInput(8, 10)->toDouble(), 999999999999999999.1);
    ASSERT_EQ(vm.getInput(9, 10)->toDouble(), 999999999999999999.1);
}

TEST(VirtualMachineTest, OP_FLOOR)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_FLOOR, OP_CONST, 1, OP_FLOOR, OP_CONST, 2,  OP_FLOOR, OP_CONST, 3,  OP_FLOOR, OP_CONST, 4,  OP_FLOOR, OP_CONST, 5,  OP_FLOOR, OP_CONST, 6, OP_FLOOR, OP_CONST, 7,
        OP_FLOOR, OP_CONST, 8, OP_FLOOR, OP_CONST, 9, OP_FLOOR, OP_CONST, 10, OP_FLOOR, OP_CONST, 11, OP_FLOOR, OP_CONST, 12, OP_FLOOR, OP_CONST, 13, OP_FLOOR, OP_HALT
    };
    static Value constValues[] = { 5, 5.4, 5.5, 5.6, -5, -5.4, -5.5, -5.52, 0, "NaN", "Infinity", "-Infinity", 999999999999999999.1, -999999999999999999.1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(1, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(2, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(3, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(4, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(5, 14)->toDouble(), -6);
    ASSERT_EQ(vm.getInput(6, 14)->toDouble(), -6);
    ASSERT_EQ(vm.getInput(7, 14)->toDouble(), -6);
    ASSERT_EQ(vm.getInput(8, 14)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(9, 14)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(9, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(10, 14)->isInfinity());
    ASSERT_TRUE(vm.getInput(11, 14)->isNegativeInfinity());
    ASSERT_EQ(vm.getInput(12, 14)->toDouble(), 999999999999999999L);
    ASSERT_EQ(vm.getInput(13, 14)->toDouble(), -999999999999999999L);
}

TEST(VirtualMachineTest, OP_CEIL)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_CEIL, OP_CONST, 1, OP_CEIL, OP_CONST, 2,  OP_CEIL, OP_CONST, 3,  OP_CEIL, OP_CONST, 4,  OP_CEIL, OP_CONST, 5,  OP_CEIL, OP_CONST, 6, OP_CEIL, OP_CONST, 7,
        OP_CEIL,  OP_CONST, 8, OP_CEIL, OP_CONST, 9, OP_CEIL, OP_CONST, 10, OP_CEIL, OP_CONST, 11, OP_CEIL, OP_CONST, 12, OP_CEIL, OP_CONST, 13, OP_CEIL, OP_HALT
    };
    static Value constValues[] = { 5, 5.4, 5.5, 5.6, -5, -5.4, -5.5, -5.52, 0, "NaN", "Infinity", "-Infinity", 999999999999999999.1, -999999999999999999.1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 14)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(1, 14)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(2, 14)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(3, 14)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(4, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(5, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(6, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(7, 14)->toDouble(), -5);
    ASSERT_EQ(vm.getInput(8, 14)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(9, 14)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(9, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(10, 14)->isInfinity());
    ASSERT_TRUE(vm.getInput(11, 14)->isNegativeInfinity());
    ASSERT_EQ(vm.getInput(12, 14)->toDouble(), 999999999999999999L);
    ASSERT_EQ(vm.getInput(13, 14)->toDouble(), -999999999999999999L);
}

TEST(VirtualMachineTest, OP_SQRT)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_SQRT, OP_CONST, 1, OP_SQRT, OP_CONST, 2, OP_SQRT, OP_CONST, 3, OP_SQRT, OP_CONST, 4, OP_SQRT, OP_CONST, 5, OP_SQRT, OP_CONST, 6, OP_SQRT, OP_HALT
    };
    static Value constValues[] = { 25, 0.25, -9, 0, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 7)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(1, 7)->toDouble(), 0.5);
    ASSERT_TRUE(vm.getInput(2, 7)->isNaN());
    ASSERT_EQ(vm.getInput(3, 7)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(4, 7)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(4, 7)->isNaN());
    ASSERT_TRUE(vm.getInput(5, 7)->isInfinity());
    ASSERT_TRUE(vm.getInput(6, 7)->isNaN());
}

TEST(VirtualMachineTest, OP_SIN)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_SIN, OP_CONST, 1, OP_SIN, OP_CONST, 2,  OP_SIN, OP_CONST, 3,  OP_SIN, OP_CONST, 4,  OP_SIN, OP_CONST, 5,  OP_SIN, OP_CONST, 6, OP_SIN, OP_CONST, 7,
        OP_SIN,   OP_CONST, 8, OP_SIN, OP_CONST, 9, OP_SIN, OP_CONST, 10, OP_SIN, OP_CONST, 11, OP_SIN, OP_CONST, 12, OP_SIN, OP_CONST, 13, OP_SIN, OP_HALT
    };
    static Value constValues[] = { 30, 90, 180, 360, 720, -30, -90, -180, -360, -720, 0, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(std::round(vm.getInput(0, 14)->toDouble() * 10), 5);
    ASSERT_EQ(vm.getInput(1, 14)->toDouble(), 1);
    ASSERT_EQ(std::round(vm.getInput(2, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(std::round(vm.getInput(3, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(std::round(vm.getInput(4, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(std::round(vm.getInput(5, 14)->toDouble() * 10), -5);
    ASSERT_EQ(vm.getInput(6, 14)->toDouble(), -1);
    ASSERT_EQ(std::round(vm.getInput(7, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(std::round(vm.getInput(8, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(std::round(vm.getInput(9, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(vm.getInput(10, 14)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(11, 14)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(11, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(12, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(13, 14)->isNaN());
}

TEST(VirtualMachineTest, OP_COS)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_COS, OP_CONST, 1, OP_COS, OP_CONST, 2,  OP_COS, OP_CONST, 3,  OP_COS, OP_CONST, 4,  OP_COS, OP_CONST, 5,  OP_COS, OP_CONST, 6, OP_COS, OP_CONST, 7,
        OP_COS,   OP_CONST, 8, OP_COS, OP_CONST, 9, OP_COS, OP_CONST, 10, OP_COS, OP_CONST, 11, OP_COS, OP_CONST, 12, OP_COS, OP_CONST, 13, OP_COS, OP_HALT
    };
    static Value constValues[] = { 60, 90, 180, 360, 720, -60, -90, -180, -360, -720, 0, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(std::round(vm.getInput(0, 14)->toDouble() * 10), 5);
    ASSERT_EQ(std::round(vm.getInput(1, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(vm.getInput(2, 14)->toDouble(), -1);
    ASSERT_EQ(vm.getInput(3, 14)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(4, 14)->toDouble(), 1);
    ASSERT_EQ(std::round(vm.getInput(5, 14)->toDouble() * 10), 5);
    ASSERT_EQ(std::round(vm.getInput(6, 14)->toDouble() * 1000), 0);
    ASSERT_EQ(vm.getInput(7, 14)->toDouble(), -1);
    ASSERT_EQ(vm.getInput(8, 14)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(9, 14)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(10, 14)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(11, 14)->toDouble(), 1);
    ASSERT_FALSE(vm.getInput(11, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(12, 14)->isNaN());
    ASSERT_TRUE(vm.getInput(13, 14)->isNaN());
}

TEST(VirtualMachineTest, OP_TAN)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,      OP_TAN,   OP_CONST, 1,      OP_TAN,   OP_CONST, 2,      OP_TAN,   OP_CONST, 3,      OP_TAN,   OP_CONST, 4,      OP_TAN,
        OP_CONST, 5,        OP_TAN, OP_CONST, 6,        OP_TAN, OP_CONST, 7,        OP_TAN, OP_CONST, 8,        OP_TAN, OP_CONST, 9,        OP_TAN, OP_HALT
    };
    static Value constValues[] = { 0, 45, -45, 90, 270, -90, -270, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 10)->toDouble(), 0);
    ASSERT_EQ(std::round(vm.getInput(1, 10)->toDouble()), 1);
    ASSERT_EQ(std::round(vm.getInput(2, 10)->toDouble()), -1);
    ASSERT_TRUE(vm.getInput(3, 10)->isInfinity());
    ASSERT_TRUE(vm.getInput(4, 10)->isNegativeInfinity());
    ASSERT_TRUE(vm.getInput(5, 10)->isNegativeInfinity());
    ASSERT_TRUE(vm.getInput(6, 10)->isInfinity());
    ASSERT_EQ(vm.getInput(7, 10)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(7, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(8, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(9, 10)->isNaN());
}

TEST(VirtualMachineTest, OP_ASIN)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,       OP_ASIN,  OP_CONST, 1,       OP_ASIN,  OP_CONST, 2,       OP_ASIN,  OP_CONST, 3,       OP_ASIN,  OP_CONST, 4,       OP_ASIN,
        OP_CONST, 5,        OP_ASIN, OP_CONST, 6,        OP_ASIN, OP_CONST, 7,        OP_ASIN, OP_CONST, 8,        OP_ASIN, OP_CONST, 9,        OP_ASIN, OP_HALT
    };
    static Value constValues[] = { 0, 0.5, 1, -0.5, -1, 1.5, -1.5, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 10)->toDouble(), 0);
    ASSERT_EQ(std::round(vm.getInput(1, 10)->toDouble() * 1000) / 1000, 30);
    ASSERT_EQ(vm.getInput(2, 10)->toDouble(), 90);
    ASSERT_EQ(std::round(vm.getInput(3, 10)->toDouble() * 1000) / 1000, -30);
    ASSERT_EQ(vm.getInput(4, 10)->toDouble(), -90);
    ASSERT_TRUE(vm.getInput(5, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(6, 10)->isNaN());
    ASSERT_EQ(vm.getInput(7, 10)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(7, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(8, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(9, 10)->isNaN());
}

TEST(VirtualMachineTest, OP_ACOS)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,       OP_ACOS,  OP_CONST, 1,       OP_ACOS,  OP_CONST, 2,       OP_ACOS,  OP_CONST, 3,       OP_ACOS,  OP_CONST, 4,       OP_ACOS,
        OP_CONST, 5,        OP_ACOS, OP_CONST, 6,        OP_ACOS, OP_CONST, 7,        OP_ACOS, OP_CONST, 8,        OP_ACOS, OP_CONST, 9,        OP_ACOS, OP_HALT
    };
    static Value constValues[] = { 0, 0.5, 1, -0.5, -1, 1.5, -1.5, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 10)->toDouble(), 90);
    ASSERT_EQ(std::round(vm.getInput(1, 10)->toDouble() * 1000) / 1000, 60);
    ASSERT_EQ(vm.getInput(2, 10)->toDouble(), 0);
    ASSERT_EQ(std::round(vm.getInput(3, 10)->toDouble() * 1000) / 1000, 120);
    ASSERT_EQ(vm.getInput(4, 10)->toDouble(), 180);
    ASSERT_TRUE(vm.getInput(5, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(6, 10)->isNaN());
    ASSERT_EQ(vm.getInput(7, 10)->toDouble(), 90);
    ASSERT_FALSE(vm.getInput(7, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(8, 10)->isNaN());
    ASSERT_TRUE(vm.getInput(9, 10)->isNaN());
}

TEST(VirtualMachineTest, OP_ATAN)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_ATAN, OP_CONST, 1, OP_ATAN, OP_CONST, 2, OP_ATAN, OP_CONST, 3, OP_ATAN, OP_CONST, 4, OP_ATAN, OP_CONST, 5, OP_ATAN, OP_HALT };
    static Value constValues[] = { 0, 1, -1, "NaN", "Infinity", "-Infinity" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.getInput(0, 6)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(1, 6)->toDouble(), 45);
    ASSERT_EQ(vm.getInput(2, 6)->toDouble(), -45);
    ASSERT_EQ(vm.getInput(3, 6)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(3, 6)->isNaN());
    ASSERT_EQ(vm.getInput(4, 6)->toDouble(), 90);
    ASSERT_EQ(vm.getInput(5, 6)->toDouble(), -90);
}

// TODO: Add comparison operator tests

TEST(VirtualMachineTest, OP_AND)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 0, OP_AND, OP_CONST, 0, OP_CONST, 1, OP_AND, OP_CONST, 1, OP_CONST, 0, OP_AND, OP_CONST, 1, OP_CONST, 1, OP_AND, OP_HALT };
    static Value constValues[] = { false, true };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_FALSE(vm.getInput(0, 4)->toBool());
    ASSERT_FALSE(vm.getInput(1, 4)->toBool());
    ASSERT_FALSE(vm.getInput(2, 4)->toBool());
    ASSERT_TRUE(vm.getInput(3, 4)->toBool());
}

TEST(VirtualMachineTest, OP_OR)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 0, OP_OR, OP_CONST, 0, OP_CONST, 1, OP_OR, OP_CONST, 1, OP_CONST, 0, OP_OR, OP_CONST, 1, OP_CONST, 1, OP_OR, OP_HALT };
    static Value constValues[] = { false, true };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_FALSE(vm.getInput(0, 4)->toBool());
    ASSERT_TRUE(vm.getInput(1, 4)->toBool());
    ASSERT_TRUE(vm.getInput(2, 4)->toBool());
    ASSERT_TRUE(vm.getInput(3, 4)->toBool());
}

TEST(VirtualMachineTest, OP_NOT)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_NOT, OP_CONST, 1, OP_NOT, OP_HALT };
    static Value constValues[] = { false, true };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_TRUE(vm.getInput(0, 2)->toBool());
    ASSERT_FALSE(vm.getInput(1, 2)->toBool());
}

// TODO: Add tests for remaining instructions
