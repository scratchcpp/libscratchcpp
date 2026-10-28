// SPDX-License-Identifier: Apache-2.0

#include "cppcodebuilder.h"
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
    return nullptr;
}

CompilerValue *CppCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return nullptr;
}

CompilerConstant *CppCodeBuilder::addConstValue(const Value &value)
{
    auto constValue = std::make_shared<CppConstant>(TypeUtils::mapType(value.type()), value);
    auto genericValue = std::static_pointer_cast<CppValue>(constValue);
    return static_cast<CompilerConstant *>(static_cast<CppConstant *>(addValue(genericValue, nullptr)));
}

CompilerValue *CppCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addStringLength(CompilerValue *string)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addLoopIndex()
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addVariableValue(Variable *variable)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addListContents(List *list)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addListSize(List *list)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::addProcedureArgument(const std::string &name)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createNot(CompilerValue *operand)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createRound(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAbs(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createFloor(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createCeil(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createSqrt(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createSin(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createCos(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createTan(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAsin(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAcos(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createAtan(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createLn(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createLog10(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createExp(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createExp10(CompilerValue *num)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    return nullptr;
}

CompilerValue *CppCodeBuilder::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType)
{
    return nullptr;
}

CompilerLocalVariable *CppCodeBuilder::createLocalVariable(Compiler::StaticType type)
{
    return nullptr;
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
