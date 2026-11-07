// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/blockprototype.h>
#include <scratchcpp/compilerlocalvariable.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "cppcodebuilder.h"
#include "cppcompilercontext.h"
#include "cppexecutablecode.h"
#include "cppconstant.h"
#include "../typeutils.h"

using namespace libscratchcpp;

static const std::string RETURN_PLACEHOLDER = "$RETURN(";
static const std::string CLASS_PLACEHOLDER = "$CLASS";

CppCodeBuilder::CppCodeBuilder(CppCompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_procedurePrototype(procedurePrototype),
    m_codeType(codeType)
{
    assert(m_ctx);
}

std::shared_ptr<ExecutableCode> CppCodeBuilder::build()
{
    // Create the return value of reporters and hat predicates
    switch (m_codeType) {
        case Compiler::CodeType::Script:
            emitReturn(false);
            break;

        case Compiler::CodeType::Reporter: {
            assert(m_lastValue);
            std::string v = m_lastValue ? cast(m_lastValue, Compiler::StaticType::Unknown) : "";

            if (m_lastValue) {
                emit("ValueData ret;");
                emit("value_init(&ret);");
                emit("value_assign_copy(&ret, &" + v + ");");
                emit("return ret;");
            } else {
                emit("ValueData ret;");
                emit("value_init(&ret);");
                emit("return ret;");
            }

            break;
        }

        case Compiler::CodeType::HatPredicate:
            assert(m_lastValue);
            emit("return " + (m_lastValue ? cast(m_lastValue, Compiler::StaticType::Bool) : std::string("false")) + ";");
            break;
    }

    // Name generated classes after the hash of their source, so that they can be found when the code is built again
    uint64_t hash = CppCompilerContext::hash(generateSource(CLASS_PLACEHOLDER));
    std::string className = CppCompilerContext::className(hash);
    std::stringstream source;
    source << "namespace\n{\n\n";
    source << generateSource(className) << "\n";
    source << "const bool " << className << "_registered = (CppCompilerContext::registerCode(\n";
    source << "                                                 0x" << std::hex << hash << std::dec << "ULL,\n";
    source << "                                                 [](CppCompilerContext *ctx, const std::vector<void *> &pointers) -> std::shared_ptr<CppExecutableCode> {\n";
    source << "                                                     return std::make_shared<" << className << ">(ctx, pointers);\n";
    source << "                                                 }),\n";
    source << "                                             true);\n\n";
    source << "} // namespace\n";
    CppCompilerContext::reportSource(hash, m_externs.str() + "\n" + source.str());

    CppCompilerContext::CodeFactory factory = CppCompilerContext::findCode(hash);
    std::shared_ptr<CppExecutableCode> code;

    if (factory)
        code = factory(m_ctx, m_pointers);
    else
        code = std::make_shared<CppExecutableCode>(m_ctx, m_pointers);

    if (m_procedurePrototype)
        m_ctx->addProcedure(m_procedurePrototype->procCode(), code);

    return code;
}

CompilerValue *CppCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createFunctionCall(functionName, returnType, argTypes, args, "", "");
}

CompilerValue *CppCodeBuilder::addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createFunctionCall(functionName, returnType, argTypes, args, "Target *", "target");
}

CompilerValue *CppCodeBuilder::addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createFunctionCall(functionName, returnType, argTypes, args, "ExecutionContext *", "ctx");
}

CompilerConstant *CppCodeBuilder::addConstValue(const Value &value)
{
    Compiler::StaticType type = TypeUtils::mapType(value.type());
    auto constant = std::make_shared<CppConstant>(type, value);

    switch (value.type()) {
        case ValueType::Number:
            constant->expr = numberLiteral(value.data().numberValue);
            break;

        case ValueType::Bool:
            constant->expr = value.toBool() ? "true" : "false";
            break;

        case ValueType::String:
            constant->expr = addLocal(Storage::String, "c", stringLiteral(value.toString())) + ".s";
            break;

        case ValueType::Pointer:
            constant->expr = pointer(const_cast<void *>(value.toPointer()));
            break;

        default:
            assert(false);
            constant->expr = "0.0";
            break;
    }

    m_values.push_back(constant);
    m_lastValue = constant.get();
    return constant.get();
}

