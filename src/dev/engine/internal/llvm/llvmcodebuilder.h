// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
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

class Target;
class LLVMConstantRegister;

class LLVMCodeBuilder : public ICodeBuilder
{
    public:
        LLVMCodeBuilder(Target *target, const std::string &id, bool warp);

        std::shared_ptr<ExecutableCode> finalize() override;

        CompilerValue *addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) override;
        CompilerConstant *addConstValue(const Value &value) override;
        CompilerValue *addVariableValue(Variable *variable) override;
        CompilerValue *addListContents(List *list) override;
        CompilerValue *addListItem(List *list, CompilerValue *index) override;
        CompilerValue *addListItemIndex(List *list, CompilerValue *item) override;
        CompilerValue *addListContains(List *list, CompilerValue *item) override;
        CompilerValue *addListSize(List *list) override;

        CompilerValue *createAdd(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createSub(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createMul(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createDiv(CompilerValue *operand1, CompilerValue *operand2) override;

        CompilerValue *createCmpEQ(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createCmpGT(CompilerValue *operand1, CompilerValue *operand2) override;
        CompilerValue *createCmpLT(CompilerValue *operand1, CompilerValue *operand2) override;

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

        void verifyFunction(llvm::Function *func);
        void optimize();

        CompilerValue *addReg(std::shared_ptr<LLVMRegister> reg);

        void freeHeap();
        llvm::Value *castValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Value *castRawValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType);
        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg);
        llvm::Type *getType(Compiler::StaticType type);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        llvm::Value *getListPtr(llvm::Value *targetLists, List *list);
        void syncVariables(llvm::Value *targetVariables);
        void reloadVariables(llvm::Value *targetVariables);
        void reloadLists();
        void updateListDataPtr(const LLVMListPtr &listPtr, llvm::Function *func);

        CompilerValue *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args);
        CompilerValue *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes = {}, const Compiler::Args &args = {});

        void createValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);
        void createReusedValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType);
        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);
        llvm::Value *getListItem(const LLVMListPtr &listPtr, llvm::Value *index, llvm::Function *func);
        llvm::Value *getListItemIndex(const LLVMListPtr &listPtr, LLVMRegister *item, llvm::Function *func);
        llvm::Value *createValue(LLVMRegister *reg);
        llvm::Value *createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);

        llvm::FunctionCallee resolveFunction(const std::string name, llvm::FunctionType *type);
        llvm::FunctionCallee resolve_value_init();
        llvm::FunctionCallee resolve_value_free();
        llvm::FunctionCallee resolve_value_assign_long();
        llvm::FunctionCallee resolve_value_assign_double();
        llvm::FunctionCallee resolve_value_assign_bool();
        llvm::FunctionCallee resolve_value_assign_cstring();
        llvm::FunctionCallee resolve_value_assign_special();
        llvm::FunctionCallee resolve_value_assign_copy();
        llvm::FunctionCallee resolve_value_toDouble();
        llvm::FunctionCallee resolve_value_toBool();
        llvm::FunctionCallee resolve_value_toCString();
        llvm::FunctionCallee resolve_value_doubleToCString();
        llvm::FunctionCallee resolve_value_boolToCString();
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
        llvm::FunctionCallee resolve_strcasecmp();

        Target *m_target = nullptr;

        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, LLVMVariablePtr> m_variablePtrs;
        std::vector<std::unordered_map<LLVMVariablePtr *, Compiler::StaticType>> m_scopeVariables;

        std::unordered_map<List *, size_t> m_targetListMap;
        std::unordered_map<List *, LLVMListPtr> m_listPtrs;
        std::vector<std::unordered_map<LLVMListPtr *, Compiler::StaticType>> m_scopeLists;

        std::string m_id;
        llvm::LLVMContext m_ctx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        llvm::StructType *m_valueDataType = nullptr;

        std::vector<LLVMInstruction> m_instructions;
        std::vector<std::shared_ptr<LLVMRegister>> m_regs;
        bool m_defaultWarp = false;
        bool m_warp = false;

        std::vector<llvm::Value *> m_heap;

        std::shared_ptr<ExecutableCode> m_output;
};

} // namespace libscratchcpp
