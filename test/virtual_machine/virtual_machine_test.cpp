#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/list.h>
#include <scratchcpp/script.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "engine/virtualmachine_p.h"
#include "engine/internal/engine.h"
#include "engine/internal/randomgenerator.h"
#include "../common.h"

using namespace libscratchcpp;
using namespace vm;

using ::testing::Return;

TEST(VirtualMachineTest, Constructors)
{
    VirtualMachine vm1;
    ASSERT_EQ(vm1.target(), nullptr);
    ASSERT_EQ(vm1.engine(), nullptr);
    ASSERT_EQ(vm1.script(), nullptr);

    Target target;
    Engine engine;
    Script script(&target, &engine);
    VirtualMachine vm2(&target, &engine, &script);
    ASSERT_EQ(vm2.target(), &target);
    ASSERT_EQ(vm2.engine(), &engine);
    ASSERT_EQ(vm2.script(), &script);
}

TEST(VirtualMachineTest, Procedures)
{
    static unsigned int procedure[] = { OP_START, OP_HALT };
    static unsigned int *procedures[] = { procedure };

    VirtualMachine vm;
    ASSERT_EQ(vm.procedures(), nullptr);
    vm.setProcedures(procedures);
    ASSERT_EQ(vm.procedures(), procedures);
}

TEST(VirtualMachineTest, ConstValues)
{
    static Value constValues[] = { "test1", "test2" };

    VirtualMachine vm;
    ASSERT_EQ(vm.constValues(), nullptr);
    vm.setConstValues(constValues);
    ASSERT_EQ(vm.constValues(), constValues);
}

TEST(VirtualMachineTest, Variables)
{
    Value var1, var2;
    Value *variables[] = { &var1, &var2 };

    VirtualMachine vm;
    ASSERT_EQ(vm.variables(), nullptr);

    vm.setVariables(variables);
    ASSERT_EQ(vm.variables(), variables);

    vm.setVariablesVector({ &var1, &var2 });
    ASSERT_EQ(vm.variables()[0], &var1);
    ASSERT_EQ(vm.variables()[1], &var2);
}

TEST(VirtualMachineTest, Lists)
{
    List list1("", "");
    List list2("", "");
    List *lists[] = { &list1, &list2 };

    VirtualMachine vm;
    ASSERT_EQ(vm.lists(), nullptr);

    vm.setLists(lists);
    ASSERT_EQ(vm.lists(), lists);

    vm.setListsVector({ &list1, &list2 });
    ASSERT_EQ(vm.lists()[0], &list1);
    ASSERT_EQ(vm.lists()[1], &list2);
}

TEST(VirtualMachineTest, Bytecode)
{
    static unsigned int bytecode[] = { OP_START, OP_HALT };

    VirtualMachine vm;
    ASSERT_EQ(vm.bytecode(), nullptr);
    vm.setBytecode(bytecode);
    ASSERT_EQ(vm.bytecode(), bytecode);
}

TEST(VirtualMachineTest, ReturnValues)
{
    static unsigned int bytecode[] = { OP_START, OP_HALT };

    VirtualMachine vm;
    ASSERT_EQ(vm.registerCount(), 0);

    vm.addReturnValue("hello world");
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "hello world");

    vm.addReturnValue(55.23);
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toString(), "hello world");
    ASSERT_EQ(vm.getInput(1, 2)->toDouble(), 55.23);

    vm.replaceReturnValue("hello", 2);
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toString(), "hello");
    ASSERT_EQ(vm.getInput(1, 2)->toDouble(), 55.23);

    vm.replaceReturnValue("world", 1);
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toString(), "hello");
    ASSERT_EQ(vm.getInput(1, 2)->toString(), "world");
}

TEST(VirtualMachineTest, MinimalScript)
{
    static unsigned int bytecode[] = { OP_START, OP_HALT };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_CONST)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_HALT };
    static Value constValues[] = { "test" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "test");
}

