#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>

#include "project_p.h"
#include "engine/internal/engine.h"
#include "internal/scratch3reader.h"
#include "../common.h"
#include "testblocksection.h"

#define INIT_COMPILER(engineName, compilerName)                                                                                                                                                        \
    Engine engineName;                                                                                                                                                                                 \
    Compiler compilerName(&engineName);

#define LOAD_PROJECT(fileName, engineName)                                                                                                                                                             \
    Engine engineName;                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        Scratch3Reader reader;                                                                                                                                                                         \
        reader.setFileName(fileName);                                                                                                                                                                  \
        ASSERT_TRUE(reader.load());                                                                                                                                                                    \
        ASSERT_TRUE(reader.isValid());                                                                                                                                                                 \
        engineName.setTargets(reader.targets());                                                                                                                                                       \
        engineName.setBroadcasts(reader.broadcasts());                                                                                                                                                 \
        engineName.setExtensions(reader.extensions());                                                                                                                                                 \
    }

using namespace libscratchcpp;

class CompilerTest : public testing::Test
{
    public:
        void SetUp() override { m_section = std::make_shared<TestBlockSection>(); }

        std::shared_ptr<IBlockSection> m_section;
};

TEST_F(CompilerTest, Constructors)
{
    Engine engine;
    Compiler compiler1(&engine);
    ASSERT_EQ(compiler1.engine(), &engine);
    ASSERT_EQ(compiler1.target(), nullptr);

    Sprite sprite;
    Compiler compiler2(&engine, &sprite);
    ASSERT_EQ(compiler2.engine(), &engine);
    ASSERT_EQ(compiler2.target(), &sprite);
}

TEST_F(CompilerTest, Block)
{
    Engine engine;
    Compiler compiler(&engine);
    ASSERT_EQ(compiler.block(), nullptr);
    auto block = std::make_shared<Block>("", "");
    compiler.setBlock(block);
    ASSERT_EQ(compiler.block(), block);
}

TEST_F(CompilerTest, Init)
{
    Engine engine;
    Compiler compiler(&engine);
    ASSERT_TRUE(compiler.bytecode().empty());

    BlockPrototype prototype;
    compiler.setProcedurePrototype(&prototype);
    compiler.init();
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START }));
    ASSERT_EQ(compiler.procedurePrototype(), nullptr);

    compiler.setProcedurePrototype(&prototype);
    compiler.init();
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START }));
    ASSERT_EQ(compiler.procedurePrototype(), &prototype);
}

TEST_F(CompilerTest, End)
{
    Engine engine;
    Compiler compiler(&engine);
    ASSERT_TRUE(compiler.bytecode().empty());

    compiler.end();
    ASSERT_TRUE(compiler.bytecode().empty());

    compiler.init();
    compiler.end();
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    compiler.end();
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));

    compiler.init();
    compiler.init();
    compiler.end();
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));

    compiler.init();
}

TEST_F(CompilerTest, ConstValues)
{
    InputValue v1;
    v1.setValue(5.3);

    InputValue v2;
    v2.setValue(10);

    InputValue v3;
    v3.setValue("test");

    INIT_COMPILER(engine, compiler);
    ASSERT_EQ(compiler.constIndex(&v1), 0);
    ASSERT_EQ(compiler.constIndex(&v2), 1);
    ASSERT_EQ(compiler.constIndex(&v3), 2);
    ASSERT_EQ(compiler.constIndex(&v2), 1); // test index of value that was added before

    ASSERT_EQ(compiler.constInputValues(), std::vector<InputValue *>({ &v1, &v2, &v3 }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 5.3, 10, "test" }));
}

