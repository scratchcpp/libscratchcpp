#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <dev/engine/compiler_p.h>
#include <enginemock.h>
#include <targetmock.h>
#include <codebuilderfactorymock.h>
#include <codebuildermock.h>
#include <executablecodemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;

class CompilerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            CompilerPrivate::builderFactory = &m_builderFactory;
            m_builder = std::make_shared<CodeBuilderMock>();
            m_code = std::make_shared<ExecutableCodeMock>();
        }

        void TearDown() override
        {
            CompilerPrivate::builderFactory = nullptr;
            m_builder.reset();
            m_compareBlock.reset();
            m_testVar.reset();
        }

        void compile(Compiler &compiler, std::shared_ptr<Block> block)
        {
            ASSERT_EQ(compiler.block(), nullptr);
            // TODO: Test warp
            EXPECT_CALL(m_builderFactory, create(compiler.target(), block->id(), false)).WillOnce(Return(m_builder));
            EXPECT_CALL(*m_builder, finalize()).WillOnce(Return(m_code));
            ASSERT_EQ(compiler.compile(block), m_code);
            ASSERT_EQ(compiler.block(), nullptr);
        }

        EngineMock m_engine;
        TargetMock m_target;
        CodeBuilderFactoryMock m_builderFactory;
        static inline std::shared_ptr<CodeBuilderMock> m_builder;
        std::shared_ptr<ExecutableCodeMock> m_code;
        static inline std::shared_ptr<Block> m_compareBlock;
        static inline std::shared_ptr<Variable> m_testVar;
};

TEST_F(CompilerTest, Constructors)
{
    Compiler compiler(&m_engine, &m_target);
    ASSERT_EQ(compiler.engine(), &m_engine);
    ASSERT_EQ(compiler.target(), &m_target);
}