CompilerValue *CppCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    std::string str = cast(string, Compiler::StaticType::String);
    std::string i = cast(index, Compiler::StaticType::Number);
    addInstruction(CppInstruction::Type::StringChar);
    CppValue *reg = addRegister(Compiler::StaticType::String);
    emit("cpp::stringChar(" + reg->expr + ", " + str + ", " + i + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addStringLength(CompilerValue *string)
{
    return assignNumber(CppInstruction::Type::StringLength, "(double)" + cast(string, Compiler::StaticType::String) + "->size");
}

CompilerValue *CppCodeBuilder::addLoopIndex()
{
    for (auto it = m_loops.rbegin(); it != m_loops.rend(); it++) {
        if (it->isRepeatLoop)
            return assignNumber(CppInstruction::Type::LoopIndex, it->index);
    }

    return addConstValue(0);
}

CompilerValue *CppCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    addInstruction(CppInstruction::Type::ReadLocalVariable);
    CppValue *reg = addRegister(variable->type());
    assign(reg, variable->ptr());
    return reg;
}

CompilerValue *CppCodeBuilder::addVariableValue(Variable *variable)
{
    addInstruction(CppInstruction::Type::ReadVariable)->targetVariable = variable;
    CppValue *reg = addRegister(Compiler::StaticType::Unknown);
    emit("value_assign_copy(&" + reg->expr + ", " + variablePtr(variable) + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListContents(List *list)
{
    addInstruction(CppInstruction::Type::GetListContents)->targetList = list;
    CppValue *reg = addRegister(Compiler::StaticType::String);
    emit(listPtr(list) + "->toStringPtr(" + reg->expr + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    addInstruction(CppInstruction::Type::GetListItem)->targetList = list;
    CppValue *reg = addRegister(Compiler::StaticType::Unknown);
    emit("cpp::listItem(&" + reg->expr + ", " + listPtr(list) + ", " + i + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    CppValue *ret = assignNumber(CppInstruction::Type::GetListItemIndex, "cpp::listIndexOf(" + listPtr(list) + ", " + v + ")");
    m_instructions.last()->targetList = list;
    return ret;
}

CompilerValue *CppCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    CppValue *ret = assignBool(CppInstruction::Type::ListContainsItem, listPtr(list) + "->contains(" + v + ")");
    m_instructions.last()->targetList = list;
    return ret;
}

CompilerValue *CppCodeBuilder::addListSize(List *list)
{
    CppValue *ret = assignNumber(CppInstruction::Type::GetListSize, "(double)" + listPtr(list) + "->size()");
    m_instructions.last()->targetList = list;
    return ret;
}

CompilerValue *CppCodeBuilder::addProcedureArgument(const std::string &name)
{
    if (!m_procedurePrototype)
        return addConstValue(Value());

    const auto &names = m_procedurePrototype->argumentNames();
    const auto &types = m_procedurePrototype->argumentTypes();
    auto it = std::find(names.begin(), names.end(), name);

    if (it == names.end()) {
        std::cout << "warning: could not find argument '" << name << "' in custom block '" << m_procedurePrototype->procCode() << "'" << std::endl;
        return addConstValue(Value());
    }

    size_t index = it - names.begin();
    std::string arg = "args[" + std::to_string(index) + "]";

    if (index < types.size() && types[index] == BlockPrototype::ArgType::Bool)
        return assignBool(CppInstruction::Type::ProcedureArg, "value_toBool(&" + arg + ")");

    addInstruction(CppInstruction::Type::ProcedureArg)->procedureArgIndex = index;
    return addRegister(Compiler::StaticType::Unknown, arg);
}

CompilerValue *CppCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(CppInstruction::Type::Add, cast(operand1, Compiler::StaticType::Number) + " + " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(CppInstruction::Type::Sub, cast(operand1, Compiler::StaticType::Number) + " - " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(CppInstruction::Type::Mul, cast(operand1, Compiler::StaticType::Number) + " * " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(CppInstruction::Type::Div, cast(operand1, Compiler::StaticType::Number) + " / " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    // Strings can contain decimal numbers which are integers ("1.0"), so pass them as values
    auto isNumeric = [](CompilerValue *v) { return storage(v->type()) != Storage::Unknown && storage(v->type()) != Storage::String; };
    Compiler::StaticType type = isNumeric(from) && isNumeric(to) ? Compiler::StaticType::Number : Compiler::StaticType::Unknown;
    return assignNumber(CppInstruction::Type::Random, "cpp::random(ctx, " + cast(from, type) + ", " + cast(to, type) + ")");
}

CompilerValue *CppCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return assignNumber(CppInstruction::Type::RandomInt, "cpp::randomInt(ctx, " + cast(from, Compiler::StaticType::Number) + ", " + cast(to, Compiler::StaticType::Number) + ")");
}

CompilerValue *CppCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(CppInstruction::Type::CmpEQ, "value_equals(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(CppInstruction::Type::CmpGT, "value_greater(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(CppInstruction::Type::CmpLT, "value_lower(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    std::string func = caseSensitive ? "string_compare_case_sensitive" : "string_compare_case_insensitive";
    auto type = caseSensitive ? CppInstruction::Type::StrCmpEQCS : CppInstruction::Type::StrCmpEQCI;
    return assignBool(type, func + "(" + cast(string1, Compiler::StaticType::String) + ", " + cast(string2, Compiler::StaticType::String) + ") == 0");
}

CompilerValue *CppCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(CppInstruction::Type::And, cast(operand1, Compiler::StaticType::Bool) + " && " + cast(operand2, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(CppInstruction::Type::Or, cast(operand1, Compiler::StaticType::Bool) + " || " + cast(operand2, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createNot(CompilerValue *operand)
{
    return assignBool(CppInstruction::Type::Not, "!" + cast(operand, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return assignNumber(CppInstruction::Type::Mod, "cpp::mod(" + cast(num1, Compiler::StaticType::Number) + ", " + cast(num2, Compiler::StaticType::Number) + ")");
}

CompilerValue *CppCodeBuilder::createRound(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Round, num, "cpp::round");
}

CompilerValue *CppCodeBuilder::createAbs(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Abs, num, "std::fabs");
}

CompilerValue *CppCodeBuilder::createFloor(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Floor, num, "std::floor");
}

CompilerValue *CppCodeBuilder::createCeil(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Ceil, num, "std::ceil");
}

CompilerValue *CppCodeBuilder::createSqrt(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Sqrt, num, "std::sqrt");
}

CompilerValue *CppCodeBuilder::createSin(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Sin, num, "cpp::sin");
}

CompilerValue *CppCodeBuilder::createCos(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Cos, num, "cpp::cos");
}

CompilerValue *CppCodeBuilder::createTan(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Tan, num, "cpp::tan");
}

CompilerValue *CppCodeBuilder::createAsin(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Asin, num, "std::asin", " * 180.0 / cpp::PI + 0.0");
}

CompilerValue *CppCodeBuilder::createAcos(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Acos, num, "std::acos", " * 180.0 / cpp::PI");
}

CompilerValue *CppCodeBuilder::createAtan(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Atan, num, "std::atan", " * 180.0 / cpp::PI");
}

CompilerValue *CppCodeBuilder::createLn(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Ln, num, "std::log");
}

CompilerValue *CppCodeBuilder::createLog10(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Log10, num, "std::log10");
}