TEST_F(CompilerTest, Variables)
{
    auto v1 = std::make_shared<Variable>("", "var1");
    auto v2 = std::make_shared<Variable>("", "var2");
    auto v3 = std::make_shared<Variable>("", "var3");

    INIT_COMPILER(engine, compiler);
    ASSERT_EQ(compiler.variableIndex(v1), 0);
    ASSERT_EQ(compiler.variableIndex(v2), 1);
    ASSERT_EQ(compiler.variableIndex(v3), 2);
    ASSERT_EQ(compiler.variableIndex(v2), 1); // test index of variable that was added before

    ASSERT_EQ(compiler.variables(), std::vector<Variable *>({ v1.get(), v2.get(), v3.get() }));
    ASSERT_EQ(compiler.variablePtrs(), std::vector<Value *>({ v1->valuePtr(), v2->valuePtr(), v3->valuePtr() }));
}

TEST_F(CompilerTest, Lists)
{
    auto l1 = std::make_shared<List>("", "var1");
    auto l2 = std::make_shared<List>("", "var2");
    auto l3 = std::make_shared<List>("", "var3");

    INIT_COMPILER(engine, compiler);
    ASSERT_EQ(compiler.listIndex(l1), 0);
    ASSERT_EQ(compiler.listIndex(l2), 1);
    ASSERT_EQ(compiler.listIndex(l3), 2);
    ASSERT_EQ(compiler.listIndex(l2), 1); // test index of list that was added before

    ASSERT_EQ(compiler.lists(), std::vector<List *>({ l1.get(), l2.get(), l3.get() }));
}

TEST_F(CompilerTest, EmptyProject)
{
    ProjectPrivate p("empty_project.sb3", ScratchVersion::Scratch3);
    ASSERT_TRUE(p.load());
    auto engine = p.engine;
    ASSERT_EQ(engine->targets().size(), 1);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 0);
    Stage *stage = dynamic_cast<Stage *>(engine->targetAt(0));
    ASSERT_EQ(stage->blocks().size(), 0);
}

TEST_F(CompilerTest, ResolveIds)
{
    ProjectPrivate p("resolve_id_test.sb3", ScratchVersion::Scratch3);
    ASSERT_TRUE(p.load());
    auto engine = p.engine;
    ASSERT_EQ(engine->targets().size(), 2);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 1);

    // Stage
    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);
    ASSERT_EQ(stage->blocks().size(), 2);
    auto block = stage->greenFlagBlocks().at(0);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->parentId(), "");
    ASSERT_FALSE(block->parent());
    std::string greenFlagClickedId = block->id();
    std::string id = block->nextId();

    block = block->next();
    ASSERT_EQ(block->parentId(), greenFlagClickedId);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    ASSERT_EQ(block->nextId(), "");
    ASSERT_FALSE(block->next());

    // Sprite1
    ASSERT_NE(engine->findTarget("Sprite1"), -1);
    Sprite *sprite1 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1")));
    ASSERT_TRUE(sprite1);
    ASSERT_EQ(sprite1->blocks().size(), 5);
    block = sprite1->greenFlagBlocks().at(0);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->parentId(), "");
    ASSERT_FALSE(block->parent());
    greenFlagClickedId = block->id();
    id = block->nextId();

    block = block->next();
    ASSERT_EQ(block->parentId(), greenFlagClickedId);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    id = block->nextId();

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    ASSERT_INPUT(block, "BROADCAST_INPUT");
    auto v = GET_INPUT(block, "BROADCAST_INPUT")->primaryValue();
    ASSERT_EQ(v->valuePtr()->id(), v->valueId());

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_FIELD(block, "VARIABLE");
    auto field = GET_FIELD(block, "VARIABLE");
    ASSERT_EQ(field->valuePtr()->id(), field->valueId());
    ASSERT_VAR(stage, "my variable");
    auto var = GET_VAR(stage, "my variable");
    ASSERT_EQ(var->id(), field->valueId());
    ASSERT_EQ(var, field->valuePtr());

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_FIELD(block, "LIST");
    field = GET_FIELD(block, "LIST");
    ASSERT_EQ(field->valuePtr()->id(), field->valueId());
    ASSERT_LIST(stage, "list");
    auto list = GET_LIST(stage, "list");
    ASSERT_EQ(list->id(), field->valueId());
    ASSERT_EQ(list, field->valuePtr());
    ASSERT_EQ(block->nextId(), "");
    ASSERT_FALSE(block->next());
}

