// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "icodebuilder.h"

namespace libscratchcpp
{

class Target;

class LLVMCodeBuilder : public ICodeBuilder
{
    public:
        LLVMCodeBuilder(const std::string &id, bool warp);

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

        void createOp(Step::Type type, Compiler::StaticType retType, Compiler::StaticType argType, size_t argCount);
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
