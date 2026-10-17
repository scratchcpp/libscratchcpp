// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>

#include "../icodebuilder.h"
#include "../../../test_export.h"

namespace libscratchcpp
{

class CppCompilerContext;
struct CppRegister;

/*!
 * \brief The CppCodeBuilder class generates C++ code (one function) and adds it to a CppCompilerContext.
 *
 * Every value is stored in a function frame, so the function can be suspended and resumed later.
 */
class LIBSCRATCHCPP_TEST_EXPORT CppCodeBuilder : public ICodeBuilder
{
    public:
        CppCodeBuilder(CppCompilerContext *ctx, BlockPrototype *procedurePrototype = nullptr, Compiler::CodeType codeType = Compiler::CodeType::Script);
        ~CppCodeBuilder();

        std::shared_ptr<ExecutableCode> build() override;

        CompilerValue *addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) override;
        CompilerValue *addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) override;
        CompilerValue *addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) override;
        CompilerConstant *addConstValue(const Value &value) override;
        CompilerValue *addStringChar(CompilerValue *string, CompilerValue *index) override;
        CompilerValue *addStringLength(CompilerValue *string) override;
        CompilerValue *addLoopIndex() override;
        CompilerValue *addLocalVariableValue(CompilerLocalVariable *variable) override;
        CompilerValue *addVariableValue(Variable *variable) override;
        CompilerValue *addListContents(List *list) override;
        CompilerValue *addListItem(List *list, CompilerValue *index) override;
        CompilerValue *addListItemIndex(List *list, CompilerValue *item) override;
        CompilerValue *addListContains(List *list, CompilerValue *item) override;
        CompilerValue *addListSize(List *list) override;
        CompilerValue *addProcedureArgument(const std::string &name) override;

        CompilerValue *createAdd(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createSub(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createMul(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createDiv(CompilerValue *operand1, CompilerValue *operand2) override;

        CompilerValue *createRandom(CompilerValue *from, CompilerValue *to) override;
        CompilerValue *createRandomInt(CompilerValue *from, CompilerValue *to) override;

        CompilerValue *createCmpEQ(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createCmpGT(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createCmpLT(CompilerValue *operand1, CompilerValue *operand2) override;

        CompilerValue *createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive = false) override;

        CompilerValue *createAnd(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createOr(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createNot(CompilerValue *operand) override;

        CompilerValue *createMod(CompilerValue *num1, CompilerValue *num2) override;
        CompilerValue *createRound(CompilerValue *num) override;
        CompilerValue *createAbs(CompilerValue *num) override;
        CompilerValue *createFloor(CompilerValue *num) override;
        CompilerValue *createCeil(CompilerValue *num) override;
        CompilerValue *createSqrt(CompilerValue *num) override;
        CompilerValue *createSin(CompilerValue *num) override;
        CompilerValue *createCos(CompilerValue *num) override;
        CompilerValue *createTan(CompilerValue *num) override;
        CompilerValue *createAsin(CompilerValue *num) override;
        CompilerValue *createAcos(CompilerValue *num) override;
        CompilerValue *createAtan(CompilerValue *num) override;
        CompilerValue *createLn(CompilerValue *num) override;
        CompilerValue *createLog10(CompilerValue *num) override;
        CompilerValue *createExp(CompilerValue *num) override;
        CompilerValue *createExp10(CompilerValue *num) override;

        CompilerValue *createStringConcat(CompilerValue *string1, CompilerValue *string2) override;

        CompilerValue *createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType) override;

        CompilerLocalVariable *createLocalVariable(Compiler::StaticType type) override;
        void createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value) override;

        void createVariableWrite(Variable *variable, CompilerValue *value) override;

        void createListClear(List *list) override;
        void createListRemove(List *list, CompilerValue *index) override;
        void createListAppend(List *list, CompilerValue *item) override;
        void createListInsert(List *list, CompilerValue *index, CompilerValue *item) override;
        void createListReplace(List *list, CompilerValue *index, CompilerValue *item) override;

        void beginIfStatement(CompilerValue *cond) override;
        void beginElseBranch() override;
        void endIf() override;

        void beginRepeatLoop(CompilerValue *count) override;
        void beginWhileLoop(CompilerValue *cond) override;
        void beginRepeatUntilLoop(CompilerValue *cond) override;
        void beginLoopCondition() override;
        void endLoop() override;

        void yield() override;

        void createStop() override;
        void createThreadStop() override;

        void invalidateTarget() override;

        void createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args) override;

        void createPrint(CompilerValue *string) override;

        const std::string &functionName() const;

    private:
        enum class Storage
        {
            Void,
            Number,
            Bool,
            String,
            Pointer,
            Unknown
        };

        struct Loop
        {
                bool isRepeatLoop = false;
                std::string index;
        };

        static Storage storage(Compiler::StaticType type);
        static std::string cppType(Compiler::StaticType type, bool arg);
        static std::string numberLiteral(double value);
        static std::string stringLiteral(const std::string &str);

        std::string addMember(Storage storage, const std::string &prefix, const std::string &init = "");
        CppRegister *addRegister(Compiler::StaticType type);
        CppRegister *addRegister(Compiler::StaticType type, const std::string &expr);
        CppRegister *assignNumber(const std::string &expr);
        CppRegister *assignBool(const std::string &expr);
        CppRegister *mathFunction(CompilerValue *num, const std::string &func, const std::string &suffix = "");
        std::string cast(CompilerValue *value, Compiler::StaticType targetType);
        void assign(CppRegister *dst, CompilerValue *src);
        void emit(const std::string &line);
        void indent();
        void unindent();
        void createSuspend(bool checkWarp = true);
        bool isWarp() const;

        CompilerValue *createFunctionCall(
            const std::string &functionName,
            Compiler::StaticType returnType,
            const Compiler::ArgTypes &argTypes,
            const Compiler::Args &args,
            const std::string &firstArgType = "",
            const std::string &firstArg = "");

        const std::string &variablePtr(Variable *variable);
        const std::string &listPtr(List *list);

        CppCompilerContext *m_ctx = nullptr;
        BlockPrototype *m_procedurePrototype = nullptr;
        Compiler::CodeType m_codeType = Compiler::CodeType::Script;
        std::string m_functionName;

        std::stringstream m_members;
        std::stringstream m_resolve;
        std::stringstream m_destroy;
        std::stringstream m_body;
        int m_indent = 3;
        int m_memberCount = 0;
        int m_stateCount = 0;

        std::vector<std::unique_ptr<CompilerValue>> m_values;
        std::vector<std::unique_ptr<CompilerLocalVariable>> m_localVariables;
        std::unordered_map<Variable *, std::string> m_variables;
        std::unordered_map<List *, std::string> m_lists;
        std::vector<Loop> m_loops;
        bool m_loopCondition = false;
        CompilerValue *m_lastValue = nullptr;
};

} // namespace libscratchcpp
