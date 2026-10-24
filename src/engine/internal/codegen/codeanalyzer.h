// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <unordered_map>

#include "test_export.h"

namespace libscratchcpp
{

class InstructionListBase;
class InstructionBase;

class LIBSCRATCHCPP_TEST_EXPORT CodeAnalyzer
{
    public:
        CodeAnalyzer(CompilerContext *ctx);
        CodeAnalyzer(const CodeAnalyzer &) = delete;

        void analyzeScript(const InstructionListBase &script) const;

    private:
        struct Branch
        {
                InstructionBase *start = nullptr;
                bool typeChanges = false;
                std::unordered_map<Variable *, Compiler::StaticType> variableTypes;
                std::unordered_map<List *, Compiler::StaticType> listTypes;

                std::unique_ptr<Branch> elseBranch;
                bool isAtElse = false; // whether we're currently processing the else branch
        };

        void updateVariableType(Branch *branch, InstructionBase *ins, std::unordered_set<InstructionBase *> &typeAssignedInstructions, bool isWrite) const;
        void updateListType(Branch *branch, InstructionBase *ins, std::unordered_set<InstructionBase *> &typeAssignedInstructions, bool isWrite) const;

        void mergeVariableTypes(Branch *branch, Branch *previousBranch) const;
        void overrideVariableTypes(Branch *branch, Branch *previousBranch) const;
        void mergeListTypes(Branch *branch, Branch *previousBranch, bool firstUnknown) const;

        bool isLoopStart(const InstructionBase *ins) const;
        bool isLoopEnd(const InstructionBase *ins) const;
        bool isIfStart(const InstructionBase *ins) const;
        bool isElse(const InstructionBase *ins) const;
        bool isIfEnd(const InstructionBase *ins) const;

        bool isVariableRead(const InstructionBase *ins) const;
        bool isVariableWrite(const InstructionBase *ins) const;

        bool isListRead(const InstructionBase *ins) const;
        bool isListWrite(const InstructionBase *ins) const;
        bool isListClear(const InstructionBase *ins) const;

        bool isProcedureCall(const InstructionBase *ins) const;

        Compiler::StaticType writeType(InstructionBase *ins) const;

    private:
        CompilerContext *m_ctx;
};

} // namespace libscratchcpp
