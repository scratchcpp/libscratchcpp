// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/blockprototype.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/value.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/compilerlocalvariable.h>
#include <engine/internal/cpp/cppcodebuilder.h>
#include <engine/internal/cpp/cppcompilercontext.h>

#include "cases.h"

using namespace libscratchcpp;

using Type = Compiler::StaticType;
using CodeType = Compiler::CodeType;

CppTestEnv::CppTestEnv() :
    ctx(std::make_shared<CppCompilerContext>(nullptr, &sprite))
{
}

CppTestEnv::~CppTestEnv()
{
}

CppCodeBuilder *CppTestEnv::createBuilder(Compiler::CodeType codeType, BlockPrototype *procedurePrototype)
{
    builders.push_back(std::make_shared<CppCodeBuilder>(ctx.get(), procedurePrototype, codeType));
    return builders.back().get();
}

BlockPrototype *CppTestEnv::createProcedure(const std::string &procCode, const std::vector<std::string> &argNames, bool warp)
{
    auto proc = std::make_shared<BlockPrototype>(procCode);
    proc->setArgumentNames(argNames);
    proc->setWarp(warp);
    procedures.push_back(proc);
    return proc.get();
}

Variable *CppTestEnv::createVariable(const std::string &name, const Value &value)
{
    auto var = std::make_shared<Variable>(name, name, value);
    sprite.addVariable(var);
    return var.get();
}

List *CppTestEnv::createList(const std::string &name, const std::vector<Value> &items)
{
    auto list = std::make_shared<List>(name, name);

    for (const Value &item : items)
        list->append(item);

    sprite.addList(list);
    return list.get();
}

static void print(CppCodeBuilder *b, CompilerValue *v)
{
    b->createPrint(v);
    b->createPrint(b->addConstValue("\n"));
}

static void printConst(CppCodeBuilder *b, const Value &v)
{
    print(b, b->addConstValue(v));
}

