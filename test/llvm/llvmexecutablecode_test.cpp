#include <scratchcpp/target.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <engine/internal/llvm/llvmexecutablecode.h>
#include <engine/internal/llvm/llvmcompilercontext.h>
#include <engine/internal/llvm/llvmexecutioncontext.h>
#include <engine/internal/llvm/llvmtypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <enginemock.h>

#include "testmock.h"
#include "testfunctions.h"

using namespace libscratchcpp;

using ::testing::Return;

class LLVMExecutableCodeTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_target.setName("test");
            m_ctx = std::make_unique<LLVMCompilerContext>(&m_engine, &m_target);
            m_module = m_ctx->module();
            m_llvmCtx = m_ctx->llvmCtx();
            m_builder = std::make_unique<llvm::IRBuilder<>>(*m_llvmCtx);
            m_valueDataType = m_ctx->valueDataType();
            test_function(nullptr, nullptr, nullptr, nullptr, nullptr); // force dependency

            m_script = std::make_unique<Script>(&m_target, nullptr, &m_engine);
        }

        inline llvm::Constant *nullPointer() { return llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*m_llvmCtx), 0)); }

        llvm::Function *beginMainFunction(Compiler::CodeType codeType = Compiler::CodeType::Script)
        {
            // void *f(ExecutionContext *, Target *, ValueData **, List **)
            // ValueData f(...) (reporters)
            // bool f(...) (hat predicates)
            llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_llvmCtx), 0);
            llvm::Type *retType = nullptr;

            switch (codeType) {
                case Compiler::CodeType::Script:
                    retType = pointerType;
                    break;

                case Compiler::CodeType::Reporter:
                    retType = m_valueDataType;
                    break;

                case Compiler::CodeType::HatPredicate:
                    retType = m_builder->getInt1Ty();
                    break;
            }

            llvm::FunctionType *funcType = llvm::FunctionType::get(retType, { pointerType, pointerType, pointerType, pointerType }, false);
            llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f", m_module);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*m_llvmCtx, "entry", func);
            m_builder->SetInsertPoint(entry);
            return func;
        }

        llvm::Function *beginResumeFunction()
        {
            // bool f(void *)
            llvm::FunctionType *funcType = llvm::FunctionType::get(m_builder->getInt1Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(*m_llvmCtx), 0), false);
            llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "resume", m_module);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*m_llvmCtx, "entry", func);
            m_builder->SetInsertPoint(entry);
            return func;
        }

        void endFunction(llvm::Value *ret) { m_builder->CreateRet(ret); }

        llvm::Value *addMainFunction(llvm::Function *mainFunc, const std::string testFuncName, llvm::Type *retType)
        {
            auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_llvmCtx), 0);
            auto func = m_module->getOrInsertFunction(testFuncName, llvm::FunctionType::get(retType, { ptrType, ptrType, ptrType, ptrType, ptrType }, false));

            llvm::Constant *mockInt = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*m_llvmCtx), (uintptr_t)&m_mock, false);
            llvm::Constant *mockPtr = llvm::ConstantExpr::getIntToPtr(mockInt, ptrType);

            return m_builder->CreateCall(func, { mockPtr, mainFunc->getArg(0), mainFunc->getArg(1), mainFunc->getArg(2), mainFunc->getArg(3) });
        }

        void addTestFunction(llvm::Function *mainFunc) { addMainFunction(mainFunc, "test_function", m_builder->getVoidTy()); }

        llvm::Value *addReporterFunction(llvm::Function *mainFunc) { return addMainFunction(mainFunc, "test_reporter", m_valueDataType); }

        llvm::Value *addPredicateFunction(llvm::Function *mainFunc) { return addMainFunction(mainFunc, "test_predicate", m_builder->getInt1Ty()); }

        void addTestPrintFunction(llvm::Value *arg1, llvm::Value *arg2)
        {
            auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_llvmCtx), 0);
            auto func = m_module->getOrInsertFunction("test_print_function", llvm::FunctionType::get(m_builder->getVoidTy(), { ptrType, ptrType }, false));
            m_builder->CreateCall(func, { arg1, arg2 });
        }

        std::unique_ptr<LLVMCompilerContext> m_ctx;
        llvm::LLVMContext *m_llvmCtx = nullptr;
        llvm::Module *m_module = nullptr;
        std::unique_ptr<llvm::IRBuilder<>> m_builder;
        llvm::StructType *m_valueDataType = nullptr;
        Target m_target;
        EngineMock m_engine;
        std::unique_ptr<Script> m_script;
        TestMock m_mock;
};

TEST_F(LLVMExecutableCodeTest, CreateExecutionContext)
{
    llvm::Function *mainFunc = beginMainFunction();
    endFunction(nullPointer());

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    {
        auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::Script);
        m_script->setCode(code);
        Thread thread(&m_target, &m_engine, m_script.get());
        auto ctx = code->createExecutionContext(&thread);
        ASSERT_TRUE(ctx);
        ASSERT_EQ(ctx->thread(), &thread);
        ASSERT_TRUE(dynamic_cast<LLVMExecutionContext *>(ctx.get()));
    }
}

TEST_F(LLVMExecutableCodeTest, CreatePredicateExecutionContext)
{
    llvm::Function *mainFunc = beginMainFunction(Compiler::CodeType::HatPredicate);
    endFunction(m_builder->getInt1(false));

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    {
        auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::HatPredicate);
        m_script->setCode(code);
        Thread thread(&m_target, &m_engine, m_script.get());
        auto ctx = code->createExecutionContext(&thread);
        ASSERT_TRUE(ctx);
        ASSERT_EQ(ctx->thread(), &thread);
        ASSERT_TRUE(dynamic_cast<LLVMExecutionContext *>(ctx.get()));
    }
}

