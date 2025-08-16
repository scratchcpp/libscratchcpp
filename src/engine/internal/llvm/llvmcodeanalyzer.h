// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <unordered_map>

namespace libscratchcpp
{

class LLVMInstructionList;
class LLVMInstruction;

class LLVMCodeAnalyzer
{
    public:
        void analyzeScript(const LLVMInstructionList &script) const;

    private:
        struct Branch
        {
                LLVMInstruction *start = nullptr;
                bool typeChanges = false;
                std::unordered_map<Variable *, Compiler::StaticType> variableTypes;
                std::unordered_map<List *, Compiler::StaticType> listTypes;

                std::unique_ptr<Branch> elseBranch;
        };

        void updateVariableType(Branch *branch, LLVMInstruction *ins, std::unordered_set<LLVMInstruction *> &typeAssignedInstructions, bool isWrite) const;
        void updateListType(Branch *branch, LLVMInstruction *ins, std::unordered_set<LLVMInstruction *> &typeAssignedInstructions, bool isWrite) const;

        void mergeVariableTypes(Branch *branch, Branch *previousBranch) const;
        void overrideVariableTypes(Branch *branch, Branch *previousBranch) const;
        void mergeListTypes(Branch *branch, Branch *previousBranch) const;

        bool isLoopStart(const LLVMInstruction *ins) const;
        bool isLoopEnd(const LLVMInstruction *ins) const;
        bool isIfStart(const LLVMInstruction *ins) const;
        bool isElse(const LLVMInstruction *ins) const;
        bool isIfEnd(const LLVMInstruction *ins) const;

        bool isVariableRead(const LLVMInstruction *ins) const;
        bool isVariableWrite(const LLVMInstruction *ins) const;

        bool isListRead(const LLVMInstruction *ins) const;
        bool isListWrite(const LLVMInstruction *ins) const;
        bool isListClear(const LLVMInstruction *ins) const;

        Compiler::StaticType writeType(LLVMInstruction *ins) const;
};

} // namespace libscratchcpp