TEST_F(CompilerTest, AddFunctionCall)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    m_compareBlock = block;
    block->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->block(), m_compareBlock);
        std::vector<Compiler::StaticType> args = { Compiler::StaticType::Number, Compiler::StaticType::Bool };
        EXPECT_CALL(*m_builder, addFunctionCall("test1", Compiler::StaticType::Void, args));
        compiler->addFunctionCall("test1", Compiler::StaticType::Void, args);

        args = { Compiler::StaticType::String };
        EXPECT_CALL(*m_builder, addFunctionCall("test2", Compiler::StaticType::Bool, args));
        compiler->addFunctionCall("test2", Compiler::StaticType::Bool, args);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddConstValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->addConstValue(1);

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addConstValue("test");

        EXPECT_CALL(*m_builder, addConstValue(Value(3.5)));
        compiler->addConstValue(3.5);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddVariableValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        Variable var1("", ""), var2("", "");
        EXPECT_CALL(*m_builder, addVariableValue(&var1));
        compiler->addVariableValue(&var1);

        EXPECT_CALL(*m_builder, addVariableValue(&var2));
        compiler->addVariableValue(&var2);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddListValue)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        List list1("", ""), list2("", "");
        EXPECT_CALL(*m_builder, addListContents(&list1));
        compiler->addListContents(&list1);

        EXPECT_CALL(*m_builder, addListContents(&list2));
        compiler->addListContents(&list2);
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, AddInput)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    auto valueBlock = std::make_shared<Block>("b", "");
    m_compareBlock = valueBlock;
    valueBlock->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->block(), m_compareBlock);
        compiler->addConstValue("value block");
    });

    auto menu = std::make_shared<Block>("c", "");
    menu->setShadow(true);
    menu->addField(std::make_shared<Field>("MENU", "selected item"));

    auto shadow = std::make_shared<Input>("SHADOW", Input::Type::Shadow);
    shadow->setPrimaryValue("test");
    block->addInput(shadow);

    auto shadowBlock = std::make_shared<Input>("SHADOW_BLOCK", Input::Type::Shadow);
    shadowBlock->setValueBlock(valueBlock);
    block->addInput(shadowBlock);

    auto shadowMenu = std::make_shared<Input>("SHADOW_MENU", Input::Type::Shadow);
    shadowMenu->setValueBlock(menu);
    block->addInput(shadowMenu);

    auto noShadow = std::make_shared<Input>("NO_SHADOW", Input::Type::NoShadow);
    noShadow->setPrimaryValue("test");
    block->addInput(noShadow);

    auto noShadowBlock = std::make_shared<Input>("NO_SHADOW_BLOCK", Input::Type::NoShadow);
    noShadowBlock->setValueBlock(valueBlock);
    block->addInput(noShadowBlock);

    auto noShadowMenu = std::make_shared<Input>("NO_SHADOW_MENU", Input::Type::Shadow);
    noShadowMenu->setValueBlock(menu);
    block->addInput(noShadowMenu);

    auto obscuredShadow = std::make_shared<Input>("OBSCURED_SHADOW", Input::Type::ObscuredShadow);
    obscuredShadow->setPrimaryValue("test");
    block->addInput(obscuredShadow);

    auto obscuredShadowVariable = std::make_shared<Input>("OBSCURED_SHADOW_VARIABLE", Input::Type::ObscuredShadow);
    m_testVar = std::make_shared<Variable>("", "");
    obscuredShadowVariable->primaryValue()->setValuePtr(m_testVar);
    block->addInput(obscuredShadowVariable);

    auto obscuredShadowBlock = std::make_shared<Input>("OBSCURED_SHADOW_BLOCK", Input::Type::ObscuredShadow);
    obscuredShadowBlock->setValueBlock(valueBlock);
    block->addInput(obscuredShadowBlock);

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, addConstValue(Value()));
        compiler->addInput("INVALID");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("SHADOW");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("SHADOW_BLOCK");

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item")));
        compiler->addInput("SHADOW_MENU");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("NO_SHADOW");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("NO_SHADOW_BLOCK");

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item")));
        compiler->addInput("NO_SHADOW_MENU");

        EXPECT_CALL(*m_builder, addConstValue(Value("test")));
        compiler->addInput("OBSCURED_SHADOW");

        EXPECT_CALL(*m_builder, addVariableValue(m_testVar.get()));
        compiler->addInput("OBSCURED_SHADOW_VARIABLE");

        EXPECT_CALL(*m_builder, addConstValue(Value("value block")));
        compiler->addInput("OBSCURED_SHADOW_BLOCK");
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAdd)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAdd);
        compiler->createAdd();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateSub)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createSub);
        compiler->createSub();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateMul)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createMul);
        compiler->createMul();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateDiv)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createDiv);
        compiler->createDiv();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateCmpEQ)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createCmpEQ);
        compiler->createCmpEQ();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateCmpGT)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createCmpGT);
        compiler->createCmpGT();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateCmpLT)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createCmpLT);
        compiler->createCmpLT();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAnd)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAnd);
        compiler->createAnd();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateOr)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createOr);
        compiler->createOr();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateNot)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createNot);
        compiler->createNot();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateMod)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createMod);
        compiler->createMod();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateRound)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createRound);
        compiler->createRound();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAbs)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAbs);
        compiler->createAbs();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateFloor)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createFloor);
        compiler->createFloor();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateCeil)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createCeil);
        compiler->createCeil();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateSqrt)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createSqrt);
        compiler->createSqrt();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateSin)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createSin);
        compiler->createSin();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateCos)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createCos);
        compiler->createCos();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateTan)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createTan);
        compiler->createTan();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAsin)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAsin);
        compiler->createAsin();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAcos)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAcos);
        compiler->createAcos();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateAtan)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createAtan);
        compiler->createAtan();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateLn)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createLn);
        compiler->createLn();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateLog10)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createLog10);
        compiler->createLog10();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateExp)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createExp);
        compiler->createExp();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateExp10)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, createExp10);
        compiler->createExp10();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, CreateVariableWrite)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) {
        auto var = std::make_shared<Variable>("", "");
        EXPECT_CALL(*m_builder, createVariableWrite(var.get()));
        compiler->createVariableWrite(var.get());
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, MoveToIf)
{
    Compiler compiler(&m_engine, &m_target);
    EXPECT_CALL(*m_builder, beginElseBranch).Times(0);

    auto if1 = std::make_shared<Block>("", "if");
    if1->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement).Times(0);
        EXPECT_CALL(*m_builder, endIf).Times(0);
        compiler->moveToIf(nullptr);
    });

    auto if2 = std::make_shared<Block>("", "if");
    if1->setNext(if2);
    if2->setParent(if1);
    if2->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement());
        EXPECT_CALL(*m_builder, addConstValue(Value()));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIf(compiler->input("SUBSTACK")->valueBlock());
    });

    auto substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if2);
    substack1->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(Value()); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    if2->addInput(input);

    compile(compiler, if1);
}