TEST_F(LLVMExecutableCodeTest, CreateReporterExecutionContext)
{
    llvm::Function *mainFunc = beginMainFunction(Compiler::CodeType::Reporter);
    endFunction(addReporterFunction(mainFunc));

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    {
        auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::Reporter);
        m_script->setCode(code);
        Thread thread(&m_target, &m_engine, m_script.get());
        auto ctx = code->createExecutionContext(&thread);
        ASSERT_TRUE(ctx);
        ASSERT_EQ(ctx->thread(), &thread);
        ASSERT_TRUE(dynamic_cast<LLVMExecutionContext *>(ctx.get()));
    }
}

TEST_F(LLVMExecutableCodeTest, MainFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    llvm::Function *mainFunc = beginMainFunction();
    addTestFunction(mainFunc);
    endFunction(nullPointer());

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::Script);
    m_script->setCode(code);
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = code->createExecutionContext(&thread);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    EXPECT_CALL(m_mock, script(ctx.get(), &m_target, m_target.variableData(), m_target.listData()));
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    code->reset(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));

    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    EXPECT_CALL(m_mock, script).Times(0);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    anotherTarget.addVariable(std::make_shared<Variable>("", ""));
    anotherTarget.addList(std::make_shared<List>("", ""));
    Script script(&anotherTarget, nullptr, &m_engine);
    script.setCode(code);
    Thread anotherThread(&anotherTarget, &m_engine, &script);
    auto anotherCtx = code->createExecutionContext(&anotherThread);
    ASSERT_FALSE(code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(code->isFinished(ctx.get()));

    EXPECT_CALL(m_mock, script(anotherCtx.get(), &anotherTarget, anotherTarget.variableData(), anotherTarget.listData()));
    code->run(anotherCtx.get());
    ASSERT_TRUE(code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(code->isFinished(ctx.get()));

    code->reset(ctx.get());
    ASSERT_TRUE(code->isFinished(anotherCtx.get()));
    ASSERT_FALSE(code->isFinished(ctx.get()));
}

TEST_F(LLVMExecutableCodeTest, PredicateFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    llvm::Function *mainFunc = beginMainFunction(Compiler::CodeType::HatPredicate);
    endFunction(addPredicateFunction(mainFunc));

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::HatPredicate);
    m_script->setCode(code);
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = code->createExecutionContext(&thread);

    EXPECT_CALL(m_mock, predicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(true));
    ASSERT_TRUE(code->runPredicate(ctx.get()));

    EXPECT_CALL(m_mock, predicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(true));
    ASSERT_TRUE(code->runPredicate(ctx.get()));

    EXPECT_CALL(m_mock, predicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(false));
    ASSERT_FALSE(code->runPredicate(ctx.get()));

    EXPECT_CALL(m_mock, predicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(false));
    ASSERT_FALSE(code->runPredicate(ctx.get()));
}

TEST_F(LLVMExecutableCodeTest, ReporterFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    llvm::Function *mainFunc = beginMainFunction(Compiler::CodeType::Reporter);
    endFunction(addReporterFunction(mainFunc));

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::Reporter);
    m_script->setCode(code);
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = code->createExecutionContext(&thread);

    Value v = "test";
    EXPECT_CALL(m_mock, reporter(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(v.data()));
    ASSERT_EQ(Value(code->runReporter(ctx.get())).toString(), v.toString());
}

TEST_F(LLVMExecutableCodeTest, Promise)
{
    llvm::Function *mainFunc = beginMainFunction();
    addTestFunction(mainFunc);
    endFunction(nullPointer());

    llvm::Function *resumeFunc = beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    auto code = std::make_shared<LLVMExecutableCode>(m_ctx.get(), mainFunc->getName().str(), resumeFunc->getName().str(), Compiler::CodeType::Script);
    m_script->setCode(code);
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = code->createExecutionContext(&thread);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    // run()
    auto promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, script).Times(0);

    for (int i = 0; i < 10; i++) {
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }

    promise->resolve();

    EXPECT_CALL(m_mock, script);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    code->reset(ctx.get());

    // kill()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, script).Times(0);

    for (int i = 0; i < 10; i++) {
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }

    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    code->reset(ctx.get());

    // reset()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, script).Times(0);

    for (int i = 0; i < 10; i++) {
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }

    code->reset(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);

    EXPECT_CALL(m_mock, script);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    Script script(&anotherTarget, nullptr, &m_engine);
    script.setCode(code);
    Thread anotherThread(&anotherTarget, &m_engine, &script);
    auto anotherCtx = code->createExecutionContext(&anotherThread);
    ASSERT_FALSE(code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(code->isFinished(ctx.get()));

    promise = std::make_shared<Promise>();
    anotherCtx->setPromise(promise);
    EXPECT_CALL(m_mock, script).Times(0);

    for (int i = 0; i < 10; i++) {
        code->run(anotherCtx.get());
        ASSERT_FALSE(code->isFinished(anotherCtx.get()));
    }

    promise->resolve();

    EXPECT_CALL(m_mock, script);
    code->run(anotherCtx.get());
    ASSERT_TRUE(code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(code->isFinished(ctx.get()));

    code->reset(ctx.get());
    ASSERT_TRUE(code->isFinished(anotherCtx.get()));
    ASSERT_FALSE(code->isFinished(ctx.get()));
}
