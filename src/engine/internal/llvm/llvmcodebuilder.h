// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
#include <scratchcpp/blockprototype.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "../icodebuilder.h"
#include "llvmbuildutils.h"
#include "llvmcodeanalyzer.h"
#include "llvminstruction.h"
#include "llvminstructionlist.h"
#include "llvmcoroutine.h"
#include "llvmvariableptr.h"
#include "llvmlistptr.h"
#include "llvmfunctions.h"
#include "instructions/instructionbuilder.h"

namespace libscratchcpp
{

class LLVMCompilerContext;
class LLVMConstantRegister;

class LLVMCodeBuilder : public ICodeBuilder
{
    public:
        LLVMCodeBuilder(LLVMCompilerContext *ctx, BlockPrototype *procedurePrototype = nullptr, Compiler::CodeType codeType = Compiler::CodeType::Script);

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

        void createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args) override;

    private:
        void initTypes();

        llvm::Function *getOrCreateFunction(const std::string &name, llvm::FunctionType *type);
        void verifyFunction(llvm::Function *func);

        LLVMRegister *addReg(std::shared_ptr<LLVMRegister> reg, std::shared_ptr<LLVMInstruction> ins);

        Compiler::StaticType getProcedureArgType(BlockPrototype::ArgType type);

        LLVMRegister *createOp(LLVMInstruction::Type type, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args);
        LLVMRegister *createOp(LLVMInstruction::Type type, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes = {}, const Compiler::Args &args = {});
        LLVMRegister *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args);
        LLVMRegister *createOp(const LLVMInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes = {}, const Compiler::Args &args = {});

        Target *m_target = nullptr;

        LLVMCompilerContext *m_ctx;
        llvm::LLVMContext &m_llvmCtx;
        llvm::Module *m_module = nullptr;
        llvm::IRBuilder<> m_builder;
        llvm::Function *m_function = nullptr;
        LLVMBuildUtils m_utils;
        LLVMCodeAnalyzer m_codeAnalyzer;

        llvm::StructType *m_valueDataType = nullptr;
        llvm::StructType *m_stringPtrType = nullptr;

        LLVMInstructionList m_instructions;
        std::vector<std::shared_ptr<LLVMRegister>> m_regs;
        std::vector<std::shared_ptr<CompilerLocalVariable>> m_localVars;
        LLVMRegister *m_lastConstValue = nullptr; // for reporters and hat predicates
        BlockPrototype *m_procedurePrototype = nullptr;
        bool m_defaultWarp = false;
        bool m_warp = false;
        Compiler::CodeType m_codeType = Compiler::CodeType::Script;

        bool m_loopCondition = false; // whether we're currently compiling a loop condition

        llvmins::InstructionBuilder m_instructionBuilder;
};

} // namespace libscratchcpp