TEST_F(CompilerTest, AddInstruction)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);
    compiler.addInstruction(vm::OP_NULL);
    compiler.addInstruction(vm::OP_CONST, { 0 });
    compiler.addInstruction(vm::OP_ADD);
    compiler.addInstruction(vm::OP_PRINT);
    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_NULL, vm::OP_CONST, 0, vm::OP_ADD, vm::OP_PRINT, vm::OP_HALT }));
}

void compileTestBlock(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_CHECKPOINT); // doesn't do anything, but is used in tests
    compiler->addInstruction(vm::OP_NULL);
}

TEST_F(CompilerTest, AddShadowInput)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);

    Input input("TEST_INPUT", Input::Type::Shadow);
    input.setPrimaryValue("test");
    compiler.addInput(&input);
    compiler.addInstruction(vm::OP_PRINT);

    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_HALT }));
}

TEST_F(CompilerTest, AddObscuredInput)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);

    Input input1("TEST_INPUT1", Input::Type::ObscuredShadow);
    std::shared_ptr<Block> block1 = std::make_shared<Block>("", "test_block1");
    block1->setCompileFunction(&compileTestBlock);
    input1.setValueBlock(block1);
    compiler.addInput(&input1);
    compiler.addInstruction(vm::OP_PRINT);

    Input input2("TEST_INPUT2", Input::Type::ObscuredShadow);
    compiler.addInput(&input2);
    compiler.addInstruction(vm::OP_PRINT);

    Input input3("TEST_INPUT3", Input::Type::ObscuredShadow);
    std::shared_ptr<Block> block2 = std::make_shared<Block>("", "test_block2");
    input3.setValueBlock(block2);
    compiler.addInput(&input3);
    compiler.addInstruction(vm::OP_PRINT);

    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CHECKPOINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_HALT }));
}

TEST_F(CompilerTest, AddNoShadowInput)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);

    Input input1("TEST_INPUT1", Input::Type::NoShadow);
    std::shared_ptr<Block> block1 = std::make_shared<Block>("", "test_block1");
    block1->setCompileFunction(&compileTestBlock);
    input1.setValueBlock(block1);
    compiler.addInput(&input1);
    compiler.addInstruction(vm::OP_PRINT);

    Input input2("TEST_INPUT2", Input::Type::NoShadow);
    input2.setPrimaryValue("test");
    compiler.addInput(&input2);

    Input input3("TEST_INPUT3", Input::Type::NoShadow);
    std::shared_ptr<Block> block2 = std::make_shared<Block>("", "test_block2");
    input3.setValueBlock(block2);
    compiler.addInput(&input3);
    compiler.addInstruction(vm::OP_PRINT);

    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CHECKPOINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_CONST, 0, vm::OP_NULL, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0], "test");
}

TEST_F(CompilerTest, ResolveInput)
{
    INIT_COMPILER(engine, compiler);

    engine.registerSection(m_section);

    auto block = std::make_shared<Block>("a", "test_block1");
    auto input = std::make_shared<Input>("INPUT1", Input::Type::Shadow);
    input->setPrimaryValue("test");
    input->setInputId(TestBlockSection::INPUT1);
    block->addInput(input);
    block->setCompileFunction(&TestBlockSection::compileTestBlock1);

    compiler.compile(block);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test" }));
}

TEST_F(CompilerTest, ResolveDropdownMenuInput)
{
    INIT_COMPILER(engine, compiler);

    engine.registerSection(m_section);

    auto block = std::make_shared<Block>("a", "test_block1");
    auto input = std::make_shared<Input>("INPUT1", Input::Type::Shadow);
    auto menu = std::make_shared<Block>("a", "test_menu");
    menu->setShadow(true);
    auto optionField = std::make_shared<Field>("OPTION", "test");
    input->setInputId(TestBlockSection::INPUT1);
    input->setValueBlock(menu);
    menu->addField(optionField);
    block->addInput(input);
    block->setCompileFunction(&TestBlockSection::compileTestBlock1);

    compiler.compile(block);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test" }));
}

