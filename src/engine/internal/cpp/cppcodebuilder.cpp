// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerlocalvariable.h>
#include <scratchcpp/blockprototype.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/target.h>
#include <scratchcpp/value.h>
#include <iomanip>
#include <cmath>
#include <cassert>

#include "cppcodebuilder.h"
#include "cppcompilercontext.h"
#include "cppexecutablecode.h"
#include "cppregister.h"

using namespace libscratchcpp;

static const std::string FRAME_ARGS = "ExecutionContext *ctx, Target *target, ValueData **varData, List **listData";
static const std::string FRAME_ARG_NAMES = "ctx, target, varData, listData";

CppCodeBuilder::CppCodeBuilder(CppCompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_procedurePrototype(procedurePrototype),
    m_codeType(codeType)
{
    assert(m_ctx);
    m_functionName = m_ctx->createFunctionName(m_procedurePrototype);
}

CppCodeBuilder::~CppCodeBuilder()
{
}

std::shared_ptr<ExecutableCode> CppCodeBuilder::build()
{
    assert(m_loops.empty());
    const std::string frame = m_functionName + "_frame";
    std::stringstream ss;

    // Procedure arguments
    std::stringstream argMembers, argParams, argNames, argInit, argTypes;

    if (m_procedurePrototype) {
        const auto &types = m_procedurePrototype->argumentTypes();

        for (size_t i = 0; i < types.size(); i++) {
            std::string name = "a" + std::to_string(i);
            std::string param = "p" + std::to_string(i);

            if (types[i] == BlockPrototype::ArgType::Bool) {
                argMembers << "        bool " << name << " = false;\n";
                argParams << ", bool " << param;
                argTypes << ", bool";
                argInit << "            " << name << " = " << param << ";\n";
            } else {
                argMembers << "        cpp::Val " << name << ";\n";
                argParams << ", const ValueData *" << param;
                argTypes << ", const ValueData *";
                argInit << "            cpp::val(&" << name << ".v, *" << param << ");\n";
            }

            argNames << ", " << param;
        }
    }

    // Frame
    ss << "// " << (m_procedurePrototype ? "procedure " + m_procedurePrototype->procCode() : m_functionName) << "\n";
    ss << "struct " << frame << " final : public cpp::Frame\n{\n";
    ss << "        " << frame << "(" << FRAME_ARGS << ", bool warp" << argParams.str() << ") :\n";
    ss << "            cpp::Frame(ctx),\n            target(target),\n            varData(varData),\n            listData(listData),\n            warp(warp)\n        {\n";
    ss << argInit.str();
    ss << "            resolve();\n        }\n\n";
    ss << "        ~" << frame << "()\n        {\n" << m_destroy.str() << "        }\n\n";
    ss << "        void resolve()\n        {\n" << m_resolve.str() << "        }\n\n";
    ss << "        int step() override\n        {\n";
    ss << "            switch (state) {\n                case 0:;\n" << m_body.str() << "            }\n\n";

    if (m_lastValue) {
        if (m_codeType == Compiler::CodeType::Reporter)
            ss << "            cpp::val(&ret.v, " << dynamic_cast<CppRegister *>(m_lastValue)->expr << ");\n";
        else if (m_codeType == Compiler::CodeType::HatPredicate)
            ss << "            ret = " << cast(m_lastValue, Compiler::StaticType::Bool) << ";\n";
    }

    ss << "            return cpp::CPP_DONE;\n        }\n\n";
    ss << "        Target *target;\n        ValueData **varData;\n        List **listData;\n        bool warp;\n";

    if (m_codeType == Compiler::CodeType::Reporter)
        ss << "        cpp::Val ret;\n";
    else if (m_codeType == Compiler::CodeType::HatPredicate)
        ss << "        bool ret = false;\n";

    ss << argMembers.str() << m_members.str() << "};\n\n";

    // Entry function
    switch (m_codeType) {
        case Compiler::CodeType::Script:
            if (m_procedurePrototype) {
                std::string warp = m_procedurePrototype->warp() ? "true" : "warp";
                m_ctx->declareProcedure(m_functionName, "static void *" + m_functionName + "(" + FRAME_ARGS + ", bool warp" + argTypes.str() + ");");
                ss << "static void *" << m_functionName << "(" << FRAME_ARGS << ", bool warp" << argParams.str() << ")\n{\n";
                ss << "    return cpp::start(new " << frame << "(" << FRAME_ARG_NAMES << ", " << warp << argNames.str() << "));\n}\n";
            } else {
                ss << "static void *" << m_functionName << "(" << FRAME_ARGS << ")\n{\n";
                ss << "    return cpp::start(new " << frame << "(" << FRAME_ARG_NAMES << ", false));\n}\n";
            }
            break;

        case Compiler::CodeType::Reporter:
            ss << "static ValueData " << m_functionName << "(" << FRAME_ARGS << ")\n{\n";
            ss << "    " << frame << " f(" << FRAME_ARG_NAMES << ", true);\n    f.step();\n";
            ss << "    ValueData ret;\n    value_init(&ret);\n    value_assign_copy(&ret, &f.ret.v);\n    return ret;\n}\n";
            break;

        case Compiler::CodeType::HatPredicate:
            ss << "static bool " << m_functionName << "(" << FRAME_ARGS << ")\n{\n";
            ss << "    " << frame << " f(" << FRAME_ARG_NAMES << ", true);\n    f.step();\n    return f.ret;\n}\n";
            break;
    }

    m_ctx->addFunction(m_functionName, ss.str(), m_functionName, !m_procedurePrototype);
    return std::make_shared<CppExecutableCode>(m_ctx, m_functionName, m_codeType);
}

CompilerValue *CppCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createFunctionCall(functionName, returnType, argTypes, args);
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
    Compiler::StaticType type;
    std::string expr;

    switch (value.type()) {
        case ValueType::Number:
            type = Compiler::StaticType::Number;
            expr = numberLiteral(value.data().numberValue);
            break;

        case ValueType::Bool:
            type = Compiler::StaticType::Bool;
            expr = value.toBool() ? "true" : "false";
            break;

        case ValueType::String:
            type = Compiler::StaticType::String;
            expr = addMember(Storage::String, "c", stringLiteral(value.toString())) + ".s";
            break;

        case ValueType::Pointer: {
            // NOTE: Pointers are only valid in the current process
            type = Compiler::StaticType::Pointer;
            std::stringstream ss;
            ss << "reinterpret_cast<void *>(0x" << std::hex << reinterpret_cast<uintptr_t>(value.toPointer()) << ")";
            expr = ss.str();
            break;
        }

        default:
            assert(false);
            type = Compiler::StaticType::Number;
            expr = "0.0";
            break;
    }

    auto reg = std::make_unique<CppConstantRegister>(type, value, expr);
    CppConstantRegister *ret = reg.get();
    m_values.push_back(std::move(reg));
    m_lastValue = ret;
    return ret;
}

