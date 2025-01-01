#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilercontext.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/dev/compilerlocalvariable.h>
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
            m_ctx = std::make_shared<CompilerContext>(&m_engine, &m_target);
            CompilerPrivate::builderFactory = &m_builderFactory;

            EXPECT_CALL(m_builderFactory, createCtx(&m_engine, &m_target)).WillOnce(Return(m_ctx));
            m_compiler = std::make_unique<Compiler>(&m_engine, &m_target);

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

        void compile(Compiler *compiler, std::shared_ptr<Block> block, BlockPrototype *procedurePrototype = nullptr)
        {
            ASSERT_EQ(compiler->block(), nullptr);
            EXPECT_CALL(m_builderFactory, create(m_ctx.get(), procedurePrototype)).WillOnce(Return(m_builder));
            EXPECT_CALL(*m_builder, finalize()).WillOnce(Return(m_code));
            ASSERT_EQ(compiler->compile(block), m_code);
            ASSERT_EQ(compiler->block(), nullptr);
        }

        EngineMock m_engine;
        TargetMock m_target;
        std::unique_ptr<Compiler> m_compiler;
        std::shared_ptr<CompilerContext> m_ctx;
        CodeBuilderFactoryMock m_builderFactory;
        static inline std::shared_ptr<CodeBuilderMock> m_builder;
        std::shared_ptr<ExecutableCodeMock> m_code;
        static inline std::shared_ptr<Block> m_compareBlock;
        static inline std::shared_ptr<Variable> m_testVar;
};

TEST_F(CompilerTest, Engine)
{
    ASSERT_EQ(m_compiler->engine(), &m_engine);
}

TEST_F(CompilerTest, Target)
{
    ASSERT_EQ(m_compiler->target(), &m_target);
}