TEST_F(CompilerTest, AddConstValue)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);

    compiler.addConstValue(50);
    compiler.addInstruction(vm::OP_PRINT);
    compiler.addConstValue(50);
    compiler.addInstruction(vm::OP_PRINT);
    compiler.addConstValue("hello");
    compiler.addInstruction(vm::OP_PRINT);
    compiler.addConstValue("world");
    compiler.addInstruction(vm::OP_PRINT);

    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_PRINT, vm::OP_CONST, 1, vm::OP_PRINT, vm::OP_CONST, 2, vm::OP_PRINT, vm::OP_CONST, 3, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 50, 50, "hello", "world" }));
}

TEST_F(CompilerTest, ResolveField)
{
    INIT_COMPILER(engine, compiler);

    engine.registerSection(m_section);

    auto block = std::make_shared<Block>("a", "test_block2");
    auto field = std::make_shared<Field>("FIELD1", "test");
    field->setFieldId(TestBlockSection::FIELD1);
    field->setSpecialValueId(TestBlockSection::TestValue);
    block->addField(field);
    block->setCompileFunction(&TestBlockSection::compileTestBlock2);

    compiler.compile(block);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_NULL, vm::OP_PRINT, vm::OP_HALT }));
}

unsigned int testFunction1(VirtualMachine *vm)
{
    return 0;
}

unsigned int testFunction2(VirtualMachine *vm)
{
    std::cout << vm->getInput(0, 1)->toString() << std::endl;
    return 1;
}

TEST_F(CompilerTest, AddFunctionCall)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);
    compiler.addFunctionCall(&testFunction1);
    compiler.addFunctionCall(&testFunction2);
    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(engine.functionIndex(&testFunction1), 0);
    ASSERT_EQ(engine.functionIndex(&testFunction2), 1);
}

TEST_F(CompilerTest, Warp)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);
    compiler.warp();
    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_WARP, vm::OP_HALT }));
}

TEST_F(CompilerTest, Procedures)
{
    std::string p1 = "procedure 1";
    std::string p2 = "procedure 2";
    std::string p3 = "procedure 3";

    INIT_COMPILER(engine, compiler);
    ASSERT_EQ(compiler.procedureIndex(p1), 0);
    ASSERT_EQ(compiler.procedureIndex(p2), 1);
    ASSERT_EQ(compiler.procedureIndex(p3), 2);
    ASSERT_EQ(compiler.procedureIndex(p2), 1); // test index of procedure that was added before

    ASSERT_EQ(compiler.procedures(), std::vector<std::string>({ p1, p2, p3 }));
}

TEST_F(CompilerTest, ProcedureArgs)
{
    INIT_COMPILER(engine, compiler);
    compiler.addProcedureArg("procedure 1", "arg 1");
    compiler.addProcedureArg("procedure 1", "arg 2");
    compiler.addProcedureArg("procedure 2", "arg 1");
    compiler.addProcedureArg("procedure 2", "arg 1"); // add existing argument
    compiler.addProcedureArg("procedure 2", "arg 2");
    compiler.addProcedureArg("procedure 3", "arg 1");
    compiler.addProcedureArg("procedure 3", "arg 2");
    compiler.addProcedureArg("procedure 3", "arg 3");

    ASSERT_EQ(compiler.procedureArgIndex("procedure 1", "arg 1"), 0);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 1", "arg 2"), 1);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 2", "arg 1"), 0);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 2", "arg 2"), 1);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 3", "arg 1"), 0);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 3", "arg 2"), 1);
    ASSERT_EQ(compiler.procedureArgIndex("procedure 3", "arg 3"), 2);
}

TEST_F(CompilerTest, ProcedurePrototype)
{
    INIT_COMPILER(engine, compiler);
    BlockPrototype prototype;

    ASSERT_EQ(compiler.procedurePrototype(), nullptr);
    compiler.setProcedurePrototype(&prototype);
    ASSERT_EQ(compiler.procedurePrototype(), &prototype);
}