CompilerValue *CppCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    std::string str = cast(string, Compiler::StaticType::String);
    std::string i = cast(index, Compiler::StaticType::Number);
    CppRegister *reg = addRegister(Compiler::StaticType::String);
    emit("cpp::stringChar(" + reg->expr + ", " + str + ", " + i + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addStringLength(CompilerValue *string)
{
    return assignNumber("(double)" + cast(string, Compiler::StaticType::String) + "->size");
}

CompilerValue *CppCodeBuilder::addLoopIndex()
{
    for (auto it = m_loops.rbegin(); it != m_loops.rend(); it++) {
        if (it->isRepeatLoop)
            return assignNumber(it->index);
    }

    return addConstValue(0);
}

CompilerValue *CppCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    CppRegister *reg = addRegister(variable->type());
    assign(reg, variable->ptr());
    return reg;
}

CompilerValue *CppCodeBuilder::addVariableValue(Variable *variable)
{
    CppRegister *reg = addRegister(Compiler::StaticType::Unknown);
    emit("value_assign_copy(&" + reg->expr + ", " + variablePtr(variable) + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListContents(List *list)
{
    CppRegister *reg = addRegister(Compiler::StaticType::String);
    emit(listPtr(list) + "->toStringPtr(" + reg->expr + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    CppRegister *reg = addRegister(Compiler::StaticType::Unknown);
    emit("cpp::listItem(&" + reg->expr + ", " + listPtr(list) + ", " + i + ");");
    return reg;
}

CompilerValue *CppCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    return assignNumber("cpp::listIndexOf(" + listPtr(list) + ", " + v + ")");
}

CompilerValue *CppCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    return assignBool(listPtr(list) + "->contains(" + v + ")");
}

CompilerValue *CppCodeBuilder::addListSize(List *list)
{
    return assignNumber("(double)" + listPtr(list) + "->size()");
}

CompilerValue *CppCodeBuilder::addProcedureArgument(const std::string &name)
{
    if (!m_procedurePrototype)
        return addConstValue(Value());

    const auto &names = m_procedurePrototype->argumentNames();
    const auto &types = m_procedurePrototype->argumentTypes();
    auto it = std::find(names.begin(), names.end(), name);

    if (it == names.end())
        return addConstValue(Value());

    size_t index = it - names.begin();
    std::string member = "a" + std::to_string(index);

    if (index < types.size() && types[index] == BlockPrototype::ArgType::Bool)
        return addRegister(Compiler::StaticType::Bool, member);
    else
        return addRegister(Compiler::StaticType::Unknown, member + ".v");
}

CompilerValue *CppCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(cast(operand1, Compiler::StaticType::Number) + " + " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(cast(operand1, Compiler::StaticType::Number) + " - " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(cast(operand1, Compiler::StaticType::Number) + " * " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignNumber(cast(operand1, Compiler::StaticType::Number) + " / " + cast(operand2, Compiler::StaticType::Number));
}

CompilerValue *CppCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    // Strings can contain decimal numbers which are integers ("1.0"), so pass them as values
    bool numbers = (storage(from->type()) != Storage::Unknown && storage(from->type()) != Storage::String) && (storage(to->type()) != Storage::Unknown && storage(to->type()) != Storage::String);
    Compiler::StaticType type = numbers ? Compiler::StaticType::Number : Compiler::StaticType::Unknown;
    return assignNumber("cpp::random(ctx, " + cast(from, type) + ", " + cast(to, type) + ")");
}

CompilerValue *CppCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return assignNumber("(double)llvm_random_int64(ctx, (int64_t)" + cast(from, Compiler::StaticType::Number) + ", (int64_t)" + cast(to, Compiler::StaticType::Number) + ")");
}

CompilerValue *CppCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool("value_equals(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool("value_greater(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool("value_lower(&" + cast(operand1, Compiler::StaticType::Unknown) + ", &" + cast(operand2, Compiler::StaticType::Unknown) + ")");
}

CompilerValue *CppCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    std::string func = caseSensitive ? "string_compare_case_sensitive" : "string_compare_case_insensitive";
    return assignBool(func + "(" + cast(string1, Compiler::StaticType::String) + ", " + cast(string2, Compiler::StaticType::String) + ") == 0");
}

CompilerValue *CppCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(cast(operand1, Compiler::StaticType::Bool) + " && " + cast(operand2, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return assignBool(cast(operand1, Compiler::StaticType::Bool) + " || " + cast(operand2, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createNot(CompilerValue *operand)
{
    return assignBool("!" + cast(operand, Compiler::StaticType::Bool));
}

CompilerValue *CppCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return assignNumber("cpp::mod(" + cast(num1, Compiler::StaticType::Number) + ", " + cast(num2, Compiler::StaticType::Number) + ")");
}

CompilerValue *CppCodeBuilder::createRound(CompilerValue *num)
{
    return mathFunction(num, "cpp::round");
}

CompilerValue *CppCodeBuilder::createAbs(CompilerValue *num)
{
    return mathFunction(num, "std::fabs");
}

CompilerValue *CppCodeBuilder::createFloor(CompilerValue *num)
{
    return mathFunction(num, "std::floor");
}

CompilerValue *CppCodeBuilder::createCeil(CompilerValue *num)
{
    return mathFunction(num, "std::ceil");
}

CompilerValue *CppCodeBuilder::createSqrt(CompilerValue *num)
{
    // + 0.0 to avoid negative zero
    return mathFunction(num, "std::sqrt", " + 0.0");
}

CompilerValue *CppCodeBuilder::createSin(CompilerValue *num)
{
    return mathFunction(num, "cpp::sin");
}

CompilerValue *CppCodeBuilder::createCos(CompilerValue *num)
{
    return mathFunction(num, "cpp::cos");
}

CompilerValue *CppCodeBuilder::createTan(CompilerValue *num)
{
    return mathFunction(num, "cpp::tan");
}

CompilerValue *CppCodeBuilder::createAsin(CompilerValue *num)
{
    return mathFunction(num, "std::asin", " * 180.0 / M_PI + 0.0");
}

CompilerValue *CppCodeBuilder::createAcos(CompilerValue *num)
{
    return mathFunction(num, "std::acos", " * 180.0 / M_PI");
}

CompilerValue *CppCodeBuilder::createAtan(CompilerValue *num)
{
    return mathFunction(num, "std::atan", " * 180.0 / M_PI");
}

CompilerValue *CppCodeBuilder::createLn(CompilerValue *num)
{
    return mathFunction(num, "std::log");
}

CompilerValue *CppCodeBuilder::createLog10(CompilerValue *num)
{
    return mathFunction(num, "std::log10");
}

CompilerValue *CppCodeBuilder::createExp(CompilerValue *num)
{
    return mathFunction(num, "std::exp");
}

CompilerValue *CppCodeBuilder::createExp10(CompilerValue *num)
{
    return assignNumber("std::pow(10.0, " + cast(num, Compiler::StaticType::Number) + ")");
}

CompilerValue *CppCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    std::string str1 = cast(string1, Compiler::StaticType::String);
    std::string str2 = cast(string2, Compiler::StaticType::String);
    CppRegister *reg = addRegister(Compiler::StaticType::String);
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
    CppRegister *reg = addRegister(castType);
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
    CppRegister *reg = addRegister(type);
    auto var = std::make_unique<CompilerLocalVariable>(reg);
    CompilerLocalVariable *ret = var.get();
    m_localVariables.push_back(std::move(var));
    return ret;
}

void CppCodeBuilder::createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value)
{
    assign(dynamic_cast<CppRegister *>(variable->ptr()), value);
}

void CppCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
    std::string v = dynamic_cast<CppRegister *>(value)->expr;

    if (storage(value->type()) == Storage::Pointer)
        emit("value_assign_pointer(" + variablePtr(variable) + ", " + v + ");");
    else
        emit("cpp::val(" + variablePtr(variable) + ", " + v + ");");
}

void CppCodeBuilder::createListClear(List *list)
{
    emit(listPtr(list) + "->clear();");
}

void CppCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + "))");
    emit("    " + l + "->removeAt((size_t)" + i + ");");
}

void CppCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    std::string v = cast(item, Compiler::StaticType::Unknown);
    emit(listPtr(list) + "->append(" + v + ");");
}

void CppCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    std::string v = cast(item, Compiler::StaticType::Unknown);
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + ", true))");
    emit("    " + l + "->insert((size_t)" + i + ", " + v + ");");
}

void CppCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    std::string i = cast(index, Compiler::StaticType::Number);
    std::string v = cast(item, Compiler::StaticType::Unknown);
    const std::string &l = listPtr(list);
    emit("if (cpp::listIndexInRange(" + l + ", " + i + "))");
    emit("    " + l + "->replace((size_t)" + i + ", " + v + ");");
}

void CppCodeBuilder::beginIfStatement(CompilerValue *cond)
{
    emit("if (" + cast(cond, Compiler::StaticType::Bool) + ") {");
    indent();
}

void CppCodeBuilder::beginElseBranch()
{
    unindent();
    emit("} else {");
    indent();
}

void CppCodeBuilder::endIf()
{
    unindent();
    emit("}");
}

void CppCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
    assert(!m_loopCondition);
    std::string c = cast(count, Compiler::StaticType::Number);
    std::string countMember = addMember(Storage::Number, "n");
    std::string index = addMember(Storage::Number, "i");

    // Scratch rounds the count, a negative count skips the loop and infinity repeats forever
    emit(countMember + " = " + c + ";");
    emit(countMember + " = " + countMember + " > 0 ? (std::isinf(" + countMember + ") ? " + countMember + " : std::round(" + countMember + ")) : 0;");
    emit("for (" + index + " = 0; std::isinf(" + countMember + ") || " + index + " < " + countMember + "; " + index + "++) {");
    indent();

    Loop loop;
    loop.isRepeatLoop = true;
    loop.index = index;
    m_loops.push_back(loop);
}

void CppCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
    if (!m_loopCondition) {
        emit("for (;;) {");
        indent();
    }

    emit("if (!" + cast(cond, Compiler::StaticType::Bool) + ")");
    emit("    break;");
    m_loopCondition = false;
    m_loops.push_back(Loop());
}

void CppCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
    if (!m_loopCondition) {
        emit("for (;;) {");
        indent();
    }

    emit("if (" + cast(cond, Compiler::StaticType::Bool) + ")");
    emit("    break;");
    m_loopCondition = false;
    m_loops.push_back(Loop());
}

void CppCodeBuilder::beginLoopCondition()
{
    // The condition is evaluated at the beginning of every iteration
    emit("for (;;) {");
    indent();
    m_loopCondition = true;
}

void CppCodeBuilder::endLoop()
{
    assert(!m_loops.empty());

    if (!isWarp())
        createSuspend();

    unindent();
    emit("}");
    m_loops.pop_back();
}

void CppCodeBuilder::yield()
{
    if (!isWarp())
        createSuspend();
}

void CppCodeBuilder::createStop()
{
    emit("return cpp::CPP_DONE;");
}

void CppCodeBuilder::createThreadStop()
{
    if (m_codeType == Compiler::CodeType::Script)
        emit("return cpp::CPP_END_THREAD;");
    else
        emit("return cpp::CPP_DONE;");
}