TEST_F(CompilerTest, MoveToIfElse)
{
    Compiler compiler(&m_engine, &m_target);

    auto if1 = std::make_shared<Block>("", "if");
    if1->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement).Times(0);
        EXPECT_CALL(*m_builder, beginElseBranch).Times(0);
        EXPECT_CALL(*m_builder, endIf).Times(0);
        compiler->moveToIfElse(nullptr, nullptr);
    });

    auto if2 = std::make_shared<Block>("", "if");
    if1->setNext(if2);
    if2->setParent(if1);
    if2->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement());
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, beginElseBranch());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
    });

    auto substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if2);
    substack1->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    auto substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(if2);
    substack2->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    if2->addInput(input);
    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    if2->addInput(input);

    // Nested
    auto if3 = std::make_shared<Block>("", "if");
    if2->setNext(if3);
    if3->setParent(if2);
    if3->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement()).Times(3);
        EXPECT_CALL(*m_builder, beginElseBranch()).Times(3);
        EXPECT_CALL(*m_builder, endIf()).Times(3);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, addConstValue(Value(3)));
        EXPECT_CALL(*m_builder, addConstValue(Value(4)));
        compiler->moveToIfElse(compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
    });

    // if
    auto ifSubstack1 = std::make_shared<Block>("", "if");
    ifSubstack1->setParent(if3);
    ifSubstack1->setCompileFunction([](Compiler *compiler) { compiler->moveToIfElse(compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock()); });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(ifSubstack1);
    substack1->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(ifSubstack1);
    substack2->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    ifSubstack1->addInput(input);
    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    ifSubstack1->addInput(input);

    // else
    auto ifSubstack2 = std::make_shared<Block>("", "if");
    ifSubstack2->setParent(if3);
    ifSubstack2->setCompileFunction([](Compiler *compiler) { compiler->moveToIfElse(compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock()); });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(ifSubstack2);
    substack1->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(3); });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(ifSubstack2);
    substack2->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(4); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    ifSubstack2->addInput(input);
    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    ifSubstack2->addInput(input);

    // End if
    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(ifSubstack1);
    if3->addInput(input);
    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(ifSubstack2);
    if3->addInput(input);

    // Empty 'then' branch
    auto if4 = std::make_shared<Block>("", "if");
    if3->setNext(if4);
    if4->setParent(if3);
    if4->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement());
        EXPECT_CALL(*m_builder, beginElseBranch());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(nullptr, compiler->input("SUBSTACK2")->valueBlock());
    });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(if4);
    substack2->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    if4->addInput(input);

    // Empty 'else' branch
    auto if5 = std::make_shared<Block>("", "if");
    if4->setNext(if5);
    if5->setParent(if4);
    if5->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginIfStatement());
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, beginElseBranch()).Times(0);
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(compiler->input("SUBSTACK")->valueBlock(), nullptr);
    });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if5);
    substack1->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    if5->addInput(input);

    // Code after the if statement
    auto block = std::make_shared<Block>("", "");
    block->setParent(if5);
    if5->setNext(block);
    block->setCompileFunction([](Compiler *compiler) { compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(compiler, if1);
}

TEST_F(CompilerTest, MoveToRepeatLoop)
{
    Compiler compiler(&m_engine, &m_target);

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(nullptr);
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatLoop());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatLoop()).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToRepeatLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) { compiler->moveToRepeatLoop(compiler->input("SUBSTACK")->valueBlock()); });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    loopSubstack->addInput(input);

    // End loop
    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(loopSubstack);
    l3->addInput(input);

    // Empty loop body
    auto l4 = std::make_shared<Block>("", "loop");
    l3->setNext(l4);
    l4->setParent(l3);
    l4->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(nullptr);
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) { compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(compiler, l1);
}

TEST_F(CompilerTest, MoveToWhileLoop)
{
    Compiler compiler(&m_engine, &m_target);

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginWhileLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(nullptr);
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginWhileLoop());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginWhileLoop()).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToWhileLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) { compiler->moveToWhileLoop(compiler->input("SUBSTACK")->valueBlock()); });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    loopSubstack->addInput(input);

    // End loop
    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(loopSubstack);
    l3->addInput(input);

    // Empty loop body
    auto l4 = std::make_shared<Block>("", "loop");
    l3->setNext(l4);
    l4->setParent(l3);
    l4->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginWhileLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(nullptr);
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) { compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(compiler, l1);
}

