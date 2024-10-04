#include <scratchcpp/target.h>
#include <scratchcpp/value_functions.h>
#include <dev/engine/internal/llvmexecutablecode.h>
#include <dev/engine/internal/llvmexecutioncontext.h>
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
            test_function(nullptr, nullptr); // force dependency

            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        llvm::Function *beginFunction(size_t index)
        {
            // size_t f#(Target *)
            llvm::FunctionType *funcType = llvm::FunctionType::get(m_builder->getInt64Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false);
            llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f" + std::to_string(index), m_module.get());

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
            m_builder->SetInsertPoint(entry);
            return func;
        }

        void endFunction(size_t index)
        {
            // Return next function index
            m_builder->CreateRet(m_builder->getInt64(index + 1));
        }

        void addTestFunction(llvm::Function *mainFunc)
        {
            auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
            auto func = m_module->getOrInsertFunction("test_function", llvm::FunctionType::get(m_builder->getVoidTy(), { ptrType, ptrType }, false));

            llvm::Constant *mockInt = llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_ctx), (uintptr_t)&m_mock, false);
            llvm::Constant *mockPtr = llvm::ConstantExpr::getIntToPtr(mockInt, ptrType);

            m_builder->CreateCall(func, { mockPtr, mainFunc->getArg(0) });
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

TEST_F(LLVMExecutableCodeTest, NoFunctions)
{
    LLVMExecutionContext ctx(&m_target);
    std::vector<std::unique_ptr<ValueData>> constValues;
    LLVMExecutableCode code(std::move(m_module), constValues);
    ASSERT_TRUE(code.isFinished(&ctx));

    code.run(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));

    code.kill(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));

    code.reset(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));
}

TEST_F(LLVMExecutableCodeTest, SingleFunction)
{
    auto f = beginFunction(0);
    addTestFunction(f);
    endFunction(0);

    LLVMExecutionContext ctx(&m_target);
    std::vector<std::unique_ptr<ValueData>> constValues;
    LLVMExecutableCode code(std::move(m_module), constValues);
    ASSERT_FALSE(code.isFinished(&ctx));

    EXPECT_CALL(m_mock, f(&m_target));
    code.run(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));

    code.kill(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));

    code.reset(&ctx);
    ASSERT_FALSE(code.isFinished(&ctx));

    code.kill(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));

    EXPECT_CALL(m_mock, f).Times(0);
    code.run(&ctx);
    ASSERT_TRUE(code.isFinished(&ctx));
}

TEST_F(LLVMExecutableCodeTest, MultipleFunctions)
{
    static const int count = 5;

    for (int i = 0; i < count; i++) {
        auto f = beginFunction(i);
        addTestFunction(f);
        endFunction(i);
    }

    LLVMExecutionContext ctx(&m_target);
    std::vector<std::unique_ptr<ValueData>> constValues;
    LLVMExecutableCode code(std::move(m_module), constValues);
    ASSERT_FALSE(code.isFinished(&ctx));

    for (int i = 0; i < count; i++) {
        ASSERT_FALSE(code.isFinished(&ctx));
        EXPECT_CALL(m_mock, f(&m_target));
        code.run(&ctx);
    }

    ASSERT_TRUE(code.isFinished(&ctx));
}

TEST_F(LLVMExecutableCodeTest, ConstValues)
{
    beginFunction(0);
    std::vector<std::unique_ptr<ValueData>> constValues;

    for (int i = 0; i < 2; i++) {
        std::unique_ptr<ValueData> value = std::make_unique<ValueData>();
        value_init(value.get());
        value_assign_int(value.get(), i + 1);
        constValues.push_back(std::move(value));
    }

    auto ptrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    llvm::Value *intAddress = m_builder->getInt64((uintptr_t)constValues[0].get());
    llvm::Value *ptr1 = m_builder->CreateIntToPtr(intAddress, ptrType);

    intAddress = m_builder->getInt64((uintptr_t)constValues[1].get());
    llvm::Value *ptr2 = m_builder->CreateIntToPtr(intAddress, ptrType);

    addTestPrintFunction(ptr1, ptr2);
    endFunction(0);

    LLVMExecutionContext ctx(&m_target);
    LLVMExecutableCode code(std::move(m_module), constValues);
    ASSERT_TRUE(constValues.empty());
    ASSERT_FALSE(code.isFinished(&ctx));

    testing::internal::CaptureStdout();
    code.run(&ctx);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1 2\n");
    ASSERT_TRUE(code.isFinished(&ctx));
}
