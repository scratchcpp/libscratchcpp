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
        Compiler::StaticType variableTypeAfterBranchFromEnd(Variable *var, LLVMInstruction *end, Compiler::StaticType previousType, bool &write) const;
        LLVMInstruction *skipBranch(LLVMInstruction *pos) const;
        bool isLoopStart(LLVMInstruction *ins) const;
        bool isLoopEnd(LLVMInstruction *ins) const;
        bool isIfStart(LLVMInstruction *ins) const;
        bool isElse(LLVMInstruction *ins) const;
        bool isIfEnd(LLVMInstruction *ins) const;
        bool isVariableWrite(LLVMInstruction *ins, Variable *var) const;

        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        Compiler::StaticType writeValueType(LLVMInstruction *ins) const;
        bool typesMatch(Compiler::StaticType a, Compiler::StaticType b) const;
        bool writeTypesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const;
};

} // namespace libscratchcpp
