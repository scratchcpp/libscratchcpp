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
        Compiler::StaticType optimizeRegisterType(LLVMRegister *reg) const;
        bool typesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const;
};

} // namespace libscratchcpp
