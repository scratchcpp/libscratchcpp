// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct LLVMVariablePtr;
struct LLVMInstruction;
struct LLVMRegister;

class LLVMTypeAnalyzer
{
    public:
        Compiler::StaticType variableType(LLVMVariablePtr *varPtr, LLVMInstruction *pos, Compiler::StaticType previousType) const;
        Compiler::StaticType variableTypeAfterBranch(LLVMVariablePtr *varPtr, LLVMInstruction *start, Compiler::StaticType previousType) const;

    private:
        Compiler::StaticType variableTypeAfterBranchFromEnd(LLVMVariablePtr *varPtr, LLVMInstruction *end, Compiler::StaticType previousType, bool &write) const;
        LLVMInstruction *skipBranch(LLVMInstruction *pos) const;
        bool isLoopStart(LLVMInstruction *ins) const;
        bool isLoopEnd(LLVMInstruction *ins) const;
        bool isIfStart(LLVMInstruction *ins) const;
        bool isElse(LLVMInstruction *ins) const;
        bool isIfEnd(LLVMInstruction *ins) const;

        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        Compiler::StaticType writeValueType(LLVMInstruction *ins) const;
        bool typesMatch(Compiler::StaticType a, Compiler::StaticType b) const;
        bool writeTypesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const;
};

} // namespace libscratchcpp
