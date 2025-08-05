// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <unordered_map>

namespace libscratchcpp
{

struct LLVMInstruction;
struct LLVMRegister;

class LLVMTypeAnalyzer
{
    public:
        Compiler::StaticType variableType(const Variable *var, const LLVMInstruction *pos, Compiler::StaticType previousType) const;
        Compiler::StaticType variableTypeAfterBranch(const Variable *var, const LLVMInstruction *start, Compiler::StaticType previousType) const;

        Compiler::StaticType listType(const List *list, const LLVMInstruction *pos, Compiler::StaticType previousType, bool isEmpty) const;
        Compiler::StaticType listTypeAfterBranch(const List *list, const LLVMInstruction *start, Compiler::StaticType previousType, bool isEmpty) const;

    private:
        using InstructionSet = std::unordered_set<const LLVMInstruction *>;

        Compiler::StaticType variableType(
            const Variable *var,
            const LLVMInstruction *pos,
            Compiler::StaticType previousType,
            std::unordered_map<const List *, Compiler::StaticType> listTypes,
            InstructionSet &visitedInstructions) const;

        Compiler::StaticType variableTypeAfterBranch(const Variable *var, const LLVMInstruction *start, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const;

        Compiler::StaticType variableTypeAfterBranchFromEnd(
            const Variable *var,
            const LLVMInstruction *end,
            Compiler::StaticType previousType,
            bool &write,
            std::unordered_map<const List *, Compiler::StaticType> listTypes,
            InstructionSet &visitedInstructions) const;

        Compiler::StaticType listType(
            const List *list,
            const LLVMInstruction *pos,
            Compiler::StaticType previousType,
            bool isEmpty,
            std::unordered_map<const List *, Compiler::StaticType> listTypes,
            InstructionSet &visitedInstructions) const;

        Compiler::StaticType listTypeAfterBranch(
            const List *list,
            const LLVMInstruction *start,
            Compiler::StaticType previousType,
            bool isEmpty,
            const LLVMInstruction *query,
            bool &write,
            std::unordered_map<const List *, Compiler::StaticType> listTypes,
            InstructionSet &visitedInstructions) const;

        bool handleListWrite(Compiler::StaticType writeType, Compiler::StaticType &previousType, bool &isEmpty) const;

        const LLVMInstruction *branchEnd(const LLVMInstruction *start) const;
        const LLVMInstruction *branchStart(const LLVMInstruction *end) const;

        bool isLoopStart(const LLVMInstruction *ins) const;
        bool isLoopEnd(const LLVMInstruction *ins) const;
        bool isIfStart(const LLVMInstruction *ins) const;
        bool isElse(const LLVMInstruction *ins) const;
        bool isIfEnd(const LLVMInstruction *ins) const;

        bool isVariableRead(const LLVMInstruction *ins) const;
        bool isVariableWrite(const LLVMInstruction *ins, const Variable *var) const;

        bool isListRead(const LLVMInstruction *ins) const;
        bool isListWrite(const LLVMInstruction *ins, const List *list) const;
        bool isListClear(const LLVMInstruction *ins, const List *list) const;

        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        bool isWriteNoOp(const LLVMInstruction *ins) const;
        Compiler::StaticType writeValueType(const LLVMInstruction *ins, std::unordered_map<const List *, Compiler::StaticType> listTypes, InstructionSet &visitedInstructions) const;
        bool typesMatch(Compiler::StaticType a, Compiler::StaticType b) const;
        bool
        writeTypesMatch(const LLVMInstruction *ins, Compiler::StaticType expectedType, std::unordered_map<const List *, Compiler::StaticType> listTypes, InstructionSet &visitedInstructions) const;
};

} // namespace libscratchcpp