TEST_F(CompilerTest, RepeatLoop)
{
    LOAD_PROJECT("repeat10.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_CONST, 1, vm::OP_REPEAT_LOOP, vm::OP_CONST, 2, vm::OP_CHANGE_VAR, 0, vm::OP_BREAK_FRAME, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.variablePtrs().size(), 1);
    ASSERT_EQ(compiler.variablePtrs()[0]->toString(), "test");
    ASSERT_EQ(compiler.lists().size(), 0);
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 10, 1 }));
}

TEST_F(CompilerTest, EmptyRepeatLoop)
{
    LOAD_PROJECT("repeat_empty.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.variablePtrs().size(), 1);
    ASSERT_EQ(compiler.variablePtrs()[0]->toString(), "test");
    ASSERT_EQ(compiler.lists().size(), 0);
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0], 0);
}

TEST_F(CompilerTest, ForeverLoop)
{
    LOAD_PROJECT("forever_loop.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_FOREVER_LOOP, vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 0, vm::OP_BREAK_FRAME, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1 }));
}

TEST_F(CompilerTest, RepeatUntilLoop)
{
    LOAD_PROJECT("repeat_until.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_SET_VAR,
              0,
              vm::OP_UNTIL_LOOP,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_BEGIN_UNTIL_LOOP,
              vm::OP_CONST,
              1,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_BREAK_FRAME,
              vm::OP_LOOP_END,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1 }));
}

TEST_F(CompilerTest, RepeatWhileLoop)
{
    LOAD_PROJECT("repeat_while.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_SET_VAR,
              0,
              vm::OP_UNTIL_LOOP,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_NOT,
              vm::OP_BEGIN_UNTIL_LOOP,
              vm::OP_CONST,
              1,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_BREAK_FRAME,
              vm::OP_LOOP_END,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1 }));
}

TEST_F(CompilerTest, RepeatForEachLoop)
{
    LOAD_PROJECT("repeat_for_each.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_SET_VAR,
              0,
              vm::OP_CONST,
              1,
              vm::OP_REPEAT_LOOP,
              vm::OP_REPEAT_LOOP_INDEX1,
              vm::OP_SET_VAR,
              1,
              vm::OP_CONST,
              2,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_BREAK_FRAME,
              vm::OP_LOOP_END,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 10, 1 }));
}

TEST_F(CompilerTest, EmptyRepeatForEachLoop)
{
    LOAD_PROJECT("repeat_for_each_empty.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_CONST, 1, vm::OP_SET_VAR, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 10 }));
}

TEST_F(CompilerTest, IfStatement)
{
    LOAD_PROJECT("if.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_NULL, vm::OP_NOT, vm::OP_IF, vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 0, vm::OP_ENDIF, vm::OP_HALT }));
}

TEST_F(CompilerTest, EmptyIfStatement)
{
    LOAD_PROJECT("if_empty.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_HALT }));
}

TEST_F(CompilerTest, IfElseStatement)
{
    LOAD_PROJECT("if_else.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_SET_VAR,
              0,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_IF,
              vm::OP_CONST,
              1,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_ELSE,
              vm::OP_CONST,
              2,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_ENDIF,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1, -1 }));
}

TEST_F(CompilerTest, EmptyIfElseStatement)
{
    LOAD_PROJECT("if_else_empty.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CONST, 0,         vm::OP_SET_VAR, 0, vm::OP_NULL,       vm::OP_NOT, vm::OP_NOT,   vm::OP_IF,  vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 0, vm::OP_ENDIF,
              vm::OP_NULL,  vm::OP_NOT,   vm::OP_IF, vm::OP_CONST,   2, vm::OP_CHANGE_VAR, 0,          vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, -1, 1 }));
}

