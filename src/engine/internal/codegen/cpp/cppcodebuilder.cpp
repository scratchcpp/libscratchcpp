// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerlocalvariable.h>

#include "cppcodebuilder.h"
#include "cppexecutablecode.h"
#include "cppconstant.h"
#include "../typeutils.h"

using namespace libscratchcpp;

CppCodeBuilder::CppCodeBuilder(CppCompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_procedurePrototype(procedurePrototype),
    m_codeType(codeType)
{
}

std::shared_ptr<ExecutableCode> CppCodeBuilder::build()
{
    return std::make_shared<CppExecutableCode>(m_ctx);
}

CompilerValue *CppCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return addConstValue(Value());
}

CompilerConstant *CppCodeBuilder::addConstValue(const Value &value)
{
    auto constValue = std::make_shared<CppConstant>(TypeUtils::mapType(value.type()), value);
    auto genericValue = std::static_pointer_cast<CppValue>(constValue);
    return static_cast<CompilerConstant *>(static_cast<CppConstant *>(addValue(genericValue, nullptr)));
}

CompilerValue *CppCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addStringLength(CompilerValue *string)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addLoopIndex()
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addVariableValue(Variable *variable)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListContents(List *list)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addListSize(List *list)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::addProcedureArgument(const std::string &name)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createNot(CompilerValue *operand)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createRound(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAbs(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createFloor(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createCeil(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createSqrt(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createSin(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createCos(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createTan(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAsin(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAcos(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createAtan(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createLn(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createLog10(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createExp(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createExp10(CompilerValue *num)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    return addConstValue(Value());
}

CompilerValue *CppCodeBuilder::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType)
{
    return addConstValue(Value());
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
}

void CppCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
}

void CppCodeBuilder::createListClear(List *list)
{
}

void CppCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
}

void CppCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
}

void CppCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
}

void CppCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
}

void CppCodeBuilder::beginIfStatement(CompilerValue *cond)
{
}

void CppCodeBuilder::beginElseBranch()
{
}

void CppCodeBuilder::endIf()
{
}

void CppCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
}

void CppCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
}

void CppCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
}

void CppCodeBuilder::beginLoopCondition()
{
}

void CppCodeBuilder::endLoop()
{
}

void CppCodeBuilder::yield()
{
}

void CppCodeBuilder::createStop()
{
}

void CppCodeBuilder::createThreadStop()
{
}

void CppCodeBuilder::invalidateTarget()
{
}

void CppCodeBuilder::createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args)
{
}

void CppCodeBuilder::createPrint(CompilerValue *string)
{
}

CppValue *CppCodeBuilder::addValue(std::shared_ptr<CppValue> value, std::shared_ptr<CppInstruction> ins)
{
    value->instruction = ins;
    m_values.push_back(value);
    return value.get();
}