CompilerValue *CppCodeBuilder::createExp(CompilerValue *num)
{
    return mathFunction(CppInstruction::Type::Exp, num, "std::exp");
}

CompilerValue *CppCodeBuilder::createExp10(CompilerValue *num)
{
    return assignNumber(CppInstruction::Type::Exp10, "std::pow(10.0, cpp::num(" + cast(num, Compiler::StaticType::Number) + "))");
}

CompilerValue *CppCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    std::string str1 = cast(string1, Compiler::StaticType::String);
    std::string str2 = cast(string2, Compiler::StaticType::String);
    addInstruction(CppInstruction::Type::StringConcat);
    CppValue *reg = addRegister(Compiler::StaticType::String);
    emit("cpp::concat(" + reg->expr + ", " + str1 + ", " + str2 + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType)
{
    Storage s = storage(valueType);
    Compiler::StaticType castType = s == Storage::Unknown ? Compiler::StaticType::Unknown : valueType;
    std::string c = cast(cond, Compiler::StaticType::Bool);
    std::string v1 = cast(trueValue, castType);
    std::string v2 = cast(falseValue, castType);
    addInstruction(CppInstruction::Type::Select);
    CppValue *reg = addRegister(castType);
    std::string select = "(" + c + " ? " + v1 + " : " + v2 + ")";

    switch (s) {
        case Storage::String:
            emit("cpp::str(" + reg->expr + ", " + select + ");");
            break;

        case Storage::Unknown:
            emit("cpp::val(&" + reg->expr + ", " + select + ");");
            break;

        default:
            emit(reg->expr + " = " + select + ";");
            break;
    }

    return reg;
}

CompilerLocalVariable *CppCodeBuilder::createLocalVariable(Compiler::StaticType type)
{
    addInstruction(CppInstruction::Type::CreateLocalVariable);
    CppValue *reg = addRegister(type);
    auto var = std::make_shared<CompilerLocalVariable>(reg);
    m_localVars.push_back(var);
    return var.get();
}

