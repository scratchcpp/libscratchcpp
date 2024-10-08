// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/valuedata.h>
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
        LLVMCodeBuilder(const std::string &id);
        ~LLVMCodeBuilder();

        std::shared_ptr<ExecutableCode> finalize() override;

        void addFunctionCall(const std::string &functionName, int argCount, bool returns) override;
        void addConstValue(const Value &value) override;
        void addVariableValue(Variable *variable) override;
        void addListContents(List *list) override;

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
                llvm::Value *value = nullptr;
                bool isConstValue = false;
                size_t constValueIndex = 0;
        };

        struct Step
        {
                enum class Type
                {
                    FunctionCall,
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
                std::vector<std::shared_ptr<Register>> args;
                bool functionReturns = false;
                size_t functionReturnRegIndex = 0;
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

        void initTypes();
        llvm::Function *beginFunction(size_t index);
        void endFunction(llvm::Function *func, size_t index);

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

        std::string m_id;
        llvm::LLVMContext m_ctx;
        std::unique_ptr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        llvm::StructType *m_valueDataType = nullptr;

        std::vector<Step> m_steps;
        size_t m_currentFunction = 0;
        std::vector<std::vector<std::unique_ptr<ValueData>>> m_constValues;
        std::vector<std::vector<std::shared_ptr<Register>>> m_regs;
        std::vector<std::shared_ptr<Register>> m_tmpRegs;

        std::shared_ptr<ExecutableCode> m_output;
};

} // namespace libscratchcpp
