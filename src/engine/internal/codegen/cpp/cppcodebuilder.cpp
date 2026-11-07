// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerlocalvariable.h>

#include "cppcodebuilder.h"
#include "cppcompilercontext.h"
#include "cppexecutablecode.h"
#include "cppinstruction.h"
#include "cppconstant.h"
#include "../typeutils.h"

using namespace libscratchcpp;

CppCodeBuilder::CppCodeBuilder(CppCompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_procedurePrototype(procedurePrototype),
    m_codeType(codeType),
    m_codeAnalyzer(ctx)
{
}

std::shared_ptr<ExecutableCode> CppCodeBuilder::build()
{
    if (!m_warp) {
        // Do not create coroutine if there are no yield instructions nor non-warp procedure calls
        if (!m_instructions.containsInstruction([](const CppInstruction *ins) {
                return ins->type == CppInstruction::Type::Yield || (ins->type == CppInstruction::Type::CallProcedure && ins->procedurePrototype && !ins->procedurePrototype->warp());
            })) {
            m_warp = true;
        }

        // Only create coroutines in scripts
        if (m_codeType != Compiler::CodeType::Script)
            m_warp = true;
    }

    if (m_warp) {
#ifdef ENABLE_CODE_ANALYZER
        // Analyze the script (type analysis, optimizations, etc.)
        // NOTE: Do this only for warp scripts
        m_codeAnalyzer.analyzeScript(m_instructions);
#endif
    }

    // Build recorded instructions
    CppInstruction *ins = m_instructions.first();

    while (ins)
        ins = m_instructionBuilder.process(ins);

    return std::make_shared<CppExecutableCode>(m_ctx);
}

CompilerValue *CppCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    assert(argTypes.size() == args.size());

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::FunctionCall, m_loopCondition);
    ins->functionName = functionName;

    for (size_t i = 0; i < args.size(); i++)
        ins->args.push_back({ argTypes[i], dynamic_cast<CppValue *>(args[i]) });

    m_instructions.addInstruction(ins);

    if (returnType != Compiler::StaticType::Void) {
        auto reg = std::make_shared<CppValue>(returnType);

        if (TypeUtils::isSingleType(returnType))
            reg->isRawValue = true;

        ins->setFunctionReturnReg(reg.get());
        return addValue(reg, ins);
    }

    return nullptr;
}

CompilerValue *CppCodeBuilder::addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    CompilerValue *ret = addFunctionCall(functionName, returnType, argTypes, args);
    m_instructions.last()->functionTargetArg = true;
    return ret;
}

CompilerValue *CppCodeBuilder::addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    CompilerValue *ret = addFunctionCall(functionName, returnType, argTypes, args);
    m_instructions.last()->functionCtxArg = true;
    return ret;
}

CompilerConstant *CppCodeBuilder::addConstValue(const Value &value)
{
    auto constValue = std::make_shared<CppConstant>(TypeUtils::mapType(value.type()), value);
    auto genericValue = std::static_pointer_cast<CppValue>(constValue);
    return static_cast<CompilerConstant *>(static_cast<CppConstant *>(addValue(genericValue, nullptr)));
}

CompilerValue *CppCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    return createOp(CppInstruction::Type::StringChar, Compiler::StaticType::String, { Compiler::StaticType::String, Compiler::StaticType::Number }, { string, index });
}

CompilerValue *CppCodeBuilder::addStringLength(CompilerValue *string)
{
    return createOp(CppInstruction::Type::StringLength, Compiler::StaticType::Number, Compiler::StaticType::String, { string });
}

CompilerValue *CppCodeBuilder::addLoopIndex()
{
    return createOp(CppInstruction::Type::LoopIndex, Compiler::StaticType::Number, {}, {});
}

CompilerValue *CppCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    // TODO: Add support for local variables
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addVariableValue(Variable *variable)
{
    // TODO: Add support for variables
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListContents(List *list)
{
    // TODO: Add support for lists
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    // TODO: Add support for lists
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    // TODO: Add support for lists
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    // TODO: Add support for lists
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListSize(List *list)
{
    // TODO: Add support for lists
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addProcedureArgument(const std::string &name)
{
    if (!m_procedurePrototype)
        return addConstValue(Value());

    const auto &argNames = m_procedurePrototype->argumentNames();
    auto it = std::find(argNames.begin(), argNames.end(), name);

    if (it == argNames.end()) {
        std::cout << "warning: could not find argument '" << name << "' in custom block '" << m_procedurePrototype->procCode() << "'" << std::endl;
        return addConstValue(Value());
    }

    const auto index = it - argNames.begin();
    const Compiler::StaticType type = getProcedureArgType(m_procedurePrototype->argumentTypes()[index]);
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::ProcedureArg, m_loopCondition);
    auto ret = std::make_shared<CppValue>(type);
    ret->isRawValue = (type != Compiler::StaticType::Unknown);
    ins->setFunctionReturnReg(ret.get());
    ins->procedureArgIndex = index;

    m_instructions.addInstruction(ins);
    return addValue(ret, ins);
}

CompilerValue *CppCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::Add, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::Sub, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::Mul, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::Div, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    return createOp(CppInstruction::Type::Random, Compiler::StaticType::Number, Compiler::StaticType::Unknown, { from, to });
}

CompilerValue *CppCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return createOp(CppInstruction::Type::RandomInt, Compiler::StaticType::Number, Compiler::StaticType::Number, { from, to });
}

CompilerValue *CppCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::CmpEQ, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::CmpGT, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::CmpLT, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    return createOp(caseSensitive ? CppInstruction::Type::StrCmpEQCS : CppInstruction::Type::StrCmpEQCI, Compiler::StaticType::Bool, Compiler::StaticType::String, { string1, string2 });
}