// clang-format off
static const std::vector<CppTestCase> CASES = {
    { "constants", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        printConst(b, 5.25);
        printConst(b, -12);
        printConst(b, true);
        printConst(b, "Hello \"world\" \\ ?");
        printConst(b, "Unicode: ěščř 😀");
        printConst(b, std::numeric_limits<double>::infinity());
        printConst(b, -std::numeric_limits<double>::infinity());
        printConst(b, std::numeric_limits<double>::quiet_NaN());
        return b->build();
    }, "5.25\n-12\ntrue\nHello \"world\" \\ ?\nUnicode: ěščř 😀\nInfinity\n-Infinity\nNaN\n" },

    { "function_calls", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        b->addFunctionCall("cpp_test_empty", Type::Void, {}, {});
        auto v = b->addFunctionCall("cpp_test_add", Type::Number, { Type::Number, Type::Number }, { b->addConstValue(2), b->addConstValue("3.5") });
        print(b, v);
        auto s = b->addFunctionCall("cpp_test_string", Type::String, { Type::String }, { v });
        print(b, s);
        auto u = b->addFunctionCall("cpp_test_value", Type::Unknown, { Type::Unknown, Type::Bool }, { s, b->addConstValue(1) });
        print(b, u);
        b->addTargetFunctionCall("cpp_test_target", Type::Void, {}, {});
        auto r = b->addFunctionCallWithCtx("cpp_test_ctx", Type::Bool, {}, {});
        print(b, r);
        return b->build();
    }, "empty\n5.5\nstring: 5.5\nvalue: string: 5.5 true\ntarget: 1\ntrue\n" },

    { "strings", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        auto str = b->addConstValue("abc");
        print(b, b->createStringConcat(str, b->addConstValue(12.5)));
        print(b, b->addStringChar(str, b->addConstValue(1)));
        print(b, b->addStringChar(str, b->addConstValue(3)));
        print(b, b->addStringLength(b->addConstValue("Hello")));
        print(b, b->createStrCmpEQ(b->addConstValue("ABC"), str, false));
        print(b, b->createStrCmpEQ(b->addConstValue("ABC"), str, true));
        return b->build();
    }, "abc12.5\nb\n\n5\ntrue\nfalse\n" },

    { "arithmetic", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        auto a = b->addConstValue(7);
        auto c = b->addConstValue("-3");
        print(b, b->createAdd(a, c));
        print(b, b->createSub(a, c));
        print(b, b->createMul(a, c));
        print(b, b->createDiv(a, c));
        print(b, b->createDiv(b->addConstValue(1), b->addConstValue(0)));
        print(b, b->createMod(a, c));
        print(b, b->createMod(b->addConstValue(-7), b->addConstValue(3)));
        print(b, b->createAdd(b->addConstValue("abc"), b->addConstValue(true)));
        print(b, b->createRandom(b->addConstValue(4), b->addConstValue(4)));
        print(b, b->createRandomInt(b->addConstValue(2), b->addConstValue(2)));
        return b->build();
    }, "4\n10\n-21\n-2.333333333333333\nInfinity\n-2\n2\n1\n4\n2\n" },

    { "math_functions", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        print(b, b->createRound(b->addConstValue(2.5)));
        print(b, b->createRound(b->addConstValue(-2.5)));
        print(b, b->createAbs(b->addConstValue(-4.5)));
        print(b, b->createFloor(b->addConstValue(-4.5)));
        print(b, b->createCeil(b->addConstValue(4.2)));
        print(b, b->createSqrt(b->addConstValue(16)));
        print(b, b->createSin(b->addConstValue(30)));
        print(b, b->createCos(b->addConstValue(60)));
        print(b, b->createTan(b->addConstValue(45)));
        print(b, b->createTan(b->addConstValue(90)));
        print(b, b->createAsin(b->addConstValue(1)));
        print(b, b->createAcos(b->addConstValue(0)));
        print(b, b->createAtan(b->addConstValue(1)));
        print(b, b->createLn(b->addConstValue(1)));
        print(b, b->createLog10(b->addConstValue(1000)));
        print(b, b->createExp(b->addConstValue(0)));
        print(b, b->createExp10(b->addConstValue(2)));
        return b->build();
    }, "3\n-2\n4.5\n-5\n5\n4\n0.5\n0.5\n1\nInfinity\n90\n90\n45\n0\n3\n1\n100\n" },

    { "logic_and_comparison", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        auto t = b->addConstValue(true);
        auto f = b->addConstValue("false");
        print(b, b->createAnd(t, f));
        print(b, b->createOr(t, f));
        print(b, b->createNot(f));
        print(b, b->createCmpEQ(b->addConstValue("10"), b->addConstValue(10)));
        print(b, b->createCmpGT(b->addConstValue(10), b->addConstValue("9")));
        print(b, b->createCmpLT(b->addConstValue("abc"), b->addConstValue("abd")));
        print(b, b->createSelect(t, b->addConstValue("yes"), b->addConstValue(5), Type::Unknown));
        print(b, b->createSelect(f, b->addConstValue(1), b->addConstValue(2), Type::Number));
        return b->build();
    }, "false\ntrue\ntrue\ntrue\ntrue\ntrue\nyes\n2\n" },

    { "if_statement", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        b->beginIfStatement(b->addConstValue(true));
        printConst(b, "if 1");
        b->beginElseBranch();
        printConst(b, "else 1");
        b->endIf();
        b->beginIfStatement(b->addConstValue(0));
        printConst(b, "if 2");
        b->beginElseBranch();
        b->beginIfStatement(b->addConstValue("true"));
        printConst(b, "nested");
        b->endIf();
        b->endIf();
        return b->build();
    }, "if 1\nnested\n" },

    { "repeat_loop", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        b->beginRepeatLoop(b->addConstValue(2.6));
        print(b, b->addLoopIndex());
        b->endLoop();
        b->beginRepeatLoop(b->addConstValue(-1));
        printConst(b, "never");
        b->endLoop();
        return b->build();
    }, "0\n1\n2\n", 4 },

    { "while_loops", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        auto var = b->createLocalVariable(Type::Number);
        b->createLocalVariableWrite(var, b->addConstValue(0));
        b->beginLoopCondition();
        b->beginWhileLoop(b->createCmpLT(b->addLocalVariableValue(var), b->addConstValue(3)));
        b->createLocalVariableWrite(var, b->createAdd(b->addLocalVariableValue(var), b->addConstValue(1)));
        print(b, b->addLocalVariableValue(var));
        b->endLoop();
        b->beginLoopCondition();
        b->beginRepeatUntilLoop(b->createCmpEQ(b->addLocalVariableValue(var), b->addConstValue(0)));
        b->createLocalVariableWrite(var, b->createSub(b->addLocalVariableValue(var), b->addConstValue(1)));
        b->endLoop();
        print(b, b->addLocalVariableValue(var));
        return b->build();
    }, "1\n2\n3\n0\n", 7 },

    { "local_variables", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        auto s = b->createLocalVariable(Type::String);
        auto u = b->createLocalVariable(Type::Unknown);
        print(b, b->addLocalVariableValue(s));
        b->createLocalVariableWrite(s, b->addConstValue(12));
        b->createLocalVariableWrite(u, b->addConstValue(true));
        print(b, b->addLocalVariableValue(s));
        print(b, b->addLocalVariableValue(u));
        return b->build();
    }, "\n12\ntrue\n" },

    { "variables", [](CppTestEnv &env) {
        auto var1 = env.createVariable("a", 5);
        auto var2 = env.createVariable("b", "test");
        auto b = env.createBuilder();
        print(b, b->addVariableValue(var1));
        print(b, b->addVariableValue(var2));
        auto v = b->addVariableValue(var1);
        b->createVariableWrite(var1, b->createAdd(b->addVariableValue(var1), b->addConstValue(1)));
        print(b, v);
        print(b, b->addVariableValue(var1));
        b->createVariableWrite(var2, b->addConstValue(false));
        print(b, b->addVariableValue(var2));
        return b->build();
    }, "5\ntest\n5\n6\nfalse\n" },

    { "lists", [](CppTestEnv &env) {
        auto list = env.createList("l", { 1, "a", true });
        auto b = env.createBuilder();
        print(b, b->addListContents(list));
        print(b, b->addListSize(list));
        print(b, b->addListItem(list, b->addConstValue(1)));
        print(b, b->addListItem(list, b->addConstValue(5)));
        print(b, b->addListItemIndex(list, b->addConstValue(true)));
        print(b, b->addListItemIndex(list, b->addConstValue("b")));
        print(b, b->addListContains(list, b->addConstValue("A")));
        b->createListAppend(list, b->addConstValue("x"));
        b->createListInsert(list, b->addConstValue(0), b->addConstValue(0));
        b->createListInsert(list, b->addConstValue(10), b->addConstValue(10));
        b->createListReplace(list, b->addConstValue(2), b->addConstValue("y"));
        b->createListRemove(list, b->addConstValue(1));
        b->createListRemove(list, b->addConstValue(-1));
        print(b, b->addListContents(list));
        b->createListClear(list);
        print(b, b->addListSize(list));
        return b->build();
    }, "1 a true\n3\na\n\n0\n-1\ntrue\n0 y true x\n0\n" },

    { "yield", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        printConst(b, "1");
        b->yield();
        printConst(b, "2");
        b->yield();
        printConst(b, "3");
        return b->build();
    }, "1\n2\n3\n", 3 },

    { "stop", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        printConst(b, "before");
        b->beginIfStatement(b->addConstValue(true));
        b->createStop();
        b->endIf();
        printConst(b, "after");
        return b->build();
    }, "before\n", 1 },

    { "thread_stop", [](CppTestEnv &env) {
        auto b = env.createBuilder();
        b->yield();
        printConst(b, "before");
        b->createThreadStop();
        printConst(b, "after");
        return b->build();
    }, "before\n", 2 },

    { "reporter", [](CppTestEnv &env) {
        auto b = env.createBuilder(CodeType::Reporter);
        b->createAdd(b->addConstValue(2), b->addConstValue(3));
        return b->build();
    }, "", -1, "5", CodeType::Reporter },

    { "reporter_const", [](CppTestEnv &env) {
        auto b = env.createBuilder(CodeType::Reporter);
        b->addConstValue("hello");
        return b->build();
    }, "", -1, "hello", CodeType::Reporter },

    { "hat_predicate", [](CppTestEnv &env) {
        auto b = env.createBuilder(CodeType::HatPredicate);
        b->createCmpGT(b->addConstValue(5), b->addConstValue("4"));
        return b->build();
    }, "", -1, "true", CodeType::HatPredicate },

    { "procedures", [](CppTestEnv &env) {
        // Non-warp procedure with arguments and a yield
        auto proc1 = env.createProcedure("proc1 %s %b", { "text", "flag" }, false);
        auto p = env.createBuilder(CodeType::Script, proc1);
        print(p, p->addProcedureArgument("text"));
        p->yield();
        print(p, p->addProcedureArgument("flag"));
        print(p, p->addProcedureArgument("invalid"));
        p->build();

        // Warp procedure: yields are ignored, the non-warp procedure call isn't suspended
        auto proc2 = env.createProcedure("proc2", {}, true);
        p = env.createBuilder(CodeType::Script, proc2);
        p->createProcedureCall(proc1, { p->addConstValue("warp"), p->addConstValue(false) });
        p->yield();
        printConst(p, "proc2 end");
        p->build();

        // Procedure which stops the thread
        auto proc3 = env.createProcedure("proc3", {}, false);
        p = env.createBuilder(CodeType::Script, proc3);
        p->yield();
        p->createThreadStop();
        p->build();

        auto b = env.createBuilder();
        b->createProcedureCall(proc1, { b->addConstValue(12), b->addConstValue(true) });
        b->createProcedureCall(proc2, {});
        b->createProcedureCall(proc3, {});
        printConst(b, "never");
        return b->build();
    }, "12\ntrue\n0\nwarp\nfalse\n0\nproc2 end\n", 3 },
};
// clang-format on

const std::vector<CppTestCase> &libscratchcpp::cppTestCases()
{
    return CASES;
}
