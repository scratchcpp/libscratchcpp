#include <scratchcpp/script.h>
#include <scratchcpp/block.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class ScriptTest : public testing::Test
{
    public:
        Target m_target;
        EngineMock m_engine;
};

TEST_F(ScriptTest, Constructors)
{
    auto block = std::make_shared<Block>("", "");
    Script script(&m_target, block, &m_engine);
    ASSERT_EQ(script.target(), &m_target);
    ASSERT_EQ(script.topBlock(), block);
}

TEST_F(ScriptTest, Bytecode)
{
    Script script(nullptr, nullptr, nullptr);
    ASSERT_EQ(script.bytecode(), nullptr);
    ASSERT_TRUE(script.bytecodeVector().empty());

    script.setBytecode({ vm::OP_START, vm::OP_HALT });
    ASSERT_EQ(script.bytecode()[0], vm::OP_START);
    ASSERT_EQ(script.bytecode()[1], vm::OP_HALT);
    ASSERT_EQ(script.bytecodeVector(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
}

static Target *stageTest = nullptr;
static IEngine *engineTest = nullptr;
static Script *scriptTest = nullptr;

TEST_F(ScriptTest, HatPredicate)
{
    Script script(&m_target, nullptr, &m_engine);
    ASSERT_FALSE(script.runHatPredicate());

    Stage stage;
    EXPECT_CALL(m_engine, stage()).Times(3).WillRepeatedly(Return(&stage));

    BlockFunc f1 = [](VirtualMachine *vm) -> unsigned int {
        vm->addReturnValue(true);
        vm->stop(false, false, false);
        stageTest = vm->target();
        engineTest = vm->engine();
        scriptTest = vm->script();
        return 0;
    };

    BlockFunc f2 = f1;

    BlockFunc f3 = [](VirtualMachine *vm) -> unsigned int {
        vm->addReturnValue(false);
        vm->stop(false, false, false);
        stageTest = vm->target();
        engineTest = vm->engine();
        scriptTest = vm->script();
        return 0;
    };

    std::vector<BlockFunc> functions1 = { f1 };
    std::vector<BlockFunc> functions2 = { f1, f2, f3 };

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions1));
    stageTest = nullptr;
    engineTest = nullptr;
    scriptTest = nullptr;
    script.setConstValues({ "test" });
    script.setHatPredicateBytecode({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_EXEC, 0, vm::OP_HALT });
    testing::internal::CaptureStdout();
    ASSERT_TRUE(script.runHatPredicate());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test\n");
    ASSERT_EQ(stageTest, &stage);
    ASSERT_EQ(engineTest, &m_engine);
    ASSERT_EQ(scriptTest, &script);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions2));
    stageTest = nullptr;
    engineTest = nullptr;
    scriptTest = nullptr;
    script.setHatPredicateBytecode({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_EXEC, 1, vm::OP_HALT });
    script.setConstValues({ 5 });
    testing::internal::CaptureStdout();
    ASSERT_TRUE(script.runHatPredicate());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "5\n");
    ASSERT_EQ(stageTest, &stage);
    ASSERT_EQ(engineTest, &m_engine);
    ASSERT_EQ(scriptTest, &script);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions2));
    stageTest = nullptr;
    engineTest = nullptr;
    scriptTest = nullptr;
    script.setHatPredicateBytecode({ vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT });
    ASSERT_FALSE(script.runHatPredicate());
    ASSERT_EQ(stageTest, &stage);
    ASSERT_EQ(engineTest, &m_engine);
    ASSERT_EQ(scriptTest, &script);
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
    static std::vector<BlockFunc> noFunctions;
    static std::vector<Value> constValues = { "test" };

    std::shared_ptr<Variable> var1 = std::make_unique<Variable>("a", "", Value());
    std::shared_ptr<Variable> var2 = std::make_unique<Variable>("b", "", Value());
    static std::vector<Variable *> variables = { var1.get(), var2.get() };

    std::shared_ptr<List> list1 = std::make_unique<List>("c", "");
    std::shared_ptr<List> list2 = std::make_unique<List>("d", "");
    static std::vector<List *> lists = { list1.get(), list2.get() };

    Script script1(nullptr, nullptr, nullptr);

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

    Script script2(&m_target, nullptr, &m_engine);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(noFunctions));
    vm = script2.start();
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), &m_target);
    ASSERT_EQ(vm->engine(), &m_engine);

    Script script3(&m_target, nullptr, &m_engine);
    script3.setBytecode(bytecode);
    script3.setProcedures(procedures);
    script3.setConstValues(constValues);
    script3.setVariables(variables);
    script3.setLists(lists);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions));
    vm = script3.start();
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->bytecode()[0], bytecode[0]);
    ASSERT_EQ(vm->procedures()[0], procedures[0]);
    ASSERT_EQ(vm->functions()[0], functions[0]);
    ASSERT_EQ(vm->constValues()[0].toString(), constValues[0].toString());
    ASSERT_EQ(vm->variables()[0], variables[0]->valuePtr());
    ASSERT_EQ(vm->lists()[0], lists[0]);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions));
    Target target;
    target.addVariable(var1);
    target.addList(list1);
    vm = script3.start(&target);
    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), &target);
    ASSERT_EQ(vm->bytecode()[0], bytecode[0]);
    ASSERT_EQ(vm->procedures()[0], procedures[0]);
    ASSERT_EQ(vm->functions()[0], functions[0]);
    ASSERT_EQ(vm->constValues()[0].toString(), constValues[0].toString());
    ASSERT_EQ(vm->variables()[0], variables[0]->valuePtr());
    ASSERT_EQ(vm->lists()[0], lists[0]);

    Sprite root;
    root.setEngine(&m_engine);
    root.addVariable(var2);
    root.addList(list2);

    EXPECT_CALL(m_engine, cloneLimit()).Times(2).WillRepeatedly(Return(300));
    EXPECT_CALL(m_engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(m_engine, initClone).Times(1);
    EXPECT_CALL(m_engine, moveSpriteBehindOther(_, &root));
    EXPECT_CALL(m_engine, requestRedraw());
    auto clone = root.clone();

    Script script4(&root, nullptr, &m_engine);
    script4.setBytecode(bytecode);
    script4.setProcedures(procedures);
    script4.setConstValues(constValues);
    script4.setVariables(variables);
    script4.setLists(lists);

    EXPECT_CALL(m_engine, blockFunctions()).WillOnce(ReturnRef(functions));
    vm = script4.start(clone.get());

    ASSERT_TRUE(vm);
    ASSERT_EQ(vm->target(), clone.get());
    ASSERT_EQ(vm->bytecode()[0], bytecode[0]);
    ASSERT_EQ(vm->procedures()[0], procedures[0]);
    ASSERT_EQ(vm->functions()[0], functions[0]);
    ASSERT_EQ(vm->constValues()[0].toString(), constValues[0].toString());
    ASSERT_EQ(vm->variables()[0], variables[0]->valuePtr());
    ASSERT_EQ(vm->variables()[1], clone->variableAt(clone->findVariableById("b"))->valuePtr());
    ASSERT_EQ(vm->lists()[0], lists[0]);
    ASSERT_EQ(vm->lists()[1], clone->listAt(clone->findListById("d")).get());

    EXPECT_CALL(m_engine, deinitClone(clone));
    clone->deleteClone();
}