TEST_F(CompilerTest, AddFunctionCall)
{

    auto block = std::make_shared<Block>("a", "");
    m_compareBlock = block;
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->block(), m_compareBlock);
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        Compiler::ArgTypes argTypes = { Compiler::StaticType::Number, Compiler::StaticType::Bool };
        Compiler::Args args = { &arg1, &arg2 };
        EXPECT_CALL(*m_builder, addFunctionCall("test1", Compiler::StaticType::Void, argTypes, args));
        compiler->addFunctionCall("test1", Compiler::StaticType::Void, argTypes, args);

        args = { &arg1 };
        argTypes = { Compiler::StaticType::String };
        EXPECT_CALL(*m_builder, addFunctionCall("test2", Compiler::StaticType::Bool, argTypes, args));
        compiler->addFunctionCall("test2", Compiler::StaticType::Bool, argTypes, args);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddTargetFunctionCall)
{

    auto block = std::make_shared<Block>("a", "");
    m_compareBlock = block;
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->block(), m_compareBlock);
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        Compiler::ArgTypes argTypes = { Compiler::StaticType::Number, Compiler::StaticType::Bool };
        Compiler::Args args = { &arg1, &arg2 };
        EXPECT_CALL(*m_builder, addTargetFunctionCall("test1", Compiler::StaticType::Void, argTypes, args));
        compiler->addTargetFunctionCall("test1", Compiler::StaticType::Void, argTypes, args);

        args = { &arg1 };
        argTypes = { Compiler::StaticType::String };
        EXPECT_CALL(*m_builder, addTargetFunctionCall("test2", Compiler::StaticType::Bool, argTypes, args));
        compiler->addTargetFunctionCall("test2", Compiler::StaticType::Bool, argTypes, args);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddFunctionCallWithCtx)
{

    auto block = std::make_shared<Block>("a", "");
    m_compareBlock = block;
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->block(), m_compareBlock);
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        Compiler::ArgTypes argTypes = { Compiler::StaticType::Number, Compiler::StaticType::Bool };
        Compiler::Args args = { &arg1, &arg2 };
        EXPECT_CALL(*m_builder, addFunctionCallWithCtx("test1", Compiler::StaticType::Void, argTypes, args));
        compiler->addFunctionCallWithCtx("test1", Compiler::StaticType::Void, argTypes, args);

        args = { &arg1 };
        argTypes = { Compiler::StaticType::String };
        EXPECT_CALL(*m_builder, addFunctionCallWithCtx("test2", Compiler::StaticType::Bool, argTypes, args));
        compiler->addFunctionCallWithCtx("test2", Compiler::StaticType::Bool, argTypes, args);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddConstValue)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerConstant ret(Compiler::StaticType::Unknown, Value());

        EXPECT_CALL(*m_builder, addConstValue(Value(1))).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addConstValue(1), &ret);

        EXPECT_CALL(*m_builder, addConstValue(Value("test"))).WillOnce(Return(&ret));
        compiler->addConstValue("test");

        EXPECT_CALL(*m_builder, addConstValue(Value(3.5))).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addConstValue(3.5), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddLoopIndex)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);

        EXPECT_CALL(*m_builder, addLoopIndex()).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addLoopIndex(), &ret);

        EXPECT_CALL(*m_builder, addLoopIndex()).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addLoopIndex(), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddLocalVariableValue)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Number);
        CompilerValue ptr1(Compiler::StaticType::Number);
        CompilerValue ptr2(Compiler::StaticType::Bool);
        CompilerLocalVariable var1(&ptr1);
        CompilerLocalVariable var2(&ptr2);

        EXPECT_CALL(*m_builder, addLocalVariableValue(&var1)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addLocalVariableValue(&var1), &ret);

        EXPECT_CALL(*m_builder, addLocalVariableValue(&var2)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addLocalVariableValue(&var2), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddVariableValue)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        Variable var1("", ""), var2("", "");

        EXPECT_CALL(*m_builder, addVariableValue(&var1)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addVariableValue(&var1), &ret);

        EXPECT_CALL(*m_builder, addVariableValue(&var2)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addVariableValue(&var2), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddListContents)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        List list1("", ""), list2("", "");

        EXPECT_CALL(*m_builder, addListContents(&list1)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addListContents(&list1), &ret);

        EXPECT_CALL(*m_builder, addListContents(&list2)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addListContents(&list2), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddListItem)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        CompilerValue arg(Compiler::StaticType::Unknown);
        List list1("", ""), list2("", "");

        EXPECT_CALL(*m_builder, addListItem(&list1, &arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addListItem(&list1, &arg), &ret);

        EXPECT_CALL(*m_builder, addListItem(&list2, &arg)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addListItem(&list2, &arg), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddListItemIndex)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        CompilerValue arg(Compiler::StaticType::Unknown);
        List list1("", ""), list2("", "");

        EXPECT_CALL(*m_builder, addListItemIndex(&list1, &arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addListItemIndex(&list1, &arg), &ret);

        EXPECT_CALL(*m_builder, addListItemIndex(&list2, &arg)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addListItemIndex(&list2, &arg), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddListSize)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        List list1("", ""), list2("", "");

        EXPECT_CALL(*m_builder, addListSize(&list1)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addListSize(&list1), &ret);

        EXPECT_CALL(*m_builder, addListSize(&list2)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addListSize(&list2), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddListContains)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);
        CompilerValue arg(Compiler::StaticType::Unknown);
        List list1("", ""), list2("", "");

        EXPECT_CALL(*m_builder, addListContains(&list1, &arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addListContains(&list1, &arg), &ret);

        EXPECT_CALL(*m_builder, addListContains(&list2, &arg)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addListContains(&list2, &arg), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddProcedureArgument)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ret(Compiler::StaticType::Unknown);

        EXPECT_CALL(*m_builder, addProcedureArgument("arg 1")).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addProcedureArgument("arg 1"), &ret);

        EXPECT_CALL(*m_builder, addProcedureArgument("arg 2")).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->addProcedureArgument("arg 2"), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, AddInput)
{

    auto block = std::make_shared<Block>("a", "");
    auto valueBlock = std::make_shared<Block>("b", "");
    m_compareBlock = valueBlock;
    valueBlock->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->block(), m_compareBlock);
        return compiler->addConstValue("value block");
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

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerConstant constRet(Compiler::StaticType::Unknown, Value());
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, addConstValue(Value())).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("INVALID"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("test"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("SHADOW"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("value block"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("SHADOW_BLOCK"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("SHADOW_MENU"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("test"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("NO_SHADOW"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("value block"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("NO_SHADOW_BLOCK"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("selected item"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("NO_SHADOW_MENU"), &constRet);

        EXPECT_CALL(*m_builder, addConstValue(Value("test"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput("OBSCURED_SHADOW"), &constRet);

        EXPECT_CALL(*m_builder, addVariableValue(m_testVar.get())).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->addInput(compiler->input("OBSCURED_SHADOW_VARIABLE")), &ret);

        EXPECT_CALL(*m_builder, addConstValue(Value("value block"))).WillOnce(Return(&constRet));
        EXPECT_EQ(compiler->addInput(compiler->input("OBSCURED_SHADOW_BLOCK")), &constRet);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAdd)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAdd(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAdd(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateSub)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createSub(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createSub(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateMul)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createMul(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createMul(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateDiv)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createDiv(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createDiv(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateRandom)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createRandom(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createRandom(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateRandomInt)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createRandomInt(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createRandomInt(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateCmpEQ)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createCmpEQ(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createCmpEQ(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateCmpGT)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createCmpGT(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createCmpGT(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateCmpLT)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createCmpLT(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createCmpLT(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAnd)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAnd(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAnd(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateOr)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createOr(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createOr(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateNot)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createNot(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createNot(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateMod)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createMod(&arg1, &arg2)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createMod(&arg1, &arg2), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateRound)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createRound(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createRound(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAbs)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAbs(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAbs(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateFloor)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createFloor(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createFloor(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateCeil)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createCeil(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createCeil(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateSqrt)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createSqrt(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createSqrt(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateSin)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createSin(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createSin(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateCos)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createCos(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createCos(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateTan)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createTan(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createTan(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAsin)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAsin(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAsin(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAcos)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAcos(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAcos(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateAtan)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createAtan(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createAtan(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateLn)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createLn(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createLn(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateLog10)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createLog10(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createLog10(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateExp)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createExp(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createExp(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateExp10)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createExp10(&arg)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createExp10(&arg), &ret);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateSelect)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        CompilerValue arg3(Compiler::StaticType::Unknown);
        CompilerValue ret(Compiler::StaticType::Unknown);

        EXPECT_CALL(*m_builder, createSelect(&arg1, &arg2, &arg3, Compiler::StaticType::Number)).WillOnce(Return(&ret));
        EXPECT_EQ(compiler->createSelect(&arg1, &arg2, &arg3, Compiler::StaticType::Number), &ret);

        EXPECT_CALL(*m_builder, createSelect(&arg1, &arg2, &arg3, Compiler::StaticType::Unknown)).WillOnce(Return(nullptr));
        EXPECT_EQ(compiler->createSelect(&arg1, &arg2, &arg3, Compiler::StaticType::Unknown), nullptr);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateLocalVariable)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ptr1(Compiler::StaticType::Number);
        CompilerLocalVariable var1(&ptr1);
        EXPECT_CALL(*m_builder, createLocalVariable(var1.type())).WillOnce(Return(&var1));
        EXPECT_EQ(compiler->createLocalVariable(var1.type()), &var1);

        CompilerValue ptr2(Compiler::StaticType::Number);
        CompilerLocalVariable var2(&ptr2);
        EXPECT_CALL(*m_builder, createLocalVariable(var2.type())).WillOnce(Return(&var2));
        EXPECT_EQ(compiler->createLocalVariable(var2.type()), &var2);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateLocalVariableWrite)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue ptr(Compiler::StaticType::Number);
        CompilerLocalVariable var(&ptr);
        CompilerValue arg(Compiler::StaticType::Number);
        EXPECT_CALL(*m_builder, createLocalVariableWrite(&var, &arg));
        compiler->createLocalVariableWrite(&var, &arg);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateVariableWrite)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        auto var = std::make_shared<Variable>("", "");
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, createVariableWrite(var.get(), &arg));
        compiler->createVariableWrite(var.get(), &arg);

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CustomIfStatement)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        compiler->beginIfStatement(&arg);
        EXPECT_CALL(*m_builder, endIf());
        compiler->endIf();

        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        compiler->beginIfStatement(&arg);
        EXPECT_CALL(*m_builder, beginElseBranch());
        compiler->beginElseBranch();
        EXPECT_CALL(*m_builder, endIf());
        compiler->endIf();

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CustomWhileLoop)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginWhileLoop(&arg));
        compiler->beginWhileLoop(&arg);
        EXPECT_CALL(*m_builder, endLoop());
        compiler->endLoop();

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CustomRepeatUntilLoop)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop(&arg));
        compiler->beginRepeatUntilLoop(&arg);
        EXPECT_CALL(*m_builder, endLoop());
        compiler->endLoop();

        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, BeginLoopCondition)
{

    auto block = std::make_shared<Block>("a", "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_CALL(*m_builder, beginLoopCondition());
        compiler->beginLoopCondition();
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, MoveToIf)
{

    EXPECT_CALL(*m_builder, beginElseBranch).Times(0);

    auto if1 = std::make_shared<Block>("", "if");
    if1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement).Times(0);
        EXPECT_CALL(*m_builder, endIf).Times(0);
        compiler->moveToIf(&arg, nullptr);
        return nullptr;
    });

    auto if2 = std::make_shared<Block>("", "if");
    if1->setNext(if2);
    if2->setParent(if1);
    if2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value()));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIf(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    auto substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if2);
    substack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(Value()); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    if2->addInput(input);

    compile(m_compiler.get(), if1);
}

TEST_F(CompilerTest, MoveToIfElse)
{

    auto if1 = std::make_shared<Block>("", "if");
    if1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement).Times(0);
        EXPECT_CALL(*m_builder, beginElseBranch).Times(0);
        EXPECT_CALL(*m_builder, endIf).Times(0);
        compiler->moveToIfElse(&arg, nullptr, nullptr);
        return nullptr;
    });

    auto if2 = std::make_shared<Block>("", "if");
    if1->setNext(if2);
    if2->setParent(if1);
    if2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, beginElseBranch());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(&arg, compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
        return nullptr;
    });

    auto substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if2);
    substack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

    auto substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(if2);
    substack2->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

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
    if3->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement).Times(3);
        EXPECT_CALL(*m_builder, beginElseBranch()).Times(3);
        EXPECT_CALL(*m_builder, endIf()).Times(3);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, addConstValue(Value(3)));
        EXPECT_CALL(*m_builder, addConstValue(Value(4)));
        compiler->moveToIfElse(&arg, compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
        return nullptr;
    });

    // if
    auto ifSubstack1 = std::make_shared<Block>("", "if");
    ifSubstack1->setParent(if3);
    ifSubstack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        compiler->moveToIfElse(&arg, compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
        return nullptr;
    });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(ifSubstack1);
    substack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(ifSubstack1);
    substack2->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    ifSubstack1->addInput(input);
    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    ifSubstack1->addInput(input);

    // else
    auto ifSubstack2 = std::make_shared<Block>("", "if");
    ifSubstack2->setParent(if3);
    ifSubstack2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        compiler->moveToIfElse(&arg, compiler->input("SUBSTACK")->valueBlock(), compiler->input("SUBSTACK2")->valueBlock());
        return nullptr;
    });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(ifSubstack2);
    substack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(3); });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(ifSubstack2);
    substack2->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(4); });

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
    if4->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        EXPECT_CALL(*m_builder, beginElseBranch());
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(&arg, nullptr, compiler->input("SUBSTACK2")->valueBlock());
        return nullptr;
    });

    substack2 = std::make_shared<Block>("", "substack");
    substack2->setParent(if4);
    substack2->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

    input = std::make_shared<Input>("SUBSTACK2", Input::Type::NoShadow);
    input->setValueBlock(substack2);
    if4->addInput(input);

    // Empty 'else' branch
    auto if5 = std::make_shared<Block>("", "if");
    if4->setNext(if5);
    if5->setParent(if4);
    if5->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginIfStatement(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        EXPECT_CALL(*m_builder, beginElseBranch()).Times(0);
        EXPECT_CALL(*m_builder, endIf());
        compiler->moveToIfElse(&arg, compiler->input("SUBSTACK")->valueBlock(), nullptr);
        return nullptr;
    });

    substack1 = std::make_shared<Block>("", "substack");
    substack1->setParent(if5);
    substack1->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

    input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack1);
    if5->addInput(input);

    // Code after the if statement
    auto block = std::make_shared<Block>("", "");
    block->setParent(if5);
    if5->setNext(block);
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(m_compiler.get(), if1);
}

