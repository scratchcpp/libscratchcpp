// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/value.h>

#include "../codegentestutils.h"
#include "../testfunctions.h"

using namespace libscratchcpp;

class GeneratedCppExecutionTest : public testing::Test
{
    protected:
        std::shared_ptr<ExecutionContext> createContext(const std::shared_ptr<ExecutableCode> &code)
        {
            m_thread.reset();
            m_script = std::make_unique<Script>(&m_utils.target(), nullptr, nullptr);
            m_script->setCode(code);
            m_thread = std::make_unique<Thread>(&m_utils.target(), nullptr, m_script.get());
            return code->createExecutionContext(m_thread.get());
        }

        CodeGenTestUtils m_utils;
        std::unique_ptr<Script> m_script;
        std::unique_ptr<Thread> m_thread;
};

// The generator pass records builders; codegen_test executes the compiled output.
TEST_F(GeneratedCppExecutionTest, ScriptRunsNativeCalls)
{
    auto *builder = m_utils.createBuilder(true);
    builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    auto code = builder->build();
#ifdef LIBSCRATCHCPP_CPP_TEST_GENERATOR
    return;
#endif
    auto ctx = createContext(code);
    EXPECT_FALSE(code->isFinished(ctx.get()));
    code->run(ctx.get());
    EXPECT_EQ(test_get_counter(), 2);
    EXPECT_TRUE(code->isFinished(ctx.get()));
}

TEST_F(GeneratedCppExecutionTest, NumericReporterReturnsNativeResult)
{
    auto *builder = m_utils.createReporterBuilder(nullptr);
    auto *value = builder->addConstValue(42.5);
    builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { value });
    auto code = builder->build();
#ifdef LIBSCRATCHCPP_CPP_TEST_GENERATOR
    return;
#endif
    auto ctx = createContext(code);
    ValueData result = code->runReporter(ctx.get());
    EXPECT_TRUE(value_isNumber(&result));
    EXPECT_EQ(value_toDouble(&result), 42.5);
    value_free(&result);
}

TEST_F(GeneratedCppExecutionTest, StringReporterReturnsOwnedCopy)
{
    auto *builder = m_utils.createReporterBuilder(nullptr);
    auto *constant = builder->addConstValue("generated string");
    auto *string = builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { constant });
    builder->addFunctionCall("test_const_unknown", Compiler::StaticType::Unknown, { Compiler::StaticType::Unknown }, { string });
    auto code = builder->build();
#ifdef LIBSCRATCHCPP_CPP_TEST_GENERATOR
    return;
#endif
    auto ctx = createContext(code);
    for (int i = 0; i < 20; ++i) {
        ValueData result = code->runReporter(ctx.get());
        EXPECT_TRUE(value_isString(&result));
        EXPECT_EQ(Value(result).toString(), "generated string");
        value_free(&result);
    }
}

TEST_F(GeneratedCppExecutionTest, PredicateReturnsNativeResult)
{
    for (bool expected : { false, true }) {
        auto *builder = m_utils.createPredicateBuilder(nullptr);
        auto *value = builder->addConstValue(expected);
        builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { value });
        auto code = builder->build();
#ifdef LIBSCRATCHCPP_CPP_TEST_GENERATOR
        continue;
#endif
        auto ctx = createContext(code);
        EXPECT_EQ(code->runPredicate(ctx.get()), expected);
    }
}