TEST(VirtualMachineTest, OP_NULL)
{
    static unsigned int bytecode[] = { OP_START, OP_NULL, OP_HALT };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
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
    ASSERT_EQ(vm.registerCount(), 0);
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
    ASSERT_EQ(vm.registerCount(), 0);
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
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_FOREVER_LOOP)
{
    static unsigned int bytecode[] = { OP_START, OP_FOREVER_LOOP, OP_BREAK_ATOMIC, OP_LOOP_END, OP_HALT };

    Engine engine;
    VirtualMachine vm(nullptr, &engine, nullptr);
    vm.setBytecode(bytecode);

    for (int i = 0; i < 100; i++) {
        vm.run();
        ASSERT_FALSE(vm.atEnd());
    }

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_CONST, 1, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3, "test" };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    EXPECT_CALL(engineMock, breakingCurrentFrame()).Times(3).WillRepeatedly(Return(false));
    EXPECT_CALL(engineMock, skipFrame()).Times(3);

    while (!vm.atEnd())
        vm.run();

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\ntest\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP_warp)
{
    static unsigned int bytecode[] = { OP_START, OP_WARP, OP_CONST, 0, OP_REPEAT_LOOP, OP_CONST, 1, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3, "test" };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakFrame()).Times(0);

    vm.run();

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\ntest\ntest\n");
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP_INDEX)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_REPEAT_LOOP_INDEX, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3 };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakingCurrentFrame()).Times(3).WillRepeatedly(Return(false));
    EXPECT_CALL(engineMock, skipFrame()).Times(3);

    while (!vm.atEnd())
        vm.run();

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n1\n2\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_REPEAT_LOOP_INDEX1)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_REPEAT_LOOP, OP_REPEAT_LOOP_INDEX1, OP_PRINT, OP_LOOP_END, OP_HALT };
    static Value constValues[] = { 3 };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakingCurrentFrame()).Times(3).WillRepeatedly(Return(false));
    EXPECT_CALL(engineMock, skipFrame()).Times(3);

    while (!vm.atEnd())
        vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_UNTIL_LOOP)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_SET_VAR, 0, OP_UNTIL_LOOP, OP_READ_VAR, 0, OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP, OP_CONST, 2, OP_CHANGE_VAR, 0, OP_READ_VAR, 0, OP_PRINT, OP_LOOP_END, OP_HALT
    };
    static Value constValues[] = { 0, 3, 1 };
    Value var;
    Value *variables[] = { &var };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    EXPECT_CALL(engineMock, breakingCurrentFrame()).Times(3).WillRepeatedly(Return(false));
    EXPECT_CALL(engineMock, skipFrame()).Times(3);

    while (!vm.atEnd())
        vm.run();

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_UNTIL_LOOP_stop)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0, OP_SET_VAR, 0, OP_UNTIL_LOOP, OP_READ_VAR, 0, OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP, OP_READ_VAR, 0, OP_PRINT, OP_LOOP_END, OP_HALT
    };
    static Value constValues[] = { 0, 3, 1 };
    Value var;
    Value *variables[] = { &var };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    EXPECT_CALL(engineMock, breakingCurrentFrame()).Times(3).WillRepeatedly(Return(false));
    EXPECT_CALL(engineMock, skipFrame()).Times(3);

    while (!vm.atEnd()) {
        vm.run();
        var.add(1);
    }

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "0\n1\n2\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_UNTIL_LOOP_warp)
{
    static unsigned int bytecode[] = {
        OP_START, OP_WARP, OP_CONST,      0, OP_SET_VAR,  0, OP_UNTIL_LOOP, OP_READ_VAR, 0,      OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP,
        OP_CONST, 2,       OP_CHANGE_VAR, 0, OP_READ_VAR, 0, OP_PRINT,      OP_LOOP_END, OP_HALT
    };
    static Value constValues[] = { 0, 3, 1 };
    Value var;
    Value *variables[] = { &var };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    testing::internal::CaptureStdout();
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n3\n");
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_ADD)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_ADD, OP_HALT };
    static Value constValues[] = { -5.25, 108.837 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that the Value::add() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 103.587);
}

TEST(VirtualMachineTest, OP_SUBTRACT)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_SUBTRACT, OP_HALT };
    static Value constValues[] = { -5.25, 108.837 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that the Value::subtract() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -114.087);
}

TEST(VirtualMachineTest, OP_MULTIPLY)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_MULTIPLY, OP_HALT };
    static Value constValues[] = { -5.25, 108.837 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that the Value::multiply() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -571.39425);
}

TEST(VirtualMachineTest, OP_DIVIDE)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_DIVIDE, OP_HALT };
    static Value constValues[] = { 108.83, 5 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that the Value::divide() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 21.766);
}

TEST(VirtualMachineTest, OP_MOD)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_MOD, OP_HALT };
    static Value constValues[] = { -108.5, 2.5 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that the Value::mod() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 1.5);
}

TEST(VirtualMachineTest, OP_RANDOM)
{
    static unsigned int bytecode1[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_RANDOM, OP_HALT };
    static unsigned int bytecode2[] = { OP_START, OP_CONST, 1, OP_CONST, 2, OP_RANDOM, OP_HALT };
    static unsigned int bytecode3[] = { OP_START, OP_CONST, 3, OP_CONST, 0, OP_RANDOM, OP_HALT };
    static unsigned int bytecode4[] = { OP_START, OP_CONST, 2, OP_CONST, 3, OP_RANDOM, OP_HALT };
    static Value constValues[] = { -45, 12, 6.05, -78.686 };

    VirtualMachinePrivate vm(nullptr, nullptr, nullptr, nullptr);
    vm.constValues = constValues;

    RandomGeneratorMock rng;
    vm.rng = &rng;

    EXPECT_CALL(rng, randint(-45, 12)).WillOnce(Return(-18));
    vm.bytecode = bytecode1;
    vm.pos = bytecode1;
    vm.regCount = 0;
    vm.run(vm.pos);

    ASSERT_EQ(vm.regCount, 1);
    ASSERT_EQ(vm.regs[vm.regCount - 1]->toDouble(), -18);

    EXPECT_CALL(rng, randintDouble(12, 6.05)).WillOnce(Return(3.486789));
    vm.bytecode = bytecode2;
    vm.pos = bytecode2;
    vm.regCount = 0;
    vm.run(vm.pos);

    ASSERT_EQ(vm.regCount, 1);
    ASSERT_EQ(vm.regs[vm.regCount - 1]->toDouble(), 3.486789);

    EXPECT_CALL(rng, randintDouble(-78.686, -45)).WillOnce(Return(-59.468873));
    vm.bytecode = bytecode3;
    vm.pos = bytecode3;
    vm.regCount = 0;
    vm.run(vm.pos);

    ASSERT_EQ(vm.regCount, 1);
    ASSERT_EQ(vm.regs[vm.regCount - 1]->toDouble(), -59.468873);

    EXPECT_CALL(rng, randintDouble(6.05, -78.686)).WillOnce(Return(-28.648764));
    vm.bytecode = bytecode4;
    vm.pos = bytecode4;
    vm.regCount = 0;
    vm.run(vm.pos);

    ASSERT_EQ(vm.regCount, 1);
    ASSERT_EQ(vm.regs[vm.regCount - 1]->toDouble(), -28.648764);
}

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
    ASSERT_EQ(vm.registerCount(), 14);
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
    ASSERT_EQ(vm.registerCount(), 10);
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
    ASSERT_EQ(vm.registerCount(), 14);
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
    ASSERT_EQ(vm.registerCount(), 14);
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
    ASSERT_EQ(vm.registerCount(), 7);
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
    ASSERT_EQ(vm.registerCount(), 14);
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
    ASSERT_EQ(vm.registerCount(), 14);
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
    ASSERT_EQ(vm.registerCount(), 10);
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
    ASSERT_EQ(vm.registerCount(), 10);
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
    ASSERT_EQ(vm.registerCount(), 10);
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
    ASSERT_EQ(vm.registerCount(), 6);
    ASSERT_EQ(vm.getInput(0, 6)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(1, 6)->toDouble(), 45);
    ASSERT_EQ(vm.getInput(2, 6)->toDouble(), -45);
    ASSERT_EQ(vm.getInput(3, 6)->toDouble(), 0);
    ASSERT_FALSE(vm.getInput(3, 6)->isNaN());
    ASSERT_EQ(vm.getInput(4, 6)->toDouble(), 90);
    ASSERT_EQ(vm.getInput(5, 6)->toDouble(), -90);
}

