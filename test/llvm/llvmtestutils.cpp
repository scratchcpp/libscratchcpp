#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <engine/internal/llvm/llvmcodebuilder.h>
#include <engine/internal/llvm/llvmcompilercontext.h>

#include "llvmtestutils.h"
#include "testfunctions.h"

using namespace libscratchcpp;

LLVMTestUtils::LLVMTestUtils()
{
    test_function(nullptr, nullptr, nullptr, nullptr, nullptr); // force dependency
}

LLVMCodeBuilder *LLVMTestUtils::createBuilder(Target *target, BlockPrototype *procedurePrototype, Compiler::CodeType codeType)
{
    if (m_contexts.find(target) == m_contexts.cend() || !target)
        m_contexts[target] = std::make_shared<LLVMCompilerContext>(&m_engine, target);

    m_contextList.push_back(m_contexts[target]);
    m_builder = std::make_shared<LLVMCodeBuilder>(m_contexts[target].get(), procedurePrototype, codeType);

    return m_builder.get();
}

LLVMCodeBuilder *LLVMTestUtils::createBuilder(Target *target, bool warp)
{
    m_procedurePrototype = std::make_shared<BlockPrototype>("test");
    m_procedurePrototype->setWarp(warp);
    createBuilder(target, m_procedurePrototype.get());

    return m_builder.get();
}

LLVMCodeBuilder *LLVMTestUtils::createBuilder(bool warp)
{
    return createBuilder(nullptr, warp);
}

LLVMCodeBuilder *LLVMTestUtils::createReporterBuilder(Target *target)
{
    return createBuilder(target, nullptr, Compiler::CodeType::Reporter);
}

LLVMCodeBuilder *LLVMTestUtils::createPredicateBuilder(Target *target)
{
    return createBuilder(target, nullptr, Compiler::CodeType::HatPredicate);
}

CompilerValue *LLVMTestUtils::callConstFuncForType(ValueType type, CompilerValue *arg)
{
    switch (type) {
        case ValueType::Number:
            return m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { arg });

        case ValueType::Bool:
            return m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { arg });

        case ValueType::String:
            return m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { arg });

        default:
            EXPECT_TRUE(false);
            return nullptr;
    }
}

CompilerValue *LLVMTestUtils::callConstFuncForType(ValueType type, CompilerValue *arg, Compiler::StaticType returnType)
{
    switch (type) {
        case ValueType::Number:
            return m_builder->addFunctionCall("test_const_number_unknown", returnType, { Compiler::StaticType::Number }, { arg });

        case ValueType::Bool:
            return m_builder->addFunctionCall("test_const_bool_unknown", returnType, { Compiler::StaticType::Bool }, { arg });

        case ValueType::String:
            return m_builder->addFunctionCall("test_const_string_unknown", returnType, { Compiler::StaticType::String }, { arg });

        default:
            EXPECT_TRUE(false);
    }

    return nullptr;
}

Value LLVMTestUtils::getOpResult(OpType type, bool rawArg, const Value &v)
{
    createReporterBuilder(nullptr);

    CompilerValue *arg = m_builder->addConstValue(v);

    if (rawArg)
        addOp(type, arg);
    else {
        arg = callConstFuncForType(v.type(), arg);
        addOp(type, arg);
    }

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    ValueData data = code->runReporter(ctx.get());
    Value ret(data);
    value_free(&data);

    return ret;
}

Value LLVMTestUtils::getOpResult(OpType type, bool rawArgs, const Value &v1, const Value &v2)
{
    createReporterBuilder(nullptr);

    CompilerValue *arg1 = m_builder->addConstValue(v1);
    CompilerValue *arg2 = m_builder->addConstValue(v2);

    if (rawArgs)
        addOp(type, arg1, arg2);
    else {
        arg1 = callConstFuncForType(v1.type(), arg1);
        arg2 = callConstFuncForType(v2.type(), arg2);
        addOp(type, arg1, arg2);
    }

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    ValueData data = code->runReporter(ctx.get());
    Value ret(data);
    value_free(&data);

    return ret;
}

Value LLVMTestUtils::getExpectedOpResult(OpType type, const Value &v)
{
    switch (type) {
        case OpType::Not:
            return !v.toBool();

        case OpType::StringLength:
            return v.toUtf16().size();

        default:
            EXPECT_TRUE(false);
            return Value();
    }
}