void CppCodeBuilder::createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value)
{
    addInstruction(CppInstruction::Type::WriteLocalVariable);
    assign(dynamic_cast<CppValue *>(variable->ptr()), value);
}

void CppCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
    addInstruction(CppInstruction::Type::WriteVariable)->targetVariable = variable;
    std::string v = dynamic_cast<CppValue *>(value)->expr;

    if (storage(value->type()) == Storage::Pointer)
        emit("value_assign_pointer(" + variablePtr(variable) + ", " + v + ");");
    else
        emit("cpp::val(" + variablePtr(variable) + ", " + v + ");");
}

void CppCodeBuilder::createListClear(List *list)
{
    addInstruction(CppInstruction::Type::ClearList)->targetList = list;
    emit(listPtr(list) + "->clear();");
}

void CppCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    addInstruction(CppInstruction::Type::RemoveListItem)->targetList = list;
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + "))");
    emit("    " + l + "->removeAt((size_t)" + i + ");");
}

void CppCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    addInstruction(CppInstruction::Type::AppendToList)->targetList = list;
    emit(listPtr(list) + "->append(" + v + ");");
}

void CppCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    std::string v = cast(item, Compiler::StaticType::Unknown);
    addInstruction(CppInstruction::Type::InsertToList)->targetList = list;
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + ", true))");
    emit("    " + l + "->insert((size_t)" + i + ", " + v + ");");
}

void CppCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    std::string v = cast(item, Compiler::StaticType::Unknown);
    addInstruction(CppInstruction::Type::ListReplace)->targetList = list;
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + "))");
    emit("    " + l + "->replace((size_t)" + i + ", " + v + ");");
}

void CppCodeBuilder::beginIfStatement(CompilerValue *cond)
{
    std::string c = cast(cond, Compiler::StaticType::Bool);
    addInstruction(CppInstruction::Type::BeginIf);
    emit("if (" + c + ") {");
    indent();
}

void CppCodeBuilder::beginElseBranch()
{
    addInstruction(CppInstruction::Type::BeginElse);
    unindent();
    emit("} else {");
    indent();
}

void CppCodeBuilder::endIf()
{
    addInstruction(CppInstruction::Type::EndIf);
    unindent();
    emit("}");
}

void CppCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
    assert(!m_loopCondition);
    std::string c = cast(count, Compiler::StaticType::Number);
    addInstruction(CppInstruction::Type::BeginRepeatLoop);
    std::string n = addLocal(Storage::Number, "n");
    std::string index = addLocal(Storage::Number, "i");

    // Scratch rounds the count, a negative count skips the loop and infinity repeats forever
    emit(n + " = " + c + ";");
    emit(n + " = " + n + " > 0 ? (std::isinf(" + n + ") ? " + n + " : std::round(" + n + ")) : 0;");
    emit("for (" + index + " = 0; std::isinf(" + n + ") || " + index + " < " + n + "; " + index + "++) {");
    indent();

    Loop loop;
    loop.isRepeatLoop = true;
    loop.index = index;
    m_loops.push_back(loop);
}

void CppCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
    std::string c = cast(cond, Compiler::StaticType::Bool);
    addInstruction(CppInstruction::Type::BeginWhileLoop);

    if (!m_loopCondition) {
        emit("for (;;) {");
        indent();
    }

    emit("if (!" + c + ")");
    emit("    break;");
    m_loopCondition = false;
    m_loops.push_back(Loop());
}

void CppCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
    std::string c = cast(cond, Compiler::StaticType::Bool);
    addInstruction(CppInstruction::Type::BeginRepeatUntilLoop);

    if (!m_loopCondition) {
        emit("for (;;) {");
        indent();
    }

    emit("if (" + c + ")");
    emit("    break;");
    m_loopCondition = false;
    m_loops.push_back(Loop());
}

void CppCodeBuilder::beginLoopCondition()
{
    assert(!m_loopCondition);
    addInstruction(CppInstruction::Type::BeginLoopCondition);

    // The condition is evaluated at the beginning of every iteration
    emit("for (;;) {");
    indent();
    m_loopCondition = true;
}

void CppCodeBuilder::endLoop()
{
    assert(!m_loops.empty());
    emitYield();
    addInstruction(CppInstruction::Type::EndLoop);
    unindent();
    emit("}");
    m_loops.pop_back();
}

void CppCodeBuilder::yield()
{
    emitYield();
}