void CppCodeBuilder::invalidateTarget()
{
    emit("resolve();");
}

void CppCodeBuilder::createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args)
{
    assert(prototype);
    const auto &types = prototype->argumentTypes();
    assert(args.size() == types.size());
    std::string name = CppCompilerContext::procedureFunctionName(prototype);
    std::string call = name + "(" + FRAME_ARG_NAMES + ", " + (m_procedurePrototype ? "warp" : (isWarp() ? "true" : "false"));

    for (size_t i = 0; i < args.size(); i++) {
        if (types[i] == BlockPrototype::ArgType::Bool)
            call += ", " + cast(args[i], Compiler::StaticType::Bool);
        else
            call += ", &" + cast(args[i], Compiler::StaticType::Unknown);
    }

    call += ")";

    // The procedure handle is stored in the frame because the call can be suspended
    std::string handle = addMember(Storage::Pointer, "h");
    m_destroy << "            if (" << handle << ")\n                cpp::destroy(" << handle << ");\n";

    emit(handle + " = " + call + ";");
    emit("if (" + handle + " == cpp::END_THREAD_SENTINEL) {");
    emit("    " + handle + " = nullptr;");
    emit(m_codeType == Compiler::CodeType::Script ? "    return cpp::CPP_END_THREAD;" : "    return cpp::CPP_DONE;");
    emit("}");

    if (!isWarp() && !prototype->warp()) {
        emit("while (" + handle + ") {");
        indent();
        createSuspend(false);
        emit("if (cpp::resume(" + handle + "))");
        emit("    " + handle + " = nullptr;");
        unindent();
        emit("}");

        // The thread could be stopped from the procedure
        emit("if (cpp_is_thread_finished(ctx))");
        emit("    return cpp::CPP_END_THREAD;");
    } else
        emit(handle + " = nullptr;");

    emit("resolve();");
}

void CppCodeBuilder::createPrint(CompilerValue *string)
{
    emit("llvm_print(" + cast(string, Compiler::StaticType::String) + ");");
}

