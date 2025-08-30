// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/valuedata.h>

#include "llvmfunctions.h"
#include "llvmlocalvariableinfo.h"
#include "llvmvariableptr.h"
#include "llvmlistptr.h"
#include "llvmcoroutine.h"
#include "llvmifstatement.h"
#include "llvmloop.h"
#include "llvmcompilercontext.h"

namespace libscratchcpp
{

class LLVMRegister;
class LLVMCoroutine;

class LLVMBuildUtils
{
    public:
        enum class Comparison
        {
            EQ,
            GT,
            LT
        };

        enum class NumberType
        {
            Int,
            Double
        };

        LLVMBuildUtils(LLVMCompilerContext *ctx, llvm::IRBuilder<> &builder, Compiler::CodeType codeType);

        void init(llvm::Function *function, BlockPrototype *procedurePrototype, bool warp, const std::vector<std::shared_ptr<LLVMRegister>> &regs);
        void end(LLVMInstruction *lastInstruction, LLVMRegister *lastConstant);

        LLVMCompilerContext *compilerCtx() const;
        llvm::LLVMContext &llvmCtx();
        llvm::Module *module() const;
        llvm::IRBuilder<> &builder();
        llvm::Function *function() const;
        LLVMFunctions &functions();

        std::string scriptFunctionName(BlockPrototype *procedurePrototype);
        llvm::FunctionType *scriptFunctionType(BlockPrototype *procedurePrototype);

        function_id_t scriptFunctionId() const;

        size_t stringCount() const;

        llvm::BasicBlock *endBranch() const;

        BlockPrototype *procedurePrototype() const;
        bool warp() const;

        llvm::Value *executionContextPtr();
        llvm::Value *targetPtr();
        llvm::Value *targetVariables();
        llvm::Value *targetLists();
        llvm::Value *warpArg();

        LLVMCoroutine *coroutine() const;

        void createLocalVariableInfo(CompilerLocalVariable *variable);
        void createVariablePtr(Variable *variable);
        void createListPtr(List *list);

        LLVMLocalVariableInfo &localVariableInfo(CompilerLocalVariable *variable);
        LLVMVariablePtr &variablePtr(Variable *variable);
        LLVMListPtr &listPtr(List *list);

        void syncVariables();
        void reloadVariables();
        void reloadLists();

        std::vector<LLVMIfStatement> &ifStatements();
        std::vector<LLVMLoop> &loops();

        static Compiler::StaticType optimizeRegisterType(const LLVMRegister *reg);
        static Compiler::StaticType mapType(ValueType type);
        static ValueType mapType(Compiler::StaticType type);
        static bool isSingleType(Compiler::StaticType type);

        llvm::Value *addAlloca(llvm::Type *type);
        llvm::Value *addStringAlloca();

        llvm::Value *castValue(LLVMRegister *reg, Compiler::StaticType targetType, NumberType targetNumType = NumberType::Double);
        llvm::Type *getType(Compiler::StaticType type, bool isReturnType);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        void createValueStore(
            llvm::Value *destPtr,
            llvm::Value *destTypePtr,
            llvm::Value *destIsIntVar,
            llvm::Value *destIntVar,
            LLVMRegister *reg,
            Compiler::StaticType destType,
            Compiler::StaticType targetType);

        void createValueStore(llvm::Value *destPtr, llvm::Value *destTypePtr, llvm::Value *destIsIntVar, llvm::Value *destIntVar, LLVMRegister *reg, Compiler::StaticType targetType);

        llvm::Value *getValueTypePtr(llvm::Value *value);
        llvm::Value *getValueTypePtr(LLVMRegister *reg);

        llvm::Value *getListSize(const LLVMListPtr &listPtr);
        llvm::Value *getListItem(const LLVMListPtr &listPtr, llvm::Value *index);
        llvm::Value *getListItemIndex(const LLVMListPtr &listPtr, Compiler::StaticType listType, LLVMRegister *item);
        llvm::Value *createValue(LLVMRegister *reg);
        llvm::Value *createNewValue(LLVMRegister *reg);
        llvm::Value *createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, bool caseSensitive);

        void createSuspend();

    private:
        void initTypes();
        void createVariableMap();
        void createListMap();

        llvm::Value *loadRegisterType(LLVMRegister *reg, Compiler::StaticType type);

        llvm::Value *castRawValue(LLVMRegister *reg, Compiler::StaticType targetType, NumberType targetNumType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType, NumberType targetNumType);

        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);

        Comparison swapComparisonArgs(Comparison type);

        llvm::Value *createNumberAndNumberComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createBoolAndBoolComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createStringAndStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);

        llvm::Value *createNumberAndBoolComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createNumberAndStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);
        llvm::Value *createBoolAndStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        llvm::Value *getListPtr(llvm::Value *targetLists, List *list);
        llvm::Value *getListDataPtr(const LLVMListPtr &listPtr);

        LLVMCompilerContext *m_ctx = nullptr;
        llvm::LLVMContext &m_llvmCtx;
        llvm::IRBuilder<> &m_builder;
        LLVMFunctions m_functions;
        Target *m_target = nullptr;
        llvm::Function *m_function = nullptr;
        function_id_t m_functionId = 0;
        llvm::Value *m_functionIdValue = nullptr;

        llvm::BasicBlock *m_endBranch = nullptr;

        llvm::StructType *m_valueDataType = nullptr;
        llvm::StructType *m_stringPtrType = nullptr;
        llvm::Type *m_functionIdType = nullptr;

        BlockPrototype *m_procedurePrototype = nullptr;
        bool m_warp = false;
        Compiler::CodeType m_codeType = Compiler::CodeType::Script;

        llvm::Value *m_executionContextPtr = nullptr;
        llvm::Value *m_targetPtr = nullptr;
        llvm::Value *m_targetVariables = nullptr;
        llvm::Value *m_targetLists = nullptr;
        llvm::Value *m_warpArg = nullptr;

        std::unique_ptr<LLVMCoroutine> m_coroutine;

        std::unordered_map<CompilerLocalVariable *, LLVMLocalVariableInfo> m_localVariables;

        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, LLVMVariablePtr> m_variablePtrs;

        std::unordered_map<List *, size_t> m_targetListMap;
        std::unordered_map<List *, LLVMListPtr> m_listPtrs;

        std::vector<LLVMIfStatement> m_ifStatements;
        std::vector<LLVMLoop> m_loops;

        llvm::BasicBlock *m_stringAllocaBlock = nullptr;
        llvm::BasicBlock *m_stringAllocaNextBlock = nullptr;
        llvm::Value *m_stringArray = nullptr;
        size_t m_stringCount = 0;
};

} // namespace libscratchcpp
