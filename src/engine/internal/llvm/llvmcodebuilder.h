// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
#include <scratchcpp/blockprototype.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "../icodebuilder.h"
#include "llvminstruction.h"
#include "llvmcoroutine.h"
#include "llvmvariableptr.h"
#include "llvmlistptr.h"

namespace libscratchcpp
{

class LLVMCompilerContext;
class LLVMConstantRegister;
class LLVMLoopScope;

class LLVMCodeBuilder : public ICodeBuilder
{
    public:
        LLVMCodeBuilder(LLVMCompilerContext *ctx, BlockPrototype *procedurePrototype = nullptr, Compiler::CodeType codeType = Compiler::CodeType::Script);

        std::shared_ptr<ExecutableCode> finalize() override;

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

        void createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args) override;

    private:
        enum class Comparison
        {
            EQ,
            GT,
            LT
        };

        void initTypes();
        void createVariableMap();
        void createListMap();
        void pushScopeLevel();
        void popScopeLevel();
        void pushLoopScope(bool buildPhase);
        void popLoopScope();

        std::string getMainFunctionName(BlockPrototype *procedurePrototype);
        std::string getResumeFunctionName(BlockPrototype *procedurePrototype);
        llvm::FunctionType *getMainFunctionType(BlockPrototype *procedurePrototype);
        llvm::Function *getOrCreateFunction(const std::string &name, llvm::FunctionType *type);
        void verifyFunction(llvm::Function *func);

        LLVMRegister *addReg(std::shared_ptr<LLVMRegister> reg, std::shared_ptr<LLVMInstruction> ins);