/*! Returns the name of the generated function. */
const std::string &CppCodeBuilder::functionName() const
{
    return m_functionName;
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

std::string CppCodeBuilder::addMember(Storage storage, const std::string &prefix, const std::string &init)
{
    std::string name = prefix + std::to_string(m_memberCount++);

    switch (storage) {
        case Storage::Number:
            m_members << "        double " << name << " = 0;\n";
            break;
        case Storage::Bool:
            m_members << "        bool " << name << " = false;\n";
            break;
        case Storage::String:
            if (init.empty())
                m_members << "        cpp::Str " << name << ";\n";
            else
                m_members << "        cpp::Str " << name << "{ " << init << " };\n";
            break;
        case Storage::Pointer:
            m_members << "        void *" << name << " = nullptr;\n";
            break;
        case Storage::Unknown:
            m_members << "        cpp::Val " << name << ";\n";
            break;
        case Storage::Void:
            assert(false);
            break;
    }

    return name;
}

CppRegister *CppCodeBuilder::addRegister(Compiler::StaticType type)
{
    Storage s = storage(type);
    std::string name = addMember(s, "r");

    if (s == Storage::String)
        name += ".s";
    else if (s == Storage::Unknown)
        name += ".v";

    return addRegister(type, name);
}

CppRegister *CppCodeBuilder::addRegister(Compiler::StaticType type, const std::string &expr)
{
    auto reg = std::make_unique<CppRegister>(type, expr);
    CppRegister *ret = reg.get();
    m_values.push_back(std::move(reg));
    m_lastValue = ret;
    return ret;
}

CppRegister *CppCodeBuilder::assignNumber(const std::string &expr)
{
    CppRegister *reg = addRegister(Compiler::StaticType::Number);
    emit(reg->expr + " = " + expr + ";");
    return reg;
}

CppRegister *CppCodeBuilder::assignBool(const std::string &expr)
{
    CppRegister *reg = addRegister(Compiler::StaticType::Bool);
    emit(reg->expr + " = " + expr + ";");
    return reg;
}

CppRegister *CppCodeBuilder::mathFunction(CompilerValue *num, const std::string &func, const std::string &suffix)
{
    return assignNumber(func + "(" + cast(num, Compiler::StaticType::Number) + ")" + suffix);
}

std::string CppCodeBuilder::cast(CompilerValue *value, Compiler::StaticType targetType)
{
    assert(value);
    const std::string &expr = dynamic_cast<CppRegister *>(value)->expr;
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
            std::string tmp = addMember(Storage::String, "t") + ".s";
            emit("cpp::str(" + tmp + ", " + expr + ");");
            return tmp;
        }

        case Storage::Unknown: {
            std::string tmp = addMember(Storage::Unknown, "t") + ".v";

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

void CppCodeBuilder::assign(CppRegister *dst, CompilerValue *src)
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

void CppCodeBuilder::createSuspend(bool checkWarp)
{
    int state = ++m_stateCount;
    std::string ret = "state = " + std::to_string(state) + "; return cpp::CPP_SUSPEND; case " + std::to_string(state) + ":;";

    if (checkWarp && m_procedurePrototype)
        emit("if (!warp) { " + ret + " }");
    else
        emit(ret);
}

bool CppCodeBuilder::isWarp() const
{
    // Reporters and hat predicates can't be suspended
    if (m_codeType != Compiler::CodeType::Script)
        return true;

    return m_procedurePrototype && m_procedurePrototype->warp();
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
    std::string decl = "extern \"C\" " + cppType(returnType, false) + " " + functionName + "(";
    std::string call = functionName + "(";
    bool first = true;

    if (!firstArgType.empty()) {
        decl += firstArgType;
        call += firstArg;
        first = false;
    }

    for (size_t i = 0; i < args.size(); i++) {
        std::string v = cast(args[i], argTypes[i]);

        if (!first) {
            decl += ", ";
            call += ", ";
        }

        decl += cppType(argTypes[i], true);
        call += storage(argTypes[i]) == Storage::Unknown ? "&" + v : v;
        first = false;
    }

    decl += ");";
    call += ")";
    m_ctx->declareExternFunction(functionName, decl);

    switch (storage(returnType)) {
        case Storage::Void:
            emit(call + ";");
            return nullptr;

        case Storage::String: {
            // The returned string is owned by the caller
            CppRegister *reg = addRegister(returnType);
            std::string member = reg->expr.substr(0, reg->expr.size() - 2);
            emit(member + ".reset(" + call + ");");
            return reg;
        }

        case Storage::Unknown: {
            CppRegister *reg = addRegister(returnType);
            std::string member = reg->expr.substr(0, reg->expr.size() - 2);
            emit(member + ".reset(" + call + ");");
            return reg;
        }

        default: {
            CppRegister *reg = addRegister(returnType);
            emit(reg->expr + " = " + call + ";");
            return reg;
        }
    }
}

const std::string &CppCodeBuilder::variablePtr(Variable *variable)
{
    auto it = m_variables.find(variable);

    if (it != m_variables.cend())
        return it->second;

    std::string name = "v" + std::to_string(m_memberCount++);
    Target *target = m_ctx->target();
    Target *owner = variable->target();
    assert(owner);
    int index = owner ? owner->findVariableById(variable->id()) : -1;
    assert(index != -1);
    m_members << "        ValueData *" << name << " = nullptr;\n";

    if (!target || target->isStage() || owner == target)
        m_resolve << "            " << name << " = varData[" << index << "];\n";
    else
        m_resolve << "            " << name << " = cpp::stage(ctx)->variableData()[" << index << "];\n";

    return m_variables[variable] = name;
}

const std::string &CppCodeBuilder::listPtr(List *list)
{
    auto it = m_lists.find(list);

    if (it != m_lists.cend())
        return it->second;

    std::string name = "l" + std::to_string(m_memberCount++);
    Target *target = m_ctx->target();
    Target *owner = list->target();
    assert(owner);
    int index = owner ? owner->findListById(list->id()) : -1;
    assert(index != -1);
    m_members << "        List *" << name << " = nullptr;\n";

    if (!target || target->isStage() || owner == target)
        m_resolve << "            " << name << " = listData[" << index << "];\n";
    else
        m_resolve << "            " << name << " = cpp::stage(ctx)->listData()[" << index << "];\n";

    return m_lists[list] = name;
}