void CppCodeBuilder::createStop()
{
    addInstruction(CppInstruction::Type::Stop);
    emitReturn(false);
}

void CppCodeBuilder::createThreadStop()
{
    addInstruction(CppInstruction::Type::ThreadStop);
    emitReturn(m_codeType == Compiler::CodeType::Script);
}

void CppCodeBuilder::invalidateTarget()
{
    addInstruction(CppInstruction::Type::InvalidateTarget);
    emit("resolve();");
}

void CppCodeBuilder::createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args)
{
    assert(prototype);
    const auto &types = prototype->argumentTypes();
    assert(args.size() == types.size());
    std::vector<std::string> values;

    for (size_t i = 0; i < args.size(); i++)
        values.push_back(cast(args[i], types[i] == BlockPrototype::ArgType::Bool ? Compiler::StaticType::Bool : Compiler::StaticType::Unknown));

    addInstruction(CppInstruction::Type::CallProcedure)->procedurePrototype = prototype;
    std::string argArray = addLocal(Storage::Void, "pa", std::to_string(args.size()));

    for (size_t i = 0; i < args.size(); i++) {
        std::string dst = "&" + argArray + ".v[" + std::to_string(i) + "]";

        if (types[i] == BlockPrototype::ArgType::Bool)
            emit("value_assign_bool(" + dst + ", " + values[i] + ");");
        else
            emit("value_assign_copy(" + dst + ", &" + values[i] + ");");
    }

    std::string warp = isWarp() ? "true" : (m_procedurePrototype ? "warp" : "false");
    std::string coroutine = addLocal(Storage::Void, "pc");
    emit("if (CppExecutableCode *procedure = compilerContext()->procedure(" + stringLiteral(prototype->procCode()) + ")) {");
    indent();
    emit(coroutine + " = procedure->runProcedure(ctx, target, targetVariables, targetLists, " + warp + ", " + argArray + ".v);");

    if (!isWarp() && !prototype->warp()) {
        // Resume the procedure until it finishes
        m_suspends = true;
        emit("while (!" + coroutine + ".done()) {");
        emit("    co_await std::suspend_always{};");
        emit("    " + coroutine + ".resume();");
        emit("}");
    }

    emit("if (" + coroutine + ".endThread() || static_cast<CppExecutionContext *>(ctx)->finished())");
    m_indent++;
    emitReturn(true);
    m_indent--;
    emit(coroutine + " = CppCoroutine();");
    unindent();
    emit("}");
    emit("resolve();");
}

void CppCodeBuilder::createPrint(CompilerValue *string)
{
    std::string str = cast(string, Compiler::StaticType::String);
    addInstruction(CppInstruction::Type::Print);
    emit("cpp::print(" + str + ");");
}

std::shared_ptr<CppInstruction> CppCodeBuilder::addInstruction(CppInstruction::Type type)
{
    auto ins = std::make_shared<CppInstruction>(type, m_loopCondition);
    m_instructions.addInstruction(ins);
    m_lastInstruction = ins;
    return ins;
}

CppValue *CppCodeBuilder::addValue(std::shared_ptr<CppValue> value, std::shared_ptr<CppInstruction> ins)
{
    value->instruction = ins;

    if (ins)
        ins->functionReturnReg = value.get();

    m_values.push_back(value);
    m_lastValue = value.get();
    return value.get();
}

CppValue *CppCodeBuilder::addRegister(Compiler::StaticType type)
{
    Storage s = storage(type);
    std::string name = addLocal(s, "r");

    if (s == Storage::String)
        name += ".s";
    else if (s == Storage::Unknown)
        name += ".v";

    return addRegister(type, name);
}

CppValue *CppCodeBuilder::addRegister(Compiler::StaticType type, const std::string &expr)
{
    auto reg = std::make_shared<CppValue>(type);
    reg->expr = expr;
    reg->isRawValue = TypeUtils::isSingleType(type);
    return addValue(reg, m_lastInstruction);
}

CppValue *CppCodeBuilder::assignNumber(CppInstruction::Type type, const std::string &expr)
{
    addInstruction(type);
    CppValue *reg = addRegister(Compiler::StaticType::Number);
    emit(reg->expr + " = " + expr + ";");
    return reg;
}

CppValue *CppCodeBuilder::assignBool(CppInstruction::Type type, const std::string &expr)
{
    addInstruction(type);
    CppValue *reg = addRegister(Compiler::StaticType::Bool);
    emit(reg->expr + " = " + expr + ";");
    return reg;
}