TEST_F(CompilerTest, NestedStatements)
{
    LOAD_PROJECT("nested_statements.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_SET_VAR,
              0,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_IF,
              vm::OP_CONST,
              1,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_CONST,
              2,
              vm::OP_REPEAT_LOOP,
              vm::OP_NULL,
              vm::OP_IF,
              vm::OP_CONST,
              3,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_ENDIF,
              vm::OP_BREAK_FRAME,
              vm::OP_LOOP_END,
              vm::OP_ELSE,
              vm::OP_CONST,
              4,
              vm::OP_REPEAT_LOOP,
              vm::OP_CONST,
              5,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_IF,
              vm::OP_CONST,
              6,
              vm::OP_CHANGE_VAR,
              0,
              vm::OP_ENDIF,
              vm::OP_BREAK_FRAME,
              vm::OP_LOOP_END,
              vm::OP_ENDIF,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1, 10, 2, 10, -1, 1 }));
}

TEST_F(CompilerTest, CustomBlocks)
{
    LOAD_PROJECT("custom_blocks.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);

    auto stage = engine.targetAt(0);
    auto whenFlagClicked = stage->greenFlagBlocks().at(0);
    compiler.compile(whenFlagClicked);
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_INIT_PROCEDURE,
              vm::OP_CONST,
              0,
              vm::OP_ADD_ARG,
              vm::OP_NULL,
              vm::OP_NOT,
              vm::OP_ADD_ARG,
              vm::OP_CALL_PROCEDURE,
              0,
              vm::OP_INIT_PROCEDURE,
              vm::OP_CALL_PROCEDURE,
              1,
              vm::OP_HALT }));

    std::shared_ptr<Block> definition = nullptr;
    for (auto block : stage->blocks()) {
        if (block->opcode() == "procedures_prototype" && block->mutationPrototype()->procCode() == "test %s %b")
            definition = block->parent();
    }
    ASSERT_TRUE(definition);
    compiler.compile(definition);
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_WARP, vm::OP_CONST, 1, vm::OP_REPEAT_LOOP, vm::OP_READ_ARG, 0, vm::OP_SET_VAR, 0, vm::OP_READ_ARG, 1, vm::OP_SET_VAR, 1, vm::OP_LOOP_END, vm::OP_HALT }));

    definition = nullptr;
    for (auto block : stage->blocks()) {
        if (block->opcode() == "procedures_prototype" && block->mutationPrototype()->procCode() == "no warp test")
            definition = block->parent();
    }
    ASSERT_TRUE(definition);
    compiler.compile(definition);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 2, vm::OP_SET_VAR, 2, vm::OP_HALT }));
}

TEST_F(CompilerTest, MultipleTargets)
{
    LOAD_PROJECT("load_test.sb3", engine);
    engine.compile();
    auto scripts = engine.scripts();

    auto sprite1 = engine.targetAt(engine.findTarget("Sprite1"));
    auto script = scripts.at(sprite1->greenFlagBlocks().at(0));
    ASSERT_EQ(script->bytecodeVector().size(), 33);
    auto vm = script->start();
    ASSERT_EQ(vm->target(), sprite1);
    ASSERT_EQ(vm->engine(), &engine);
    ASSERT_EQ(vm->script(), script.get());

    auto sprite2 = engine.targetAt(engine.findTarget("Balloon1"));
    script = scripts.at(sprite2->greenFlagBlocks().at(0));
    ASSERT_EQ(script->bytecodeVector().size(), 6);
    vm = script->start();
    ASSERT_EQ(vm->target(), sprite2);
    ASSERT_EQ(vm->engine(), &engine);
    ASSERT_EQ(vm->script(), script.get());
}

TEST_F(CompilerTest, EdgeActivatedHatPredicate)
{
    auto hat = std::make_shared<Block>("", "");
    hat->setCompileFunction([](Compiler *) {});
    hat->setHatPredicateCompileFunction([](Compiler *compiler) { compiler->addConstValue(true); });

    auto block = std::make_shared<Block>("", "");
    block->setCompileFunction([](Compiler *compiler) {
        compiler->addConstValue("test");
        compiler->addInstruction(vm::OP_PRINT);
    });
    block->setParent(hat);
    hat->setNext(block);

    INIT_COMPILER(engine, compiler);
    compiler.compile(hat);
    ASSERT_EQ(compiler.hatPredicateBytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 1, vm::OP_PRINT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ true, "test" }));
}