TEST_F(CompilerTest, MoveToRepeatLoop)
{

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(&arg, nullptr);
        return nullptr;
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatLoop(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatLoop).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToRepeatLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        compiler->moveToRepeatLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

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
    l4->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatLoop(&arg, nullptr);
        return nullptr;
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(m_compiler.get(), l1);
}

TEST_F(CompilerTest, MoveToWhileLoop)
{

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginWhileLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(&arg, nullptr);
        return nullptr;
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginWhileLoop(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginWhileLoop).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToWhileLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        compiler->moveToWhileLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

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
    l4->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginWhileLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToWhileLoop(&arg, nullptr);
        return nullptr;
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(m_compiler.get(), l1);
}

TEST_F(CompilerTest, MoveToRepeatUntilLoop)
{

    auto l1 = std::make_shared<Block>("", "loop");
    l1->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(&arg, nullptr);
        return nullptr;
    });

    auto l2 = std::make_shared<Block>("", "loop");
    l1->setNext(l2);
    l2->setParent(l1);
    l2->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop(&arg));
        EXPECT_CALL(*m_builder, addConstValue(Value(2)));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    auto substack = std::make_shared<Block>("", "substack");
    substack->setParent(l2);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(2); });

    auto input = std::make_shared<Input>("SUBSTACK", Input::Type::NoShadow);
    input->setValueBlock(substack);
    l2->addInput(input);

    // Nested
    auto l3 = std::make_shared<Block>("", "loop");
    l2->setNext(l3);
    l3->setParent(l2);
    l3->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop).Times(2);
        EXPECT_CALL(*m_builder, endLoop()).Times(2);
        EXPECT_CALL(*m_builder, addConstValue(Value(1)));
        compiler->moveToRepeatUntilLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    // Begin loop
    auto loopSubstack = std::make_shared<Block>("", "loop");
    loopSubstack->setParent(l3);
    loopSubstack->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        compiler->moveToRepeatUntilLoop(&arg, compiler->input("SUBSTACK")->valueBlock());
        return nullptr;
    });

    substack = std::make_shared<Block>("", "substack");
    substack->setParent(loopSubstack);
    substack->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(1); });

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
    l4->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        CompilerValue arg(Compiler::StaticType::Unknown);
        EXPECT_CALL(*m_builder, beginRepeatUntilLoop(&arg));
        EXPECT_CALL(*m_builder, endLoop());
        compiler->moveToRepeatUntilLoop(&arg, nullptr);
        return nullptr;
    });

    // Code after the loop
    auto block = std::make_shared<Block>("", "");
    block->setParent(l4);
    l4->setNext(block);
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue("after"); });

    EXPECT_CALL(*m_builder, addConstValue(Value("after")));
    compile(m_compiler.get(), l1);
}