CppValue *CppCodeBuilder::mathFunction(CppInstruction::Type type, CompilerValue *num, const std::string &func, const std::string &suffix)
{
    return assignNumber(type, func + "(cpp::num(" + cast(num, Compiler::StaticType::Number) + "))" + suffix);
}

CompilerValue *CppCodeBuilder::createFunctionCall(
    const std::string &functionName,
    Compiler::StaticType returnType,
    const Compiler::ArgTypes &argTypes,
    const Compiler::Args &args,
    const std::string &firstArgType,
    const std::string &firstArg)
{
    assert(argTypes.size() == args.size());
    // Functions are declared without parameters and cast to the actual type on call,
    // so different signatures of the same function don't conflict in a translation unit
    // Strings are returned through an output parameter
    static const std::string STRING_RET_PLACEHOLDER = "$STRING_RET";
    const bool stringRet = storage(returnType) == Storage::String;
    std::string type = (stringRet ? std::string("void") : cppType(returnType, false)) + " (*)(";
    std::string call = "(";
    bool first = true;

    if (stringRet) {
        type += "StringPtr *";
        call += STRING_RET_PLACEHOLDER;
        first = false;
    }

    if (!firstArgType.empty()) {
        if (!first) {
            type += ", ";
            call += ", ";
        }

        type += firstArgType;
        call += firstArg;
        first = false;
    }

    for (size_t i = 0; i < args.size(); i++) {
        std::string v = cast(args[i], argTypes[i]);

        if (!first) {
            type += ", ";
            call += ", ";
        }

        type += cppType(argTypes[i], true);
        call += storage(argTypes[i]) == Storage::Unknown ? "&" + v : v;
        first = false;
    }

    type += ")";

    // value_* and string_* functions are declared by libscratchcpp headers included in the prelude
    if (functionName.rfind("value_", 0) == 0 || functionName.rfind("string_", 0) == 0)
        call = functionName + call + ")";
    else {
        call = "reinterpret_cast<" + type + ">(&" + functionName + ")" + call + ")";

        if (m_declaredFunctions.insert(functionName).second)
            m_externs << "extern \"C\" void " << functionName << "();\n";
    }

    auto ins = addInstruction(CppInstruction::Type::FunctionCall);
    ins->functionName = functionName;
    ins->functionTargetArg = (firstArg == "target");
    ins->functionCtxArg = (firstArg == "ctx");

    switch (storage(returnType)) {
        case Storage::Void:
            emit(call + ";");
            return nullptr;

        case Storage::String: {
            CppValue *reg = addRegister(returnType);
            call.replace(call.find(STRING_RET_PLACEHOLDER), STRING_RET_PLACEHOLDER.size(), reg->expr);
            emit(call + ";");
            return reg;
        }

        case Storage::Unknown: {
            // The returned value is owned by the caller
            CppValue *reg = addRegister(returnType);
            std::string local = reg->expr.substr(0, reg->expr.size() - 2);
            emit(local + ".reset(" + call + ");");
            return reg;
        }

        default: {
            CppValue *reg = addRegister(returnType);
            emit(reg->expr + " = " + call + ";");
            return reg;
        }
    }
}

CppCodeBuilder::Storage CppCodeBuilder::storage(Compiler::StaticType type)
{
    switch (type) {
        case Compiler::StaticType::Void:
            return Storage::Void;
        case Compiler::StaticType::Number:
            return Storage::Number;
        case Compiler::StaticType::Bool:
            return Storage::Bool;
        case Compiler::StaticType::String:
            return Storage::String;
        case Compiler::StaticType::Pointer:
            return Storage::Pointer;
        default:
            return Storage::Unknown;
    }
}

std::string CppCodeBuilder::cppType(Compiler::StaticType type, bool arg)
{
    switch (storage(type)) {
        case Storage::Void:
            return "void";
        case Storage::Number:
            return "double";
        case Storage::Bool:
            return "bool";
        case Storage::String:
            return arg ? "const StringPtr *" : "StringPtr *";
        case Storage::Pointer:
            return "void *";
        case Storage::Unknown:
            return arg ? "ValueData *" : "ValueData";
    }

    return "void";
}

std::string CppCodeBuilder::numberLiteral(double value)
{
    if (std::isnan(value))
        return "std::numeric_limits<double>::quiet_NaN()";
    else if (std::isinf(value))
        return value > 0 ? "std::numeric_limits<double>::infinity()" : "-std::numeric_limits<double>::infinity()";

    std::stringstream ss;
    ss << std::setprecision(17) << value;
    std::string ret = ss.str();

    if (ret.find_first_of(".e") == std::string::npos)
        ret += ".0";

    return "(" + ret + ")";
}

