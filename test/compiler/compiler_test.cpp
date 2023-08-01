#include "project_p.h"

#include "scratch/stage.h"
#include "scratch/sprite.h"
#include "engine/compiler.h"
#include "engine/script.h"
#include "internal/scratch3reader.h"
#include "../common.h"

#define INIT_COMPILER(engineName, compilerName)                                                                                                                                                        \
    Engine engineName;                                                                                                                                                                                 \
    Compiler compilerName(&engine);

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

TEST(CompilerTest, EmptyProject)
{
    ProjectPrivate p("empty_project.sb3", ScratchVersion::Scratch3);
    ASSERT_TRUE(p.load());
    const Engine *engine = &p.engine;
    ASSERT_EQ(engine->targets().size(), 1);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 0);
    Stage *stage = dynamic_cast<Stage *>(engine->targetAt(0));
    ASSERT_EQ(stage->blocks().size(), 0);
}

TEST(CompilerTest, ResolveIds)
{
    ProjectPrivate p("resolve_id_test.sb3", ScratchVersion::Scratch3);
    ASSERT_TRUE(p.load());
    const Engine *engine = &p.engine;
    ASSERT_EQ(engine->targets().size(), 2);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 1);

    // Stage
    ASSERT_NE(engine->findTarget("Stage"), -1);
    Stage *stage = dynamic_cast<Stage *>(engine->targetAt(engine->findTarget("Stage")));
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

TEST(CompilerTest, AddInstruction)
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

TEST(CompilerTest, AddShadowInput)
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

TEST(CompilerTest, AddObscuredInput)
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

TEST(CompilerTest, AddNoShadowInput)
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
    EXPECT_DEATH({ compiler.addInput(&input2); }, "(Assertion)([ ]+)(.+)([ ]+)(failed)");

    Input input3("TEST_INPUT3", Input::Type::NoShadow);
    std::shared_ptr<Block> block2 = std::make_shared<Block>("", "test_block2");
    input3.setValueBlock(block2);
    compiler.addInput(&input3);
    compiler.addInstruction(vm::OP_PRINT);

    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CHECKPOINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_NULL, vm::OP_PRINT, vm::OP_HALT }));
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

TEST(CompilerTest, AddFunctionCall)
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

TEST(CompilerTest, Warp)
{
    INIT_COMPILER(engine, compiler);
    compiler.addInstruction(vm::OP_START);
    compiler.warp();
    compiler.addInstruction(vm::OP_HALT);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_WARP, vm::OP_HALT }));
}

TEST(CompilerTest, RepeatLoop)
{
    LOAD_PROJECT("repeat10.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_CONST, 1, vm::OP_REPEAT_LOOP, vm::OP_CONST, 2, vm::OP_CHANGE_VAR, 0, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.variablePtrs().size(), 1);
    ASSERT_EQ(compiler.variablePtrs()[0]->toString(), "test");
    ASSERT_EQ(compiler.lists().size(), 0);
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 10, 1 }));
}

TEST(CompilerTest, ForeverLoop)
{
    LOAD_PROJECT("forever_loop.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_FOREVER_LOOP, vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 0, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1 }));
}

TEST(CompilerTest, RepeatUntilLoop)
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
              vm::OP_LOOP_END,
              vm::OP_HALT }));
}

TEST(CompilerTest, RepeatWhileLoop)
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
              vm::OP_LOOP_END,
              vm::OP_HALT }));
}

TEST(CompilerTest, RepeatForEachLoop)
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
              vm::OP_LOOP_END,
              vm::OP_HALT }));
}

TEST(CompilerTest, IfStatement)
{
    LOAD_PROJECT("if.sb3", engine);
    engine.resolveIds();
    Compiler compiler(&engine);
    compiler.compile(engine.targetAt(0)->greenFlagBlocks().at(0));
    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_NULL, vm::OP_NOT, vm::OP_IF, vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 0, vm::OP_ENDIF, vm::OP_HALT }));
}

TEST(CompilerTest, IfElseStatement)
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

TEST(CompilerTest, NestedStatements)
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
              vm::OP_LOOP_END,
              vm::OP_ENDIF,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 0, 1, 10, 2, 10, -1, 1 }));
}

TEST(CompilerTest, CustomBlocks)
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
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_WARP, vm::OP_READ_ARG, 0, vm::OP_SET_VAR, 0, vm::OP_READ_ARG, 1, vm::OP_SET_VAR, 1, vm::OP_HALT }));

    definition = nullptr;
    for (auto block : stage->blocks()) {
        if (block->opcode() == "procedures_prototype" && block->mutationPrototype()->procCode() == "no warp test")
            definition = block->parent();
    }
    ASSERT_TRUE(definition);
    compiler.compile(definition);
    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 1, vm::OP_SET_VAR, 2, vm::OP_HALT }));
}

TEST(CompilerTest, MultipleTargets)
{
    LOAD_PROJECT("load_test.sb3", engine);
    engine.compile();
    auto scripts = engine.scripts();

    auto sprite1 = engine.targetAt(engine.findTarget("Sprite1"));
    auto script = scripts.at(sprite1->greenFlagBlocks().at(0));
    ASSERT_EQ(script->bytecodeVector().size(), 30);
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