Value LLVMTestUtils::getExpectedOpResult(OpType type, const Value &v1, const Value &v2)
{
    switch (type) {
        case OpType::Add:
            return v1 + v2;

        case OpType::Sub:
            return v1 - v2;

        case OpType::Mul:
            return v1 * v2;

        case OpType::Div:
            return v1 / v2;

        case OpType::Random: {
            const double sum = v1.toDouble() + v2.toDouble();

            if (std::isnan(sum) || std::isinf(sum))
                return sum;

            return v1.isInt() && v2.isInt() ? m_rng.randint(v1.toLong(), v2.toLong()) : m_rng.randintDouble(v1.toDouble(), v2.toDouble());
        }

        case OpType::RandomInt:
            return m_rng.randint(v1.toLong(), v2.toLong());

        case OpType::CmpEQ:
            return v1 == v2;

        case OpType::CmpGT:
            return v1 > v2;

        case OpType::CmpLT:
            return v1 < v2;

        case OpType::StrCmpEQCS:
            return string_compare_case_sensitive(value_toStringPtr(&v1.data()), value_toStringPtr(&v2.data())) == 0;

        case OpType::StrCmpEQCI:
            return string_compare_case_insensitive(value_toStringPtr(&v1.data()), value_toStringPtr(&v2.data())) == 0;

        case OpType::And:
            return v1.toBool() && v2.toBool();

        case OpType::Or:
            return v1.toBool() || v2.toBool();

        case OpType::Mod:
            return v1 % v2;

        default:
            EXPECT_TRUE(false);
            return Value();
    }
}

EngineMock &LLVMTestUtils::engine()
{
    return m_engine;
}

TargetMock &LLVMTestUtils::target()
{
    return m_target;
}

const RandomGeneratorMock &LLVMTestUtils::rng() const
{
    return m_rng;
}

CompilerValue *LLVMTestUtils::addOp(OpType type, CompilerValue *arg)
{
    switch (type) {
        case OpType::Not:
            return m_builder->createNot(arg);

        case OpType::Round:
            return m_builder->createRound(arg);

        case OpType::Abs:
            return m_builder->createAbs(arg);

        case OpType::Floor:
            return m_builder->createFloor(arg);

        case OpType::Ceil:
            return m_builder->createCeil(arg);

        case OpType::Sqrt:
            return m_builder->createSqrt(arg);

        case OpType::Sin:
            return m_builder->createSin(arg);

        case OpType::Cos:
            return m_builder->createCos(arg);

        case OpType::Tan:
            return m_builder->createTan(arg);

        case OpType::Asin:
            return m_builder->createAsin(arg);

        case OpType::Acos:
            return m_builder->createAcos(arg);

        case OpType::Atan:
            return m_builder->createAtan(arg);

        case OpType::Ln:
            return m_builder->createLn(arg);

        case OpType::Log10:
            return m_builder->createLog10(arg);

        case OpType::Exp:
            return m_builder->createExp(arg);

        case OpType::Exp10:
            return m_builder->createExp10(arg);

        case OpType::StringLength:
            return m_builder->addStringLength(arg);

        default:
            EXPECT_TRUE(false);
            return nullptr;
    }
}

CompilerValue *LLVMTestUtils::addOp(OpType type, CompilerValue *arg1, CompilerValue *arg2)
{
    switch (type) {
        case OpType::Add:
            return m_builder->createAdd(arg1, arg2);

        case OpType::Sub:
            return m_builder->createSub(arg1, arg2);

        case OpType::Mul:
            return m_builder->createMul(arg1, arg2);

        case OpType::Div:
            return m_builder->createDiv(arg1, arg2);

        case OpType::Random:
            return m_builder->createRandom(arg1, arg2);

        case OpType::RandomInt:
            return m_builder->createRandomInt(arg1, arg2);

        case OpType::CmpEQ:
            return m_builder->createCmpEQ(arg1, arg2);

        case OpType::CmpGT:
            return m_builder->createCmpGT(arg1, arg2);

        case OpType::CmpLT:
            return m_builder->createCmpLT(arg1, arg2);

        case OpType::StrCmpEQCS:
            return m_builder->createStrCmpEQ(arg1, arg2, true);

        case OpType::StrCmpEQCI:
            return m_builder->createStrCmpEQ(arg1, arg2, false);

        case OpType::And:
            return m_builder->createAnd(arg1, arg2);

        case OpType::Or:
            return m_builder->createOr(arg1, arg2);

        case OpType::Mod:
            return m_builder->createMod(arg1, arg2);

        case OpType::StringConcat:
            return m_builder->createStringConcat(arg1, arg2);

        case OpType::StringChar:
            return m_builder->addStringChar(arg1, arg2);

        default:
            EXPECT_TRUE(false);
            return nullptr;
    }
}