TEST(VirtualMachineTest, OP_GREATER_THAN)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_GREATER_THAN, OP_CONST, 1, OP_CONST, 0, OP_GREATER_THAN, OP_CONST, 0, OP_CONST, 0, OP_GREATER_THAN, OP_HALT };
    static Value constValues[] = { 108.83, 108.542 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that Value::operator>() is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 3);
    ASSERT_EQ(vm.getInput(0, 3)->toBool(), true);
    ASSERT_EQ(vm.getInput(1, 3)->toBool(), false);
    ASSERT_EQ(vm.getInput(2, 3)->toBool(), false);
}

TEST(VirtualMachineTest, OP_LESS_THAN)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_LESS_THAN, OP_CONST, 1, OP_CONST, 0, OP_LESS_THAN, OP_CONST, 0, OP_CONST, 0, OP_LESS_THAN, OP_HALT };
    static Value constValues[] = { 108.83, 108.542 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that Value::operator<() is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 3);
    ASSERT_EQ(vm.getInput(0, 3)->toBool(), false);
    ASSERT_EQ(vm.getInput(1, 3)->toBool(), true);
    ASSERT_EQ(vm.getInput(2, 3)->toBool(), false);
}

TEST(VirtualMachineTest, OP_EQUALS)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_EQUALS, OP_CONST, 1, OP_CONST, 0, OP_EQUALS, OP_CONST, 0, OP_CONST, 0, OP_EQUALS, OP_HALT };
    static Value constValues[] = { 108.83, 108.542 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    // NOTE: This assumes that Value::operator==() is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(vm.registerCount(), 3);
    ASSERT_EQ(vm.getInput(0, 3)->toBool(), false);
    ASSERT_EQ(vm.getInput(1, 3)->toBool(), false);
    ASSERT_EQ(vm.getInput(2, 3)->toBool(), true);
}

TEST(VirtualMachineTest, OP_AND)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 0, OP_AND, OP_CONST, 0, OP_CONST, 1, OP_AND, OP_CONST, 1, OP_CONST, 0, OP_AND, OP_CONST, 1, OP_CONST, 1, OP_AND, OP_HALT };
    static Value constValues[] = { false, true };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 4);
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
    ASSERT_EQ(vm.registerCount(), 4);
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
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_TRUE(vm.getInput(0, 2)->toBool());
    ASSERT_FALSE(vm.getInput(1, 2)->toBool());
}

TEST(VirtualMachineTest, OP_SET_VAR)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_SET_VAR, 0, OP_CONST, 1, OP_SET_VAR, 1, OP_HALT };
    static Value constValues[] = { 108.542, 53.54 };
    Value var1, var2;
    Value *variables[] = { &var1, &var2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    vm.run();
    ASSERT_EQ(var1.toDouble(), 108.542);
    ASSERT_EQ(var2.toDouble(), 53.54);
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_CHANGE_VAR)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CHANGE_VAR, 0, OP_CONST, 1, OP_CHANGE_VAR, 1, OP_HALT };
    static Value constValues[] = { 3.52, 1 };
    Value var1 = 1.234;
    Value var2 = 2;
    Value *variables[] = { &var1, &var2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setVariables(variables);
    vm.run();
    // NOTE: This assumes that the Value::add() method is used, let's hope nobody changes that...
    // Value is performance-critical and can't be mocked!
    ASSERT_EQ(var1.toDouble(), 4.754);
    ASSERT_EQ(var2.toDouble(), 3);
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_READ_VAR)
{
    static unsigned int bytecode[] = { OP_START, OP_READ_VAR, 0, OP_READ_VAR, 1, OP_HALT };
    Value var1 = 1.234;
    Value var2 = 2;
    Value *variables[] = { &var1, &var2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setVariables(variables);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toDouble(), 1.234);
    ASSERT_EQ(vm.getInput(1, 2)->toDouble(), 2);
}

