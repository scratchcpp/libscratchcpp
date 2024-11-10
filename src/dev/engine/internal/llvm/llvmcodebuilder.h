// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "../icodebuilder.h"

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
        struct Register
        {
                Register(Compiler::StaticType type) :
                    type(type)
                {
                }

                Compiler::StaticType type = Compiler::StaticType::Void;
                llvm::Value *value = nullptr;
                bool isRawValue = false;
                bool isConstValue = false;
                Value constValue;
        };

        struct Step
        {
                enum class Type
                {
                    FunctionCall,
                    Add,
                    Sub,
                    Mul,
                    Div,
                    CmpEQ,
                    CmpGT,
                    CmpLT,
                    And,
                    Or,
                    Not,
                    Mod,
                    Round,
                    Abs,
                    Floor,
                    Ceil,
                    Sqrt,
                    Sin,
                    Cos,
                    Tan,
                    Asin,
                    Acos,
                    Atan,
                    Ln,
                    Log10,
                    Exp,
                    Exp10,
                    WriteVariable,
                    ReadVariable,
                    Yield,
                    BeginIf,
                    BeginElse,
                    EndIf,
                    BeginRepeatLoop,
                    BeginWhileLoop,
                    BeginRepeatUntilLoop,
                    BeginLoopCondition,
                    EndLoop
                };

                Step(Type type) :
                    type(type)
                {
                }

                Type type;
                std::string functionName;
                std::vector<std::pair<Compiler::StaticType, std::shared_ptr<Register>>> args; // target type, register
                Compiler::StaticType functionReturnType = Compiler::StaticType::Void;
                std::shared_ptr<Register> functionReturnReg;
                Variable *workVariable = nullptr; // for variable write
        };

        struct IfStatement
        {
                llvm::Value *condition = nullptr;
                llvm::BasicBlock *beforeIf = nullptr;
                llvm::BasicBlock *body = nullptr;
                llvm::BasicBlock *elseBranch = nullptr;
                llvm::BasicBlock *afterIf = nullptr;
        };

        struct Loop
        {
                bool isRepeatLoop = false;
                llvm::Value *index = nullptr;
                llvm::BasicBlock *conditionBranch = nullptr;
                llvm::BasicBlock *afterLoop = nullptr;
        };

        struct Coroutine
        {
                llvm::Value *handle = nullptr;
                llvm::BasicBlock *suspend = nullptr;
                llvm::BasicBlock *cleanup = nullptr;
                llvm::BasicBlock *freeMemRet = nullptr;
                llvm::Value *didSuspend = nullptr;
        };

        struct VariablePtr
        {
                llvm::Value *ptr = nullptr;
                Compiler::StaticType type = Compiler::StaticType::Unknown;
                bool onStack = false;
                bool changed = false;
        };

        struct Procedure
        {
                // TODO: Implement procedures
                bool warp = false;
        };

        enum class Comparison
        {
            EQ,
            GT,
            LT
        };

        void initTypes();
        void createVariableMap();

        Coroutine initCoroutine(llvm::Function *func);
        void verifyFunction(llvm::Function *func);
        void optimize();

        void freeHeap();
        llvm::Value *castValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType);
        llvm::Value *castRawValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType);
        llvm::Constant *castConstValue(const Value &value, Compiler::StaticType targetType);
        llvm::Type *getType(Compiler::StaticType type);
        llvm::Value *isNaN(llvm::Value *num);
        llvm::Value *removeNaN(llvm::Value *num);

        llvm::Value *getVariablePtr(llvm::Value *targetVariables, Variable *variable);
        void syncVariables(llvm::Value *targetVariables);

        Step &createOp(Step::Type type, Compiler::StaticType retType, Compiler::StaticType argType, size_t argCount);

        void createValueStore(std::shared_ptr<Register> reg, llvm::Value *targetPtr);
        void createValueCopy(llvm::Value *source, llvm::Value *target);
        void copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType);
        llvm::Value *createValue(std::shared_ptr<Register> reg);
        llvm::Value *createComparison(std::shared_ptr<Register> arg1, std::shared_ptr<Register> arg2, Comparison type);

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
        llvm::FunctionCallee resolve_strcasecmp();

        Target *m_target = nullptr;
        std::unordered_map<Variable *, size_t> m_targetVariableMap;
        std::unordered_map<Variable *, VariablePtr> m_variablePtrs;

        std::string m_id;
        llvm::LLVMContext m_ctx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        llvm::StructType *m_valueDataType = nullptr;

        std::vector<Step> m_steps;
        size_t m_currentFunction = 0;
        std::vector<Value> m_constValues;
        std::vector<std::vector<std::shared_ptr<Register>>> m_regs;
        std::vector<std::shared_ptr<Register>> m_tmpRegs;
        bool m_defaultWarp = false;
        bool m_warp = false;

        std::vector<llvm::Value *> m_heap;

        std::shared_ptr<ExecutableCode> m_output;
};

} // namespace libscratchcpp
