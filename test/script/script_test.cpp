#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/target.h>
#include <scratchcpp/list.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

class ScriptTest : public testing::Test
{
    public:
        Target m_target;
        EngineMock m_engine;
};

TEST_F(ScriptTest, Constructors)
{
    Script script(&m_target, &m_engine);
    ASSERT_EQ(script.target(), &m_target);
}

TEST_F(ScriptTest, Bytecode)
{
    Script script(nullptr, nullptr);
    ASSERT_EQ(script.bytecode(), nullptr);
    ASSERT_TRUE(script.bytecodeVector().empty());

    script.setBytecode({ vm::OP_START, vm::OP_HALT });
    ASSERT_EQ(script.bytecode()[0], vm::OP_START);
    ASSERT_EQ(script.bytecode()[1], vm::OP_HALT);
    ASSERT_EQ(script.bytecodeVector(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
}

unsigned int testFunction(VirtualMachine *)
{
    return 0;
}

TEST_F(ScriptTest, Start)
{
    static std::vector<unsigned int> bytecode = { vm::OP_START, vm::OP_HALT };
    static std::vector<unsigned int *> procedures = { bytecode.data() };
    static std::vector<BlockFunc> functions = { &testFunction };
    static std::vector<Value> constValues = { "test" };

    std::unique_ptr<Value> var = std::make_unique<Value>();
    static std::vector<Value *> variables = { var.get() };

    std::unique_ptr<List> list = std::make_unique<List>("", "");
    static std::vector<List *> lists = { list.get() };

    Script script1(nullptr, nullptr);

    std::shared_ptr<VirtualMachine> vm = script1.start();
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), nullptr);
    ASSERT_EQ(vm->engine(), nullptr);
    ASSERT_EQ(vm->bytecode(), nullptr);
    ASSERT_EQ(vm->procedures(), nullptr);
    ASSERT_EQ(vm->functions(), nullptr);
    ASSERT_EQ(vm->constValues(), nullptr);
    ASSERT_EQ(vm->variables(), nullptr);
    ASSERT_EQ(vm->lists(), nullptr);

    Script script2(&m_target, &m_engine);

    vm = script2.start();
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), &m_target);
    ASSERT_EQ(vm->engine(), &m_engine);

    Script script3(&m_target, &m_engine);
    script3.setBytecode(bytecode);
    script3.setProcedures(procedures);
    script3.setFunctions(functions);
    script3.setConstValues(constValues);
    script3.setVariables(variables);
    script3.setLists(lists);

    vm = script3.start();
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->bytecode()[0], bytecode[0]);
    ASSERT_EQ(vm->procedures()[0], procedures[0]);
    ASSERT_EQ(vm->functions()[0], functions[0]);
    ASSERT_EQ(vm->constValues()[0].toString(), constValues[0].toString());
    ASSERT_EQ(vm->variables()[0], variables[0]);
    ASSERT_EQ(vm->lists()[0], lists[0]);

    Target target;
    vm = script3.start(&target);
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), &target);
    ASSERT_EQ(vm->bytecode()[0], bytecode[0]);
    ASSERT_EQ(vm->procedures()[0], procedures[0]);
    ASSERT_EQ(vm->functions()[0], functions[0]);
    ASSERT_EQ(vm->constValues()[0].toString(), constValues[0].toString());
    ASSERT_EQ(vm->variables()[0], variables[0]);
    ASSERT_EQ(vm->lists()[0], lists[0]);
}