std::string CppCodeBuilder::stringLiteral(const std::string &str)
{
    std::stringstream ss;
    ss << '"';

    for (unsigned char c : str) {
        if (c == '"' || c == '\\' || c == '?' || c < 0x20 || c >= 0x7f)
            ss << '\\' << std::oct << std::setw(3) << std::setfill('0') << (int)c << std::dec;
        else
            ss << c;
    }

    ss << '"';
    return ss.str();
}

std::string CppCodeBuilder::addLocal(Storage storage, const std::string &prefix, const std::string &init)
{
    std::string name = prefix + std::to_string(m_localCount++);
    const std::string indentation(12, ' ');

    switch (storage) {
        case Storage::Number:
            m_decls << indentation << "double " << name << " = 0;\n";
            break;
        case Storage::Bool:
            m_decls << indentation << "bool " << name << " = false;\n";
            break;
        case Storage::String:
            if (init.empty())
                m_decls << indentation << "cpp::Str " << name << ";\n";
            else
                m_decls << indentation << "cpp::Str " << name << "{ " << init << " };\n";
            break;
        case Storage::Pointer:
            m_decls << indentation << "void *" << name << " = nullptr;\n";
            break;
        case Storage::Unknown:
            m_decls << indentation << "cpp::Val " << name << ";\n";
            break;
        case Storage::Void:
            // Procedure call (argument array or coroutine)
            if (init.empty())
                m_decls << indentation << "CppCoroutine " << name << ";\n";
            else
                m_decls << indentation << "cpp::Args<" << init << "> " << name << ";\n";
            break;
    }

    return name;
}

std::string CppCodeBuilder::cast(CompilerValue *value, Compiler::StaticType targetType)
{
    assert(value);
    CppValue *cppValue = dynamic_cast<CppValue *>(value);
    assert(cppValue);
    const std::string &expr = cppValue->expr;
    Storage src = storage(value->type());
    Storage dst = storage(targetType);

    if (src == dst)
        return expr;

    switch (dst) {
        case Storage::Number:
            return "cpp::num(" + expr + ")";

        case Storage::Bool:
            return "cpp::boolean(" + expr + ")";

        case Storage::String: {
            std::string tmp = addLocal(Storage::String, "t") + ".s";
            emit("cpp::str(" + tmp + ", " + expr + ");");
            return tmp;
        }

        case Storage::Unknown: {
            std::string tmp = addLocal(Storage::Unknown, "t") + ".v";

            if (src == Storage::Pointer)
                emit("value_assign_pointer(&" + tmp + ", " + expr + ");");
            else
                emit("cpp::val(&" + tmp + ", " + expr + ");");

            return tmp;
        }

        default:
            return expr;
    }
}

void CppCodeBuilder::assign(CppValue *dst, CompilerValue *src)
{
    std::string v = cast(src, dst->type());

    switch (storage(dst->type())) {
        case Storage::String:
            emit("cpp::str(" + dst->expr + ", " + v + ");");
            break;

        case Storage::Unknown:
            emit("cpp::val(&" + dst->expr + ", " + v + ");");
            break;

        default:
            emit(dst->expr + " = " + v + ";");
            break;
    }
}

void CppCodeBuilder::emit(const std::string &line)
{
    m_body << std::string(m_indent * 4, ' ') << line << "\n";
}

void CppCodeBuilder::indent()
{
    m_indent++;
}

void CppCodeBuilder::unindent()
{
    assert(m_indent > 3);
    m_indent--;
}

void CppCodeBuilder::emitYield(bool checkWarp)
{
    if (isWarp())
        return;

    addInstruction(CppInstruction::Type::Yield);
    m_suspends = true;

    if (checkWarp && m_procedurePrototype)
        emit("if (!warp)");
    else
        emit("if (true)");

    emit("    co_await std::suspend_always{};");
}

void CppCodeBuilder::emitReturn(bool endThread)
{
    switch (m_codeType) {
        case Compiler::CodeType::Script:
            emit(RETURN_PLACEHOLDER + (endThread ? "true" : "false") + ");");
            break;

        case Compiler::CodeType::Reporter:
            emit("return ValueData{ { 0.0 }, ValueType::Number, 0 };");
            break;

        case Compiler::CodeType::HatPredicate:
            emit("return false;");
            break;
    }
}

