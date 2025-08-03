#pragma once

#include <scratchcpp/compiler.h>
#include <enginemock.h>
#include <targetmock.h>
#include <randomgeneratormock.h>

#define ASSERT_NUM_OP1(utils, type, rawArg, v) ASSERT_EQ(utils.getOpResult(type, rawArg, v).toDouble(), m_utils.getExpectedOpResult(type, v).toDouble());
#define ASSERT_NUM_OP2(utils, type, rawArgs, v1, v2) ASSERT_EQ(utils.getOpResult(type, rawArgs, v1, v2).toDouble(), m_utils.getExpectedOpResult(type, v1, v2).toDouble());

#define ASSERT_BOOL_OP1(utils, type, rawArg, v) ASSERT_EQ(utils.getOpResult(type, rawArg, v).toBool(), m_utils.getExpectedOpResult(type, v).toBool());
#define ASSERT_BOOL_OP2(utils, type, rawArgs, v1, v2) ASSERT_EQ(utils.getOpResult(type, rawArgs, v1, v2).toBool(), m_utils.getExpectedOpResult(type, v1, v2).toBool());

#define ASSERT_STRING_OP1(utils, type, rawArg, v) ASSERT_EQ(utils.getOpResult(type, rawArg, v).toString(), m_utils.getExpectedOpResult(type, v).toString());
#define ASSERT_STRING_OP2(utils, type, rawArgs, v1, v2) ASSERT_EQ(utils.getOpResult(type, rawArgs, v1, v2).toString(), m_utils.getExpectedOpResult(type, v1, v2).toString());

namespace libscratchcpp
{

class LLVMCompilerContext;
class LLVMCodeBuilder;

class LLVMTestUtils
{
    public:
        enum class OpType
        {
            Add,
            Sub,
            Mul,
            Div,
            Random,
            RandomInt,
            CmpEQ,
            CmpGT,
            CmpLT,
            StrCmpEQCS,
            StrCmpEQCI,
            And,
            Or,
            Not,
            Mod,
            Round,
            Abs,
            Floor,
            Ceil,
            Sqrt,
            Sin,
            Cos,
            Tan,
            Asin,
            Acos,
            Atan,
            Ln,
            Log10,
            Exp,
            Exp10,
            StringConcat,
            StringChar,
            StringLength
        };

        LLVMTestUtils();

        LLVMCodeBuilder *createBuilder(Target *target, BlockPrototype *procedurePrototype, Compiler::CodeType codeType = Compiler::CodeType::Script);
        LLVMCodeBuilder *createBuilder(Target *target, bool warp);
        LLVMCodeBuilder *createBuilder(bool warp);

        LLVMCodeBuilder *createReporterBuilder(Target *target);
        LLVMCodeBuilder *createPredicateBuilder(Target *target);

        CompilerValue *callConstFuncForType(ValueType type, CompilerValue *arg);

        Value getOpResult(OpType type, bool rawArg, const Value &v);
        Value getOpResult(OpType type, bool rawArgs, const Value &v1, const Value &v2);

        Value getExpectedOpResult(OpType type, const Value &v);
        Value getExpectedOpResult(OpType type, const Value &v1, const Value &v2);

        const RandomGeneratorMock &rng() const;

    private:
        CompilerValue *addOp(OpType type, CompilerValue *arg);
        CompilerValue *addOp(OpType type, CompilerValue *arg1, CompilerValue *arg2);

        std::unordered_map<Target *, std::shared_ptr<LLVMCompilerContext>> m_contexts;
        std::vector<std::shared_ptr<LLVMCompilerContext>> m_contextList;
        std::shared_ptr<LLVMCodeBuilder> m_builder;
        std::shared_ptr<BlockPrototype> m_procedurePrototype;
        EngineMock m_engine;
        TargetMock m_target; // NOTE: isStage() is used for call expectations
        RandomGeneratorMock m_rng;
};

} // namespace libscratchcpp
