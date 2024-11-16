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

class LLVMCodeBuilder : public ICodeBuilder
{
    public:
        LLVMCodeBuilder(Target *target, const std::string &id, bool warp);

        std::shared_ptr<ExecutableCode> finalize() override;

        void addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const std::vector<Compiler::StaticType> &argTypes) override;
        void addConstValue(const Value &value) override;
        void addVariableValue(Variable *variable) override;
        void addListContents(List *list) override;

        void createAdd() override;
        void createSub() override;
        void createMul() override;
        void createDiv() override;

        void createCmpEQ() override;
        void createCmpGT() override;
        void createCmpLT() override;

        void createAnd() override;
        void createOr() override;
        void createNot() override;

        void createMod() override;
        void createRound() override;
        void createAbs() override;
        void createFloor() override;
        void createCeil() override;
        void createSqrt() override;
        void createSin() override;
        void createCos() override;
        void createTan() override;
        void createAsin() override;
        void createAcos() override;
        void createAtan() override;
        void createLn() override;
        void createLog10() override;
        void createExp() override;
        void createExp10() override;

        void createVariableWrite(Variable *variable) override;

        void createListClear(List *list) override;
        void createListRemove(List *list) override;
        void createListAppend(List *list) override;

        void beginIfStatement() override;
        void beginElseBranch() override;
        void endIf() override;

        void beginRepeatLoop() override;
        void beginWhileLoop() override;
        void beginRepeatUntilLoop() override;
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

        void freeHeap();
        llvm::Value *castValue(LLVMRegisterPtr reg, Compiler::StaticType targetType);
        llvm::Value *castRawValue(LLVMRegisterPtr reg, Compiler::StaticType targetType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType);
        Compiler::StaticType optimizeRegisterType(LLVMRegisterPtr reg);
        llvm::Type *getType(Compiler::StaticType type);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        llvm::Value *getListPtr(llvm::Value *targetLists, List *list);
        void syncVariables(llvm::Value *targetVariables);
        void reloadVariables(llvm::Value *targetVariables);

        LLVMInstruction &createOp(LLVMInstruction::Type type, Compiler::StaticType retType, Compiler::StaticType argType, size_t argCount);

        void createValueStore(LLVMRegisterPtr reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);
        void createInitialValueStore(LLVMRegisterPtr reg, llvm::Value *targetPtr, Compiler::StaticType sourceType);
        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);
        llvm::Value *createValue(LLVMRegisterPtr reg);
        llvm::Value *createComparison(LLVMRegisterPtr arg1, LLVMRegisterPtr arg2, Comparison type);

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
        llvm::FunctionCallee resolve_strcasecmp();

        Target *m_target = nullptr;

        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, LLVMVariablePtr> m_variablePtrs;
        std::vector<std::unordered_map<LLVMVariablePtr *, Compiler::StaticType>> m_scopeVariables;

        std::unordered_map<List *, size_t> m_targetListMap;
        std::unordered_map<List *, LLVMListPtr> m_listPtrs;

        std::string m_id;
        llvm::LLVMContext m_ctx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        llvm::StructType *m_valueDataType = nullptr;

        std::vector<LLVMInstruction> m_instructions;
        size_t m_currentFunction = 0;
        std::vector<Value> m_constValues;
        std::vector<std::vector<LLVMRegisterPtr>> m_regs;
        std::vector<LLVMRegisterPtr> m_tmpRegs;
        bool m_defaultWarp = false;
        bool m_warp = false;

        std::vector<llvm::Value *> m_heap;

        std::shared_ptr<ExecutableCode> m_output;
};

} // namespace libscratchcpp