TEST_F(CompilerTest, CreateYield)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_CALL(*m_builder, yield());
        compiler->createYield();
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateStop)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_CALL(*m_builder, createStop());
        compiler->createStop();
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, CreateProcedureCall)
{

    auto block = std::make_shared<Block>("", "");

    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        BlockPrototype prototype;
        CompilerValue arg1(Compiler::StaticType::Unknown);
        CompilerValue arg2(Compiler::StaticType::Unknown);
        Compiler::Args args = { &arg1, &arg2 };

        EXPECT_CALL(*m_builder, createProcedureCall(&prototype, args));
        compiler->createProcedureCall(&prototype, args);
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, Input)
{

    auto block = std::make_shared<Block>("a", "");
    block->addInput(std::make_shared<Input>("TEST", Input::Type::Shadow));
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->input("INVALID"), nullptr);
        EXPECT_EQ(compiler->input("TEST"), compiler->block()->inputAt(0).get());
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, Field)
{

    auto block = std::make_shared<Block>("a", "");
    block->addField(std::make_shared<Field>("TEST", "test"));
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        EXPECT_EQ(compiler->field("INVALID"), nullptr);
        EXPECT_EQ(compiler->field("TEST"), compiler->block()->fieldAt(0).get());
        return nullptr;
    });

    compile(m_compiler.get(), block);
}

