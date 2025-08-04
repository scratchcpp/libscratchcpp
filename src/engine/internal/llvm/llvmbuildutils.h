// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/valuedata.h>

#include "llvmfunctions.h"
#include "llvmvariableptr.h"
#include "llvmlistptr.h"

namespace libscratchcpp
{

class LLVMRegister;

class LLVMBuildUtils
{
    public:
        enum class Comparison
        {
            EQ,
            GT,
            LT
        };

        LLVMBuildUtils(LLVMCompilerContext *ctx, llvm::IRBuilder<> &builder);

        void init(llvm::Function *function, llvm::Value *targetVariables, llvm::Value *targetLists);
        void end();

        llvm::IRBuilder<> &builder();
        LLVMFunctions &functions();

        llvm::Value *targetVariables();
        llvm::Value *targetLists();

        void createVariablePtr(Variable *variable);
        void createListPtr(List *list);

        LLVMVariablePtr &variablePtr(Variable *variable);
        LLVMListPtr &listPtr(List *list);

        void syncVariables(llvm::Value *targetVariables);
        void reloadVariables(llvm::Value *targetVariables);

        void pushScopeLevel();
        void popScopeLevel();

        void freeStringLater(llvm::Value *value);
        void freeScopeHeap();

        static Compiler::StaticType optimizeRegisterType(LLVMRegister *reg);
        static Compiler::StaticType mapType(ValueType type);

        llvm::Value *addAlloca(llvm::Type *type);
        llvm::Value *castValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Type *getType(Compiler::StaticType type);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        void createValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);
        void createReusedValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType);

        llvm::Value *getListItem(const LLVMListPtr &listPtr, llvm::Value *index);
        llvm::Value *getListItemIndex(const LLVMListPtr &listPtr, Compiler::StaticType listType, LLVMRegister *item);
        llvm::Value *createValue(LLVMRegister *reg);
        llvm::Value *createNewValue(LLVMRegister *reg);
        llvm::Value *createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, bool caseSensitive);

    private:
        void initTypes();
        void createVariableMap();
        void createListMap();

        llvm::Value *castRawValue(LLVMRegister *reg, Compiler::StaticType targetType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType);

        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        llvm::Value *getListPtr(llvm::Value *targetLists, List *list);
        llvm::Value *getListDataPtr(const LLVMListPtr &listPtr);

        LLVMCompilerContext *m_ctx = nullptr;
        llvm::LLVMContext &m_llvmCtx;
        llvm::IRBuilder<> &m_builder;
        LLVMFunctions m_functions;
        Target *m_target = nullptr;
        llvm::Function *m_function = nullptr;

        llvm::StructType *m_valueDataType = nullptr;
        llvm::StructType *m_stringPtrType = nullptr;

        llvm::Value *m_targetVariables = nullptr;
        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, LLVMVariablePtr> m_variablePtrs;

        llvm::Value *m_targetLists = nullptr;
        std::unordered_map<List *, size_t> m_targetListMap;
        std::unordered_map<List *, LLVMListPtr> m_listPtrs;

        std::vector<std::vector<llvm::Value *>> m_stringHeap; // scopes
};

} // namespace libscratchcpp
