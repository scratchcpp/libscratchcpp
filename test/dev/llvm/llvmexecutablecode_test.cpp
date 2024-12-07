#include <scratchcpp/target.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/promise.h>
#include <dev/engine/internal/llvm/llvmexecutablecode.h>
#include <dev/engine/internal/llvm/llvmexecutioncontext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/IRBuilder.h>
#include <gmock/gmock.h>

#include "testmock.h"
#include "testfunctions.h"

using namespace libscratchcpp;

class LLVMExecutableCodeTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_module = std::make_unique<llvm::Module>("test", m_ctx);
            m_builder = std::make_unique<llvm::IRBuilder<>>(m_ctx);
            test_function(nullptr, nullptr, nullptr, nullptr, nullptr); // force dependency

            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        inline llvm::Constant *nullPointer() { return llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0)); }

        llvm::Function *beginMainFunction()
        {
            // void *f(ExecutionContext *, Target *, ValueData **, List **)
            llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
            llvm::FunctionType *funcType = llvm::FunctionType::get(pointerType, { pointerType, pointerType, pointerType, pointerType }, false);
            llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f", m_module.get());

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
            m_builder->SetInsertPoint(entry);
            return func;
        }

        llvm::Function *beginResumeFunction()
        {
            // bool f(void *)
            llvm::FunctionType *funcType = llvm::FunctionType::get(m_builder->getInt1Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false);
            llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "resume", m_module.get());

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
            m_builder->SetInsertPoint(entry);
            return func;
        }

        void endFunction(llvm::Value *ret) { m_builder->CreateRet(ret); }

        void addTestFunction(llvm::Function *mainFunc)
        {
            auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
            auto func = m_module->getOrInsertFunction("test_function", llvm::FunctionType::get(m_builder->getVoidTy(), { ptrType, ptrType, ptrType, ptrType, ptrType }, false));

            llvm::Constant *mockInt = llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_ctx), (uintptr_t)&m_mock, false);
            llvm::Constant *mockPtr = llvm::ConstantExpr::getIntToPtr(mockInt, ptrType);

            m_builder->CreateCall(func, { mockPtr, mainFunc->getArg(0), mainFunc->getArg(1), mainFunc->getArg(2), mainFunc->getArg(3) });
        }

        void addTestPrintFunction(llvm::Value *arg1, llvm::Value *arg2)
        {
            auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
            auto func = m_module->getOrInsertFunction("test_print_function", llvm::FunctionType::get(m_builder->getVoidTy(), { ptrType, ptrType }, false));
            m_builder->CreateCall(func, { arg1, arg2 });
        }

        llvm::LLVMContext m_ctx;
        std::unique_ptr<llvm::Module> m_module;
        std::unique_ptr<llvm::IRBuilder<>> m_builder;
        Target m_target;
        TestMock m_mock;
};

TEST_F(LLVMExecutableCodeTest, CreateExecutionContext)
{
    beginMainFunction();
    endFunction(nullPointer());

    beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    LLVMExecutableCode code(std::move(m_module));
    auto ctx = code.createExecutionContext(&m_target);
    ASSERT_TRUE(ctx);
    ASSERT_EQ(ctx->target(), &m_target);
    ASSERT_TRUE(dynamic_cast<LLVMExecutionContext *>(ctx.get()));
}

TEST_F(LLVMExecutableCodeTest, MainFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    auto f = beginMainFunction();
    addTestFunction(f);
    endFunction(nullPointer());

    beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    LLVMExecutableCode code(std::move(m_module));
    auto ctx = code.createExecutionContext(&m_target);
    ASSERT_FALSE(code.isFinished(ctx.get()));

    EXPECT_CALL(m_mock, f(ctx.get(), &m_target, m_target.variableData(), m_target.listData()));
    code.run(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));

    code.kill(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));

    code.reset(ctx.get());
    ASSERT_FALSE(code.isFinished(ctx.get()));

    code.kill(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));

    EXPECT_CALL(m_mock, f).Times(0);
    code.run(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    anotherTarget.addVariable(std::make_shared<Variable>("", ""));
    anotherTarget.addList(std::make_shared<List>("", ""));
    auto anotherCtx = code.createExecutionContext(&anotherTarget);
    ASSERT_FALSE(code.isFinished(anotherCtx.get()));
    ASSERT_TRUE(code.isFinished(ctx.get()));

    EXPECT_CALL(m_mock, f(anotherCtx.get(), &anotherTarget, anotherTarget.variableData(), anotherTarget.listData()));
    code.run(anotherCtx.get());
    ASSERT_TRUE(code.isFinished(anotherCtx.get()));
    ASSERT_TRUE(code.isFinished(ctx.get()));

    code.reset(ctx.get());
    ASSERT_TRUE(code.isFinished(anotherCtx.get()));
    ASSERT_FALSE(code.isFinished(ctx.get()));
}

TEST_F(LLVMExecutableCodeTest, Promise)
{
    auto f = beginMainFunction();
    addTestFunction(f);
    endFunction(nullPointer());

    beginResumeFunction();
    endFunction(m_builder->getInt1(true));

    LLVMExecutableCode code(std::move(m_module));
    auto ctx = code.createExecutionContext(&m_target);
    ASSERT_FALSE(code.isFinished(ctx.get()));

    // run()
    auto promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, f).Times(0);

    for (int i = 0; i < 10; i++) {
        code.run(ctx.get());
        ASSERT_FALSE(code.isFinished(ctx.get()));
    }

    promise->resolve();

    EXPECT_CALL(m_mock, f);
    code.run(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    code.reset(ctx.get());

    // kill()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, f).Times(0);

    for (int i = 0; i < 10; i++) {
        code.run(ctx.get());
        ASSERT_FALSE(code.isFinished(ctx.get()));
    }

    code.kill(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    code.reset(ctx.get());

    // reset()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(m_mock, f).Times(0);

    for (int i = 0; i < 10; i++) {
        code.run(ctx.get());
        ASSERT_FALSE(code.isFinished(ctx.get()));
    }

    code.reset(ctx.get());
    ASSERT_FALSE(code.isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);

    EXPECT_CALL(m_mock, f);
    code.run(ctx.get());
    ASSERT_TRUE(code.isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    auto anotherCtx = code.createExecutionContext(&anotherTarget);
    ASSERT_FALSE(code.isFinished(anotherCtx.get()));
    ASSERT_TRUE(code.isFinished(ctx.get()));

    promise = std::make_shared<Promise>();
    anotherCtx->setPromise(promise);
    EXPECT_CALL(m_mock, f).Times(0);

    for (int i = 0; i < 10; i++) {
        code.run(anotherCtx.get());
        ASSERT_FALSE(code.isFinished(anotherCtx.get()));
    }

    promise->resolve();

    EXPECT_CALL(m_mock, f);
    code.run(anotherCtx.get());
    ASSERT_TRUE(code.isFinished(anotherCtx.get()));
    ASSERT_TRUE(code.isFinished(ctx.get()));

    code.reset(ctx.get());
    ASSERT_TRUE(code.isFinished(anotherCtx.get()));
    ASSERT_FALSE(code.isFinished(ctx.get()));
}