TEST_F(CompilerTest, MoveToRepeatUntilLoop)
{
    Compiler compiler(&m_engine, &m_target);

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(nullptr);
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop()).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToRepeatUntilLoop(compiler->input("SUBSTACK")->valueBlock());
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) { compiler->moveToRepeatUntilLoop(compiler->input("SUBSTACK")->valueBlock()); });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) { compiler->addConstValue(1); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    loopSubstack->addInput(input);

    // End loop
    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(loopSubstack);
    l3->addInput(input);

    // Empty loop body
    auto l4 = std::make_shared<Block>("", "loop");
    l3->setNext(l4);
    l4->setParent(l3);
    l4->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop());
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(nullptr);
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) { compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(compiler, l1);
}

TEST_F(CompilerTest, BeginLoopCondition)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) {
        EXPECT_CALL(*m_builder, beginLoopCondition());
        compiler->beginLoopCondition();
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, Input)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->addInput(std::make_shared<Input>("TEST", Input::Type::Shadow));
    block->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->input("INVALID"), nullptr);
        ASSERT_EQ(compiler->input("TEST"), compiler->block()->inputAt(0).get());
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, Field)
{
    Compiler compiler(&m_engine, &m_target);
    auto block = std::make_shared<Block>("a", "");
    block->addField(std::make_shared<Field>("TEST", "test"));
    block->setCompileFunction([](Compiler *compiler) {
        ASSERT_EQ(compiler->field("INVALID"), nullptr);
        ASSERT_EQ(compiler->field("TEST"), compiler->block()->fieldAt(0).get());
    });

    compile(compiler, block);
}

TEST_F(CompilerTest, UnsupportedBlocks)
{
    auto valueBlock1 = std::make_shared<Block>("v1", "value_block1");
    auto valueBlock2 = std::make_shared<Block>("v2", "value_block2");
    valueBlock2->setCompileFunction([](Compiler *) {});

    auto valueBlock3 = std::make_shared<Block>("v3", "value_block3");
    auto valueBlock4 = std::make_shared<Block>("v4", "value_block4");
    valueBlock4->setCompileFunction([](Compiler *) {});

    auto valueBlock5 = std::make_shared<Block>("v5", "value_block5");
    auto valueBlock6 = std::make_shared<Block>("v6", "value_block6");
    valueBlock6->setCompileFunction([](Compiler *) {});

    auto block1 = std::make_shared<Block>("b1", "block1");

    auto block2 = std::make_shared<Block>("b2", "block2");
    block2->setParent(block1);
    block1->setNext(block2);

    auto block3 = std::make_shared<Block>("b3", "block3");
    auto input1 = std::make_shared<Input>("INPUT1", Input::Type::Shadow);
    input1->setValueBlock(valueBlock1);
    block3->addInput(input1);
    auto input2 = std::make_shared<Input>("INPUT2", Input::Type::Shadow);
    input2->setValueBlock(valueBlock2);
    block3->addInput(input2);
    auto input3 = std::make_shared<Input>("INPUT3", Input::Type::NoShadow);
    input3->setValueBlock(valueBlock3);
    block3->addInput(input3);
    auto input4 = std::make_shared<Input>("INPUT4", Input::Type::NoShadow);
    input4->setValueBlock(valueBlock4);
    block3->addInput(input4);
    auto input5 = std::make_shared<Input>("INPUT5", Input::Type::ObscuredShadow);
    input5->setValueBlock(valueBlock5);
    block3->addInput(input5);
    auto input6 = std::make_shared<Input>("INPUT6", Input::Type::ObscuredShadow);
    input6->setValueBlock(valueBlock6);
    block3->addInput(input6);
    block3->setCompileFunction([](Compiler *compiler) {
        compiler->addInput("INPUT1");
        compiler->addInput("INPUT2");
        compiler->addInput("INPUT3");
        compiler->addInput("INPUT4");
        compiler->addInput("INPUT5");
        compiler->addInput("INPUT6");
    });
    block3->setParent(block2);
    block2->setNext(block3);

    auto block4 = std::make_shared<Block>("b4", "block4");
    block4->setParent(block3);
    block3->setNext(block4);

    Compiler compiler(&m_engine, &m_target);
    EXPECT_CALL(*m_builder, addConstValue).WillRepeatedly(Return());
    compile(compiler, block1);

    ASSERT_EQ(compiler.unsupportedBlocks(), std::unordered_set<std::string>({ "block1", "block2", "value_block1", "value_block3", "value_block5", "block4" }));
}
