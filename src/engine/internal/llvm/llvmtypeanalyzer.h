// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct LLVMInstruction;
struct LLVMRegister;

class LLVMTypeAnalyzer
{
    public:
        Compiler::StaticType variableType(Variable *var, LLVMInstruction *pos, Compiler::StaticType previousType) const;
        Compiler::StaticType variableTypeAfterBranch(Variable *var, LLVMInstruction *start, Compiler::StaticType previousType) const;

    private:
        using InstructionSet = std::unordered_set<LLVMInstruction *>;

        Compiler::StaticType variableType(Variable *var, LLVMInstruction *pos, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const;
        Compiler::StaticType variableTypeAfterBranch(Variable *var, LLVMInstruction *start, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const;
        Compiler::StaticType variableTypeAfterBranchFromEnd(Variable *var, LLVMInstruction *end, Compiler::StaticType previousType, bool &write, InstructionSet &visitedInstructions) const;

        LLVMInstruction *skipBranch(LLVMInstruction *pos) const;

        bool isLoopStart(LLVMInstruction *ins) const;
        bool isLoopEnd(LLVMInstruction *ins) const;
        bool isIfStart(LLVMInstruction *ins) const;
        bool isElse(LLVMInstruction *ins) const;
        bool isIfEnd(LLVMInstruction *ins) const;
        bool isVariableRead(LLVMInstruction *ins) const;
        bool isVariableWrite(LLVMInstruction *ins, Variable *var) const;

        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        bool isWriteNoOp(LLVMInstruction *ins) const;
        Compiler::StaticType writeValueType(LLVMInstruction *ins, InstructionSet &visitedInstructions) const;
        bool typesMatch(Compiler::StaticType a, Compiler::StaticType b) const;
        bool writeTypesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType, InstructionSet &visitedInstructions) const;
};

} // namespace libscratchcpp