bool CppCodeBuilder::isWarp() const
{
    // Reporters and hat predicates can't be suspended
    if (m_codeType != Compiler::CodeType::Script)
        return true;

    return m_procedurePrototype && m_procedurePrototype->warp();
}

/*! Returns a pointer from the pointer table of the generated code (keeps the generated source independent of memory addresses). */
std::string CppCodeBuilder::pointer(void *ptr)
{
    auto it = m_pointerIndexes.find(ptr);
    size_t index;

    if (it == m_pointerIndexes.cend()) {
        index = m_pointers.size();
        m_pointers.push_back(ptr);
        m_pointerIndexes[ptr] = index;
    } else
        index = it->second;

    return "pointers()[" + std::to_string(index) + "]";
}

const std::string &CppCodeBuilder::variablePtr(Variable *variable)
{
    auto it = m_variables.find(variable);

    if (it != m_variables.cend())
        return it->second;

    Target *target = m_ctx->target();
    ValueData *data = &variable->valuePtr()->data();

    // Variables of the target are read from the target's variable array, so that clones use their own variables
    if (target && variable->target() == target) {
        ValueData **variableData = target->variableData();
        const size_t count = target->variables().size();

        for (size_t i = 0; i < count; i++) {
            if (variableData[i] == data) {
                std::string name = addLocal(Storage::Pointer, "v");
                m_resolve << "                " << name << " = targetVariables[" << i << "];\n";
                return m_variables[variable] = "static_cast<ValueData *>(" + name + ")";
            }
        }
    }

    return m_variables[variable] = "static_cast<ValueData *>(" + pointer(data) + ")";
}

const std::string &CppCodeBuilder::listPtr(List *list)
{
    auto it = m_lists.find(list);

    if (it != m_lists.cend())
        return it->second;

    Target *target = m_ctx->target();

    // Lists of the target are read from the target's list array, so that clones use their own lists
    if (target && list->target() == target) {
        List **listData = target->listData();
        const size_t count = target->lists().size();

        for (size_t i = 0; i < count; i++) {
            if (listData[i] == list) {
                std::string name = addLocal(Storage::Pointer, "l");
                m_resolve << "                " << name << " = targetLists[" << i << "];\n";
                return m_lists[list] = "static_cast<List *>(" + name + ")";
            }
        }
    }

    return m_lists[list] = "static_cast<List *>(" + pointer(list) + ")";
}

std::string CppCodeBuilder::generateSource(const std::string &className) const
{
    std::string signature;
    bool coroutine = m_codeType == Compiler::CodeType::Script && m_suspends;
    const std::string params = "ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists";

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            if (m_procedurePrototype)
                signature = "CppCoroutine runProcedure(" + params + ", bool warp, ValueData *args) override";
            else
                signature = "CppCoroutine runScript(" + params + ") override";
            break;

        case Compiler::CodeType::Reporter:
            signature = "ValueData runReporter(" + params + ") override";
            break;

        case Compiler::CodeType::HatPredicate:
            signature = "bool runPredicate(" + params + ") override";
            break;
    }

    std::string body = m_body.str();
    const std::string returnStr = coroutine ? "co_return (" : "return CppCoroutine(";
    size_t pos = 0;

    while ((pos = body.find(RETURN_PLACEHOLDER, pos)) != std::string::npos) {
        body.replace(pos, RETURN_PLACEHOLDER.size(), returnStr);
        pos += returnStr.size();
    }

    std::stringstream ss;
    ss << "class " << className << " : public CppExecutableCode\n";
    ss << "{\n";
    ss << "    public:\n";
    ss << "        using CppExecutableCode::CppExecutableCode;\n";
    ss << "        using CppExecutableCode::runReporter;\n";
    ss << "        using CppExecutableCode::runPredicate;\n\n";
    ss << "        " << signature << "\n";
    ss << "        {\n";
    ss << m_decls.str();
    ss << "            auto resolve = [&]() {\n";
    ss << m_resolve.str();
    ss << "            };\n";
    ss << "            resolve();\n";
    ss << body;
    ss << "        }\n";

    // Procedures can be also run directly as scripts
    if (m_codeType == Compiler::CodeType::Script && m_procedurePrototype) {
        ss << "\n        CppCoroutine runScript(" << params << ") override\n";
        ss << "        {\n";
        ss << "            return runProcedure(ctx, target, targetVariables, targetLists, " << (m_procedurePrototype->warp() ? "true" : "false") << ", nullptr);\n";
        ss << "        }\n";
    }

    ss << "};\n";
    return ss.str();
}