        llvm::Value *addAlloca(llvm::Type *type);
        void freeStringLater(llvm::Value *value);
        void freeScopeHeap();
        llvm::Value *castValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Value *castRawValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType);
        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        llvm::Type *getType(Compiler::StaticType type);
        Compiler::StaticType getProcedureArgType(BlockPrototype::ArgType type);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        llvm::Value *getListPtr(llvm::Value *targetLists, List *list);
        void syncVariables(llvm::Value *targetVariables);
        void reloadVariables(llvm::Value *targetVariables);
        void reloadLists();
        void updateListDataPtr(const LLVMListPtr &listPtr);
        bool isVarOrListTypeSafe(std::shared_ptr<LLVMInstruction> ins, Compiler::StaticType expectedType) const;
        bool isVarOrListTypeSafe(std::shared_ptr<LLVMInstruction> ins, Compiler::StaticType expectedType, std::unordered_set<LLVMInstruction *> &log, int &c) const;
        bool isVarOrListWriteResultTypeSafe(std::shared_ptr<LLVMInstruction> ins, Compiler::StaticType expectedType, bool ignoreSavedType, std::unordered_set<LLVMInstruction *> &log, int &c) const;

        LLVMRegister *createOp(LLVMInstruction::Type type, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args);
        LLVMRegister *createOp(LLVMInstruction::Type type, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes = {}, const Compiler::Args &args = {});
        LLVMRegister *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args);
        LLVMRegister *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes = {}, const Compiler::Args &args = {});
        LLVMLoopScope *currentLoopScope() const;

        void createValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);
        void createReusedValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);
        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);
        llvm::Value *getListItem(const LLVMListPtr &listPtr, llvm::Value *index);
        llvm::Value *getListItemIndex(const LLVMListPtr &listPtr, LLVMRegister *item);
        llvm::Value *createValue(LLVMRegister *reg);
        llvm::Value *createNewValue(LLVMRegister *reg);
        llvm::Value *createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, bool caseSensitive);

        void createSuspend(LLVMCoroutine *coro, llvm::Value *warpArg, llvm::Value *targetVariables);

        llvm::FunctionCallee resolveFunction(const std::string name, llvm::FunctionType *type);
        llvm::FunctionCallee resolve_value_init();
        llvm::FunctionCallee resolve_value_free();
        llvm::FunctionCallee resolve_value_assign_long();
        llvm::FunctionCallee resolve_value_assign_double();
        llvm::FunctionCallee resolve_value_assign_bool();
        llvm::FunctionCallee resolve_value_assign_stringPtr();
        llvm::FunctionCallee resolve_value_assign_special();
        llvm::FunctionCallee resolve_value_assign_copy();
        llvm::FunctionCallee resolve_value_toDouble();
        llvm::FunctionCallee resolve_value_toBool();
        llvm::FunctionCallee resolve_value_toStringPtr();
        llvm::FunctionCallee resolve_value_doubleToStringPtr();
        llvm::FunctionCallee resolve_value_boolToStringPtr();
        llvm::FunctionCallee resolve_value_stringToDouble();
        llvm::FunctionCallee resolve_value_stringToBool();
        llvm::FunctionCallee resolve_value_equals();
        llvm::FunctionCallee resolve_value_greater();
        llvm::FunctionCallee resolve_value_lower();
        llvm::FunctionCallee resolve_list_clear();
        llvm::FunctionCallee resolve_list_remove();
        llvm::FunctionCallee resolve_list_append_empty();
        llvm::FunctionCallee resolve_list_insert_empty();
        llvm::FunctionCallee resolve_list_data();
        llvm::FunctionCallee resolve_list_size_ptr();
        llvm::FunctionCallee resolve_list_alloc_size_ptr();
        llvm::FunctionCallee resolve_list_to_string();
        llvm::FunctionCallee resolve_llvm_random();
        llvm::FunctionCallee resolve_llvm_random_double();
        llvm::FunctionCallee resolve_llvm_random_long();
        llvm::FunctionCallee resolve_llvm_random_bool();
        llvm::FunctionCallee resolve_string_pool_new();
        llvm::FunctionCallee resolve_string_pool_free();
        llvm::FunctionCallee resolve_string_alloc();
        llvm::FunctionCallee resolve_string_assign();
        llvm::FunctionCallee resolve_string_compare_case_sensitive();
        llvm::FunctionCallee resolve_string_compare_case_insensitive();

        Target *m_target = nullptr;

        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, LLVMVariablePtr> m_variablePtrs;
        std::vector<std::unordered_map<LLVMVariablePtr *, Compiler::StaticType>> m_scopeVariables;

        std::unordered_map<List *, size_t> m_targetListMap;
        std::unordered_map<List *, LLVMListPtr> m_listPtrs;
        std::vector<std::unordered_map<LLVMListPtr *, Compiler::StaticType>> m_scopeLists;

        LLVMCompilerContext *m_ctx;
        llvm::LLVMContext &m_llvmCtx;
        llvm::Module *m_module = nullptr;
        llvm::IRBuilder<> m_builder;
        llvm::Function *m_function = nullptr;

        llvm::StructType *m_valueDataType = nullptr;
        llvm::StructType *m_stringPtrType = nullptr;
        llvm::FunctionType *m_resumeFuncType = nullptr;

        std::vector<std::shared_ptr<LLVMInstruction>> m_instructions;
        std::vector<std::shared_ptr<LLVMRegister>> m_regs;
        std::vector<std::shared_ptr<CompilerLocalVariable>> m_localVars;
        LLVMRegister *m_lastConstValue = nullptr; // for reporters and hat predicates
        BlockPrototype *m_procedurePrototype = nullptr;
        bool m_defaultWarp = false;
        bool m_warp = false;
        int m_defaultArgCount = 0;
        Compiler::CodeType m_codeType = Compiler::CodeType::Script;

        long m_loopScope = -1; // index
        std::vector<std::shared_ptr<LLVMLoopScope>> m_loopScopes;
        long m_loopScopeCounter = 0; // replacement for m_loopScopes size in build phase
        std::vector<long> m_loopScopeTree;
        bool m_loopCondition = false; // whether we're currently compiling a loop condition
        std::vector<std::shared_ptr<LLVMInstruction>> m_variableInstructions;
        std::vector<std::shared_ptr<LLVMInstruction>> m_listInstructions;
        std::vector<std::vector<llvm::Value *>> m_stringHeap; // scopes

        std::shared_ptr<ExecutableCode> m_output;
};

} // namespace libscratchcpp