CompilerValue *CppCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::And, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(CppInstruction::Type::Or, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand1, operand2 });
}

CompilerValue *CppCodeBuilder::createNot(CompilerValue *operand)
{
    return createOp(CppInstruction::Type::Not, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand });
}

CompilerValue *CppCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return createOp(CppInstruction::Type::Mod, Compiler::StaticType::Number, Compiler::StaticType::Number, { num1, num2 });
}

CompilerValue *CppCodeBuilder::createRound(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Round, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createAbs(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Abs, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createFloor(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Floor, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createCeil(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Ceil, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createSqrt(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Sqrt, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createSin(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Sin, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createCos(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Cos, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createTan(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Tan, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createAsin(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Asin, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createAcos(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Acos, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createAtan(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Atan, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createLn(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Ln, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createLog10(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Log10, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createExp(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Exp, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createExp10(CompilerValue *num)
{
    return createOp(CppInstruction::Type::Exp10, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *CppCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    return createOp(CppInstruction::Type::StringConcat, Compiler::StaticType::String, Compiler::StaticType::String, { string1, string2 });
}

CompilerValue *CppCodeBuilder::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType)
{
    CppValue *ret = createOp(CppInstruction::Type::Select, valueType, { Compiler::StaticType::Bool, valueType, valueType }, { cond, trueValue, falseValue });
    ret->isRawValue = TypeUtils::isSingleType(valueType);
    return ret;
}

CompilerLocalVariable *CppCodeBuilder::createLocalVariable(Compiler::StaticType type)
{
    // TODO: Complete this
    CompilerValue *ptr = addConstValue(Value());
    auto var = std::make_shared<CompilerLocalVariable>(ptr);
    m_localVars.push_back(var);
    return var.get();
}

void CppCodeBuilder::createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value)
{
    // TODO: Add support for local variables
}

void CppCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
    // TODO: Add support for variables
}

void CppCodeBuilder::createListClear(List *list)
{
    // TODO: Add support for lists
}

void CppCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    // TODO: Add support for lists
}

void CppCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    // TODO: Add support for lists
}

void CppCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    // TODO: Add support for lists
}

void CppCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    // TODO: Add support for lists
}

void CppCodeBuilder::beginIfStatement(CompilerValue *cond)
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginIf, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<CppValue *>(cond) });
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::beginElseBranch()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginElse, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::endIf()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::EndIf, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
    assert(!m_loopCondition);

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginRepeatLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Number, dynamic_cast<CppValue *>(count) });
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
    assert(m_loopCondition);
    m_loopCondition = false;

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginWhileLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<CppValue *>(cond) });
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
    assert(m_loopCondition);
    m_loopCondition = false;

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginRepeatUntilLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<CppValue *>(cond) });
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::beginLoopCondition()
{
    assert(!m_loopCondition);

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::BeginLoopCondition, m_loopCondition);
    m_instructions.addInstruction(ins);
    m_loopCondition = true;
}

void CppCodeBuilder::endLoop()
{
    if (!m_warp) {
        auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::Yield, m_loopCondition);
        m_instructions.addInstruction(ins);
    }

    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::EndLoop, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::yield()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::Yield, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::createStop()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::Stop, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::createThreadStop()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::ThreadStop, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::invalidateTarget()
{
    auto ins = std::make_shared<CppInstruction>(CppInstruction::Type::InvalidateTarget, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void CppCodeBuilder::createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args)
{
    assert(prototype);
    assert(prototype->argumentTypes().size() == args.size());
    const auto &procedureArgs = prototype->argumentTypes();
    Compiler::ArgTypes types;

    for (BlockPrototype::ArgType type : procedureArgs)
        types.push_back(getProcedureArgType(type));

    CppInstruction ins(CppInstruction::Type::CallProcedure, m_loopCondition);
    ins.procedurePrototype = prototype;
    createOp(ins, Compiler::StaticType::Void, types, args);
}

void CppCodeBuilder::createPrint(CompilerValue *string)
{
    createOp(CppInstruction::Type::Print, Compiler::StaticType::Void, Compiler::StaticType::String, { string });
}

CppValue *CppCodeBuilder::addValue(std::shared_ptr<CppValue> value, std::shared_ptr<CppInstruction> ins)
{
    value->instruction = ins;
    m_values.push_back(value);
    return value.get();
}

Compiler::StaticType CppCodeBuilder::getProcedureArgType(BlockPrototype::ArgType type)
{
    return type == BlockPrototype::ArgType::Bool ? Compiler::StaticType::Bool : Compiler::StaticType::Unknown;
}

CppValue *CppCodeBuilder::createOp(CppInstruction::Type type, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args)
{
    return createOp({ type, m_loopCondition }, retType, argType, args);
}

CppValue *CppCodeBuilder::createOp(CppInstruction::Type type, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createOp({ type, m_loopCondition }, retType, argTypes, args);
}

CppValue *CppCodeBuilder::createOp(const CppInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args)
{
    std::vector<Compiler::StaticType> types;
    types.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++)
        types.push_back(argType);

    return createOp(ins, retType, types, args);
}

CppValue *CppCodeBuilder::createOp(const CppInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    auto createdIns = std::make_shared<CppInstruction>(ins);
    m_instructions.addInstruction(createdIns);

    for (size_t i = 0; i < args.size(); i++)
        createdIns->args.push_back({ argTypes[i], dynamic_cast<CppValue *>(args[i]) });

    if (retType != Compiler::StaticType::Void) {
        auto ret = std::make_shared<CppValue>(retType);
        ret->isRawValue = true;
        createdIns->functionReturnReg = ret.get();
        return addValue(ret, createdIns);
    }

    return nullptr;
}