TEST_F(CompilerTest, UnsupportedBlocks)
{
    auto valueBlock1 = std::make_shared<Block>("v1", "value_block1");
    auto valueBlock2 = std::make_shared<Block>("v2", "value_block2");
    valueBlock2->setCompileFunction([](Compiler *) -> CompilerValue * { return nullptr; });

    auto valueBlock3 = std::make_shared<Block>("v3", "value_block3");
    auto valueBlock4 = std::make_shared<Block>("v4", "value_block4");
    valueBlock4->setCompileFunction([](Compiler *) -> CompilerValue * { return nullptr; });

    auto valueBlock5 = std::make_shared<Block>("v5", "value_block5");
    auto valueBlock6 = std::make_shared<Block>("v6", "value_block6");
    valueBlock6->setCompileFunction([](Compiler *) -> CompilerValue * { return nullptr; });

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
    block3->setCompileFunction([](Compiler *compiler) -> CompilerValue * {
        compiler->addInput("INPUT1");
        compiler->addInput("INPUT2");
        compiler->addInput("INPUT3");
        compiler->addInput("INPUT4");
        compiler->addInput("INPUT5");
        compiler->addInput("INPUT6");
        return nullptr;
    });
    block3->setParent(block2);
    block2->setNext(block3);

    auto block4 = std::make_shared<Block>("b4", "block4");
    block4->setParent(block3);
    block3->setNext(block4);

    EXPECT_CALL(*m_builder, addConstValue).WillRepeatedly(Return(nullptr));
    compile(m_compiler.get(), block1);

    ASSERT_EQ(m_compiler->unsupportedBlocks(), std::unordered_set<std::string>({ "block1", "block2", "value_block1", "value_block3", "value_block5", "block4" }));
}

TEST_F(CompilerTest, Procedure)
{
    {
        auto block = std::make_shared<Block>("", "");
        auto customBlock = std::make_shared<Block>("", "");
        customBlock->mutationPrototype()->setProcCode("");

        auto input = std::make_shared<Input>("custom_block", Input::Type::ObscuredShadow);
        input->setValueBlock(customBlock);
        block->addInput(input);

        compile(m_compiler.get(), block, nullptr);
    }

    {
        auto block = std::make_shared<Block>("", "");
        auto customBlock = std::make_shared<Block>("", "");
        customBlock->mutationPrototype()->setProcCode("test");

        auto input = std::make_shared<Input>("custom_block", Input::Type::ObscuredShadow);
        input->setValueBlock(customBlock);
        block->addInput(input);

        compile(m_compiler.get(), block, customBlock->mutationPrototype());
    }
}
