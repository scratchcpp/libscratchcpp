// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace libscratchcpp
{

class CppCompilerContext;
class CppCodeBuilder;
class BlockPrototype;
class ExecutableCode;

// The environment of a test case, the same environment is used by the generator and by the test
struct CppTestEnv
{
        CppTestEnv();
        ~CppTestEnv();

        CppCodeBuilder *createBuilder(Compiler::CodeType codeType = Compiler::CodeType::Script, BlockPrototype *procedurePrototype = nullptr);
        BlockPrototype *createProcedure(const std::string &procCode, const std::vector<std::string> &argNames, bool warp);
        Variable *createVariable(const std::string &name, const Value &value);
        List *createList(const std::string &name, const std::vector<Value> &items);

        Sprite sprite;
        std::shared_ptr<CppCompilerContext> ctx;
        std::vector<std::shared_ptr<CppCodeBuilder>> builders;
        std::vector<std::shared_ptr<BlockPrototype>> procedures;
};

struct CppTestCase
{
        std::string name;
        std::function<std::shared_ptr<ExecutableCode>(CppTestEnv &)> build;
        std::string expectedOutput;  // standard output after running the code
        int expectedRuns = -1;       // the number of run() calls until the script finishes (-1 to skip)
        std::string expectedValue;   // the return value of reporters and hat predicates
        Compiler::CodeType codeType = Compiler::CodeType::Script;
};

const std::vector<CppTestCase> &cppTestCases();

} // namespace libscratchcpp
