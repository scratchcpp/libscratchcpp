// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct LLVMVariablePtr;
struct LLVMInstruction;
struct LLVMRegister;

class LLVMLoopAnalyzer
{
    public:
        bool variableTypeChanges(LLVMVariablePtr *varPtr, LLVMInstruction *loopBody, Compiler::StaticType preLoopType) const;

    private:
        bool variableTypeChangesFromEnd(LLVMVariablePtr *varPtr, LLVMInstruction *loopEnd, Compiler::StaticType preLoopType) const;
        bool isLoopStart(LLVMInstruction *ins) const;
        bool isLoopEnd(LLVMInstruction *ins) const;
        bool isIfStart(LLVMInstruction *ins) const;
        bool isElse(LLVMInstruction *ins) const;
        bool isIfEnd(LLVMInstruction *ins) const;

        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        Compiler::StaticType writeValueType(LLVMInstruction *ins) const;
        bool typesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const;
};

} // namespace libscratchcpp