TEST(VirtualMachineTest, OP_READ_LIST)
{
    static unsigned int bytecode[] = { OP_START, OP_READ_LIST, 0, OP_READ_LIST, 1, OP_HALT };
    List list1("", "list1");
    list1.push_back(5.3);
    list1.push_back("abc");
    list1.push_back(false);
    List list2("", "list2");
    list2.push_back("t e s t");
    list2.push_back(true);
    List *lists[] = { &list1, &list2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setLists(lists);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toString(), "5.3 abc false");
    ASSERT_EQ(vm.getInput(1, 2)->toString(), "t e s t true");
}

TEST(VirtualMachineTest, OP_LIST_APPEND)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_LIST_APPEND, 0, OP_CONST, 1, OP_LIST_APPEND, 1, OP_HALT };
    static Value constValues[] = { 3.52, "test" };
    List list1("", "list1");
    List list2("", "list2");
    List *lists[] = { &list1, &list2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);
    vm.run();

    ASSERT_EQ(list1.size(), 1);
    ASSERT_EQ(list1[0], 3.52);

    ASSERT_EQ(list2.size(), 1);
    ASSERT_EQ(list2[0], "test");

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_LIST_DEL)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 1,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 2,  OP_LIST_DEL, 0, OP_READ_LIST,
        0,        OP_CONST, 3,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 4,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 5,  OP_LIST_DEL, 0, OP_READ_LIST,
        0,        OP_CONST, 6,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 7,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 8,  OP_LIST_DEL, 0, OP_READ_LIST,
        0,        OP_CONST, 9,  OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 10, OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 11, OP_LIST_DEL, 0, OP_READ_LIST,
        0,        OP_CONST, 12, OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_CONST, 13, OP_LIST_DEL, 0, OP_READ_LIST, 0, OP_HALT
    };
    static Value constValues[] = { 3, 1, 6, 0, 7, -1, 9, Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN, "invalid", "last", "random", "all" };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("f");
    list1.push_back("g");
    list1.push_back("h");
    List *lists[] = { &list1 };

    RandomGeneratorMock rng;
    VirtualMachinePrivate::rng = &rng;

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);

    EXPECT_CALL(rng, randint(1, 4)).WillOnce(Return(2));
    vm.run();

    VirtualMachinePrivate::rng = RandomGenerator::instance().get();

    ASSERT_EQ(vm.registerCount(), 14);
    ASSERT_EQ(vm.getInput(0, 14)->toString(), "a b d e f g h");
    ASSERT_EQ(vm.getInput(1, 14)->toString(), "b d e f g h");
    ASSERT_EQ(vm.getInput(2, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(3, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(4, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(5, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(6, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(7, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(8, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(9, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(10, 14)->toString(), "b d e f g");
    ASSERT_EQ(vm.getInput(11, 14)->toString(), "b d e f");
    ASSERT_EQ(vm.getInput(12, 14)->toString().size(), 5);
    ASSERT_EQ(vm.getInput(13, 14)->toString(), "");
}

TEST(VirtualMachineTest, OP_LIST_DEL_ALL)
{
    static unsigned int bytecode[] = { OP_START, OP_LIST_DEL_ALL, 0, OP_LIST_DEL_ALL, 1, OP_HALT };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    List list2("", "list1");
    list1.push_back("ab c");
    List *lists[] = { &list1, &list2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setLists(lists);
    vm.run();
    ASSERT_TRUE(list1.empty());
    ASSERT_TRUE(list2.empty());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_LIST_INSERT)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST,       0,  OP_CONST,       1,  OP_LIST_INSERT, 0, OP_READ_LIST,   0, OP_CONST,       0,  OP_CONST,       2,  OP_LIST_INSERT, 0, OP_READ_LIST,   0, OP_CONST,
        0,        OP_CONST,       3,  OP_LIST_INSERT, 0,  OP_READ_LIST,   0, OP_CONST,       0, OP_CONST,       4,  OP_LIST_INSERT, 0,  OP_READ_LIST,   0, OP_CONST,       0, OP_CONST,
        5,        OP_LIST_INSERT, 0,  OP_READ_LIST,   0,  OP_CONST,       0, OP_CONST,       6, OP_LIST_INSERT, 0,  OP_READ_LIST,   0,  OP_CONST,       0, OP_CONST,       7, OP_LIST_INSERT,
        0,        OP_READ_LIST,   0,  OP_CONST,       0,  OP_CONST,       8, OP_LIST_INSERT, 0, OP_READ_LIST,   0,  OP_CONST,       0,  OP_CONST,       9, OP_LIST_INSERT, 0, OP_READ_LIST,
        0,        OP_CONST,       0,  OP_CONST,       10, OP_LIST_INSERT, 0, OP_READ_LIST,   0, OP_CONST,       0,  OP_CONST,       11, OP_LIST_INSERT, 0, OP_READ_LIST,   0, OP_CONST,
        0,        OP_CONST,       12, OP_LIST_INSERT, 0,  OP_READ_LIST,   0, OP_CONST,       0, OP_CONST,       13, OP_LIST_INSERT, 0,  OP_READ_LIST,   0, OP_HALT
    };
    static Value constValues[] = { "new item", 3, 1, 10, 0, 12, -1, 14, Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN, "last", "random", "invalid" };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("f");
    list1.push_back("g");
    list1.push_back("h");
    List *lists[] = { &list1 };

    RandomGeneratorMock rng;
    VirtualMachinePrivate::rng = &rng;

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);

    EXPECT_CALL(rng, randint(1, 12)).WillOnce(Return(5));
    vm.run();

    VirtualMachinePrivate::rng = RandomGenerator::instance().get();

    ASSERT_EQ(vm.registerCount(), 13);
    ASSERT_EQ(vm.getInput(0, 13)->toString(), "a b new item c d e f g h");
    ASSERT_EQ(vm.getInput(1, 13)->toString(), "new item a b new item c d e f g h");
    ASSERT_EQ(vm.getInput(2, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(3, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(4, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(5, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(6, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(7, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(8, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(9, 13)->toString(), "new item a b new item c d e f g new item h");
    ASSERT_EQ(vm.getInput(10, 13)->toString(), "new item a b new item c d e f g new item h new item");
    ASSERT_EQ(vm.getInput(11, 13)->toString().size(), 60);
    ASSERT_EQ(vm.getInput(12, 13)->toString(), vm.getInput(11, 13)->toString());
}

TEST(VirtualMachineTest, OP_LIST_REPLACE)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST,        1,  OP_CONST,        0, OP_LIST_REPLACE, 0, OP_READ_LIST,    0,  OP_CONST,        2,  OP_CONST,        0,  OP_LIST_REPLACE, 0, OP_READ_LIST,    0, OP_CONST,
        3,        OP_CONST,        0,  OP_LIST_REPLACE, 0, OP_READ_LIST,    0, OP_CONST,        4,  OP_CONST,        0,  OP_LIST_REPLACE, 0,  OP_READ_LIST,    0, OP_CONST,        5, OP_CONST,
        0,        OP_LIST_REPLACE, 0,  OP_READ_LIST,    0, OP_CONST,        6, OP_CONST,        0,  OP_LIST_REPLACE, 0,  OP_READ_LIST,    0,  OP_CONST,        7, OP_CONST,        0, OP_LIST_REPLACE,
        0,        OP_READ_LIST,    0,  OP_CONST,        8, OP_CONST,        0, OP_LIST_REPLACE, 0,  OP_READ_LIST,    0,  OP_CONST,        9,  OP_CONST,        0, OP_LIST_REPLACE, 0, OP_READ_LIST,
        0,        OP_CONST,        10, OP_CONST,        0, OP_LIST_REPLACE, 0, OP_READ_LIST,    0,  OP_CONST,        11, OP_CONST,        11, OP_LIST_REPLACE, 0, OP_READ_LIST,    0, OP_CONST,
        12,       OP_CONST,        14, OP_LIST_REPLACE, 0, OP_READ_LIST,    0, OP_CONST,        13, OP_CONST,        0,  OP_LIST_REPLACE, 0,  OP_READ_LIST,    0, OP_HALT
    };
    static Value constValues[] = {
        "new item", 3, 1, 8, 0, 9, -1, 12, Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN, "last", "random", "invalid", "test"
    };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("f");
    list1.push_back("g");
    list1.push_back("h");
    List *lists[] = { &list1 };

    RandomGeneratorMock rng;
    VirtualMachinePrivate::rng = &rng;

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);

    EXPECT_CALL(rng, randint(1, 8)).WillOnce(Return(7));
    vm.run();

    VirtualMachinePrivate::rng = RandomGenerator::instance().get();

    ASSERT_EQ(vm.registerCount(), 13);
    ASSERT_EQ(vm.getInput(0, 13)->toString(), "a b new item d e f g h");
    ASSERT_EQ(vm.getInput(1, 13)->toString(), "new item b new item d e f g h");
    ASSERT_EQ(vm.getInput(2, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(3, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(4, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(5, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(6, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(7, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(8, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(9, 13)->toString(), "new item b new item d e f g new item");
    ASSERT_EQ(vm.getInput(10, 13)->toString(), "new item b new item d e f g last");
    ASSERT_TRUE(list1.contains("test"));
    ASSERT_EQ(vm.getInput(12, 13)->toString(), vm.getInput(11, 13)->toString());
}

TEST(VirtualMachineTest, OP_LIST_GET_ITEM)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST, 0,  OP_LIST_GET_ITEM, 0, OP_CONST, 1,  OP_LIST_GET_ITEM, 0, OP_CONST, 2,  OP_LIST_GET_ITEM, 0, OP_CONST, 3, OP_LIST_GET_ITEM, 0, OP_CONST, 4, OP_LIST_GET_ITEM,
        0,        OP_CONST, 5,  OP_LIST_GET_ITEM, 0, OP_CONST, 6,  OP_LIST_GET_ITEM, 0, OP_CONST, 7,  OP_LIST_GET_ITEM, 0, OP_CONST, 8, OP_LIST_GET_ITEM, 0, OP_CONST, 9, OP_LIST_GET_ITEM,
        0,        OP_CONST, 10, OP_LIST_GET_ITEM, 0, OP_CONST, 11, OP_LIST_GET_ITEM, 0, OP_CONST, 12, OP_LIST_GET_ITEM, 0, OP_HALT
    };
    static Value constValues[] = { 3, 1, 8, 0, 9, -1, 12, Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN, "last", "random", "invalid" };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("f");
    list1.push_back("g");
    list1.push_back("h");
    List *lists[] = { &list1 };

    RandomGeneratorMock rng;
    VirtualMachinePrivate::rng = &rng;

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);

    EXPECT_CALL(rng, randint(1, 8)).WillOnce(Return(1));
    vm.run();

    VirtualMachinePrivate::rng = RandomGenerator::instance().get();

    ASSERT_EQ(vm.registerCount(), 13);
    ASSERT_EQ(vm.getInput(0, 13)->toString(), "c");
    ASSERT_EQ(vm.getInput(1, 13)->toString(), "a");
    ASSERT_EQ(vm.getInput(2, 13)->toString(), "h");
    ASSERT_EQ(vm.getInput(3, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(4, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(5, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(6, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(7, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(8, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(9, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(10, 13)->toString(), "h");
    ASSERT_EQ(vm.getInput(11, 13)->toString().size(), 1);
    ASSERT_EQ(vm.getInput(12, 13)->toString(), "");
}

TEST(VirtualMachineTest, OP_LIST_INDEX_OF)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST,         0, OP_LIST_INDEX_OF, 0, OP_CONST,         1, OP_LIST_INDEX_OF, 0, OP_CONST,         2, OP_LIST_INDEX_OF, 0, OP_CONST,         3, OP_LIST_INDEX_OF, 0, OP_CONST,
        4,        OP_LIST_INDEX_OF, 0, OP_CONST,         5, OP_LIST_INDEX_OF, 0, OP_CONST,         6, OP_LIST_INDEX_OF, 0, OP_CONST,         7, OP_LIST_INDEX_OF, 0, OP_HALT
    };
    static Value constValues[] = { "c", "A", "e", "", "invalid", Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("");
    list1.push_back(Value::SpecialValue::Infinity);
    list1.push_back(8);
    List *lists[] = { &list1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 8);
    ASSERT_EQ(vm.getInput(0, 8)->toDouble(), 3);
    ASSERT_EQ(vm.getInput(1, 8)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(2, 8)->toDouble(), 5);
    ASSERT_EQ(vm.getInput(3, 8)->toDouble(), 6);
    ASSERT_EQ(vm.getInput(4, 8)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(5, 8)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(6, 8)->toDouble(), 7);
    ASSERT_EQ(vm.getInput(7, 8)->toDouble(), 0);
}

TEST(VirtualMachineTest, OP_LIST_LENGTH)
{
    static unsigned int bytecode[] = { OP_START, OP_LIST_LENGTH, 0, OP_LIST_LENGTH, 1, OP_HALT };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    List list2("", "list2");
    list2.push_back("a");
    List *lists[] = { &list1, &list2 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setLists(lists);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toDouble(), 4);
    ASSERT_EQ(vm.getInput(1, 2)->toDouble(), 1);
}

TEST(VirtualMachineTest, OP_LIST_CONTAINS)
{
    static unsigned int bytecode[] = {
        OP_START, OP_CONST,         0, OP_LIST_CONTAINS, 0, OP_CONST,         1, OP_LIST_CONTAINS, 0, OP_CONST,         2, OP_LIST_CONTAINS, 0, OP_CONST,         3, OP_LIST_CONTAINS, 0, OP_CONST,
        4,        OP_LIST_CONTAINS, 0, OP_CONST,         5, OP_LIST_CONTAINS, 0, OP_CONST,         6, OP_LIST_CONTAINS, 0, OP_CONST,         7, OP_LIST_CONTAINS, 0, OP_HALT
    };
    static Value constValues[] = { "c", "A", "e", "", "invalid", Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN };
    List list1("", "list1");
    list1.push_back("a");
    list1.push_back("b");
    list1.push_back("c");
    list1.push_back("d");
    list1.push_back("e");
    list1.push_back("");
    list1.push_back(Value::SpecialValue::Infinity);
    list1.push_back(8);
    List *lists[] = { &list1 };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.setLists(lists);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 8);
    ASSERT_EQ(vm.getInput(0, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(1, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(2, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(3, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(4, 8)->toBool(), false);
    ASSERT_EQ(vm.getInput(5, 8)->toBool(), false);
    ASSERT_EQ(vm.getInput(6, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(7, 8)->toBool(), false);
}

TEST(VirtualMachineTest, OP_STR_CONCAT)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_CONST, 1, OP_STR_CONCAT, OP_CONST, 2, OP_STR_CONCAT, OP_CONST, 3, OP_STR_CONCAT, OP_HALT };
    static Value constValues[] = { "abc ", "def", " ghi", Value::SpecialValue::NegativeInfinity };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "abc def ghi-Infinity");
}

TEST(VirtualMachineTest, OP_STR_AT)
{
    static unsigned int bytecode[] = {
        OP_START,  OP_CONST, 0, OP_CONST, 1,  OP_STR_AT, OP_CONST, 0, OP_CONST, 2,  OP_STR_AT, OP_CONST, 0, OP_CONST, 3,  OP_STR_AT, OP_CONST, 0, OP_CONST, 4, OP_STR_AT, OP_CONST, 0, OP_CONST, 5,
        OP_STR_AT, OP_CONST, 0, OP_CONST, 6,  OP_STR_AT, OP_CONST, 0, OP_CONST, 7,  OP_STR_AT, OP_CONST, 0, OP_CONST, 8,  OP_STR_AT, OP_CONST, 0, OP_CONST, 9, OP_STR_AT, OP_CONST, 0, OP_CONST, 10,
        OP_STR_AT, OP_CONST, 0, OP_CONST, 11, OP_STR_AT, OP_CONST, 0, OP_CONST, 12, OP_STR_AT, OP_CONST, 0, OP_CONST, 13, OP_STR_AT, OP_HALT
    };
    static Value constValues[] = {
        "abcd efg  hijý", 3, 1, 14, 0, 15, -1, 16, Value::SpecialValue::NegativeInfinity, Value::SpecialValue::Infinity, Value::SpecialValue::NaN, "last", "random", "invalid"
    };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 13);
    ASSERT_EQ(vm.getInput(0, 13)->toString(), "c");
    ASSERT_EQ(vm.getInput(1, 13)->toString(), "a");
    ASSERT_EQ(vm.getInput(2, 13)->toString(), "ý");
    ASSERT_EQ(vm.getInput(3, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(4, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(5, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(6, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(7, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(8, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(9, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(10, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(11, 13)->toString(), "");
    ASSERT_EQ(vm.getInput(12, 13)->toString(), "");
}

TEST(VirtualMachineTest, OP_STR_LENGTH)
{
    static unsigned int bytecode[] = { OP_START, OP_CONST, 0, OP_STR_LENGTH, OP_CONST, 1, OP_STR_LENGTH, OP_HALT };
    static Value constValues[] = { "abcd efg  hijý", "abcĎĐ" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 2);
    ASSERT_EQ(vm.getInput(0, 2)->toDouble(), 14);
    ASSERT_EQ(vm.getInput(1, 2)->toDouble(), 5);
}

TEST(VirtualMachineTest, OP_STR_CONTAINS)
{
    static unsigned int bytecode[] = {
        OP_START,        OP_CONST, 0, OP_CONST, 1, OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 2, OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 3,
        OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 4, OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 5, OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 6,
        OP_STR_CONTAINS, OP_CONST, 0, OP_CONST, 7, OP_STR_CONTAINS, OP_CONST, 4, OP_CONST, 4, OP_STR_CONTAINS, OP_HALT
    };
    static Value constValues[] = { "abcd efg  hijý abcĎĐ", "ĎĐ", "a", "test", "", " ", "  ", "k" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setConstValues(constValues);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 8);
    ASSERT_EQ(vm.getInput(0, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(1, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(2, 8)->toBool(), false);
    ASSERT_EQ(vm.getInput(3, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(4, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(5, 8)->toBool(), true);
    ASSERT_EQ(vm.getInput(6, 8)->toBool(), false);
    ASSERT_EQ(vm.getInput(7, 8)->toBool(), true);
}

unsigned int testFunction1(VirtualMachine *vm)
{
    std::cout << "it works" << std::endl;
    return 0;
}

unsigned int testFunction2(VirtualMachine *vm)
{
    std::cout << vm->getInput(0, 2)->toString() << " " << vm->getInput(1, 2)->toString() << std::endl;
    return 2;
}

unsigned int testFunction3(VirtualMachine *vm)
{
    vm->stop(true, false, false);
    return 1;
}

unsigned int testFunction4(VirtualMachine *vm)
{
    vm->stop(true, true, false);
    return 1;
}

unsigned int testFunction5(VirtualMachine *vm)
{
    vm->stop(true, false, true);
    std::cout << "function 5" << std::endl;
    return 0;
}

unsigned int testFunction6(VirtualMachine *vm)
{
    vm->addReturnValue(0);
    vm->stop(true, false, false);
    return 1;
}

unsigned int testFunction7(VirtualMachine *vm)
{
    vm->addReturnValue(0);
    vm->stop(true, false, false);
    vm->reset();
    return 1;
}

TEST(VirtualMachineTest, OP_EXEC)
{
    static unsigned int bytecode[] = {
        OP_START, OP_EXEC, 0, OP_CONST, 0, OP_CONST, 1, OP_EXEC, 1, OP_NULL, OP_EXEC, 2, OP_CONST, 0, OP_PRINT, OP_NULL, OP_EXEC, 3, OP_CONST, 1, OP_PRINT, OP_NULL, OP_EXEC, 4, OP_HALT
    };
    static BlockFunc functions[] = { &testFunction1, &testFunction2, &testFunction3, &testFunction4, &testFunction5 };
    static Value constValues[] = { "hello", "world" };

    EngineMock engineMock;

    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "it works\nhello world\n");
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(engineMock, breakFrame()).Times(1);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\n");
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "world\nfunction 5\n");
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 1);

    for (int i = 0; i < 100; i++) {
        EXPECT_CALL(engineMock, breakFrame()).Times(0);
        testing::internal::CaptureStdout();
        vm.run();
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "function 5\n");
        ASSERT_FALSE(vm.atEnd());
        ASSERT_EQ(vm.registerCount(), 1);
    }
}

TEST(VirtualMachineTest, Functions)
{
    static BlockFunc functions[] = { &testFunction1, &testFunction2 };

    VirtualMachine vm;
    ASSERT_EQ(vm.functions(), nullptr);
    vm.setFunctions(functions);
    ASSERT_EQ(vm.functions(), functions);
}

TEST(VirtualMachineTest, RunProcedures)
{
    static unsigned int bytecode[] = {
        OP_START, OP_INIT_PROCEDURE, OP_CONST, 0, OP_ADD_ARG, OP_CONST, 1, OP_ADD_ARG, OP_CALL_PROCEDURE, 0, OP_NULL, OP_EXEC, 0, OP_INIT_PROCEDURE, OP_CALL_PROCEDURE, 1, OP_HALT
    };
    static unsigned int procedure1[] = { OP_START, OP_READ_ARG, 0, OP_PRINT, OP_READ_ARG, 1, OP_PRINT, OP_HALT };
    static unsigned int procedure2[] = { OP_START, OP_CONST, 2, OP_PRINT, OP_HALT };
    static unsigned int *procedures[] = { procedure1, procedure2 };
    static BlockFunc functions[] = { &testFunction3 };
    static Value constValues[] = { "hello", "world", "test" };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setProcedures(procedures);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "hello\nworld\n");
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    testing::internal::CaptureStdout();
    vm.run();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_BREAK_ATOMIC)
{
    static unsigned int bytecode1[] = { OP_START, OP_FOREVER_LOOP, OP_BREAK_ATOMIC, OP_LOOP_END, OP_HALT };
    static unsigned int bytecode2[] = { OP_START, OP_CONST, 1, OP_REPEAT_LOOP, OP_BREAK_ATOMIC, OP_LOOP_END, OP_HALT };
    static unsigned int bytecode3[] = {
        OP_START, OP_CONST, 0, OP_SET_VAR, 0, OP_UNTIL_LOOP, OP_READ_VAR, 0, OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP, OP_BREAK_ATOMIC, OP_CONST, 2, OP_CHANGE_VAR, 0, OP_LOOP_END, OP_HALT
    };
    static unsigned int bytecode4[] = { OP_START, OP_BREAK_ATOMIC, OP_NULL, OP_EXEC, 0, OP_HALT };
    static BlockFunc functions[] = { &testFunction3 };
    static Value constValues[] = { 0, 10, 1 };
    Value var;
    static Value *variables[] = { &var };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.setVariables(variables);

    for (int i = 0; i < 100; i++) {
        EXPECT_CALL(engineMock, breakFrame()).Times(1);
        vm.run();
        ASSERT_FALSE(vm.atEnd());
    }

    ASSERT_EQ(vm.registerCount(), 0);

    vm.setBytecode(bytecode2);

    for (int i = 0; i < 10; i++) {
        EXPECT_CALL(engineMock, breakFrame()).Times(1);
        vm.run();
        ASSERT_FALSE(vm.atEnd());
        ASSERT_EQ(vm.registerCount(), 0);
    }

    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    vm.setBytecode(bytecode3);

    for (int i = 0; i < 10; i++) {
        EXPECT_CALL(engineMock, breakFrame()).Times(1);
        vm.run();
        ASSERT_FALSE(vm.atEnd());
        ASSERT_EQ(vm.registerCount(), 0);
    }

    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    vm.setBytecode(bytecode4);
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, OP_WARP)
{
    static unsigned int bytecode1[] = { OP_START, OP_WARP, OP_CONST, 1, OP_REPEAT_LOOP, OP_BREAK_ATOMIC, OP_LOOP_END, OP_HALT };
    static unsigned int bytecode2[] = {
        OP_START, OP_WARP, OP_CONST, 0, OP_SET_VAR, 0, OP_UNTIL_LOOP, OP_READ_VAR, 0, OP_CONST, 1, OP_EQUALS, OP_BEGIN_UNTIL_LOOP, OP_BREAK_ATOMIC, OP_CONST, 2, OP_CHANGE_VAR, 0, OP_LOOP_END, OP_HALT
    };
    static unsigned int bytecode3[] = { OP_START, OP_WARP, OP_BREAK_ATOMIC, OP_NULL, OP_EXEC, 0, OP_HALT };
    static BlockFunc functions[] = { &testFunction3 };
    static Value constValues[] = { 0, 10, 1 };
    Value var;
    static Value *variables[] = { &var };

    EngineMock engineMock;
    VirtualMachine vm(nullptr, &engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.setVariables(variables);

    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    vm.setBytecode(bytecode2);

    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);

    vm.setBytecode(bytecode3);
    EXPECT_CALL(engineMock, breakFrame()).Times(0);
    vm.run();
    ASSERT_TRUE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 0);
}

TEST(VirtualMachineTest, Reset)
{
    static unsigned int bytecode1[] = { OP_START, OP_NULL, OP_EXEC, 0, OP_HALT };
    static unsigned int bytecode2[] = { OP_START, OP_NULL, OP_EXEC, 1, OP_HALT };
    static BlockFunc functions[] = { &testFunction6, &testFunction7 };

    VirtualMachine vm;
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);

    vm.run();
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 1);
    vm.reset();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_TRUE(vm.atEnd());

    vm.reset();
    ASSERT_FALSE(vm.atEnd());

    vm.setBytecode(bytecode2);

    vm.run();
    ASSERT_FALSE(vm.atEnd());
    ASSERT_EQ(vm.registerCount(), 1);
    vm.reset();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_FALSE(vm.atEnd());
    vm.run();
    ASSERT_TRUE(vm.atEnd());

    vm.reset();
    ASSERT_FALSE(vm.atEnd());
}
