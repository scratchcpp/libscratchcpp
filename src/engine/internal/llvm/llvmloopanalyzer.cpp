// SPDX-License-Identifier: Apache-2.0

#include "llvmloopanalyzer.h"
#include "llvminstruction.h"
#include "llvmvariableptr.h"

using namespace libscratchcpp;

bool LLVMLoopAnalyzer::variableTypeChanges(LLVMVariablePtr *varPtr, LLVMInstruction *loopBody, Compiler::StaticType preLoopType) const
{
    if (!varPtr || !loopBody)
        return false;

    if (preLoopType == Compiler::StaticType::Unknown)
        return true;

    // Find the last write instruction
    LLVMInstruction *lastVarWrite = nullptr;
    LLVMInstruction *ins = loopBody;

    while (ins && ins->type != LLVMInstruction::Type::EndLoop) {
        // TODO: Handle nested loops
        if (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == varPtr->var)
            lastVarWrite = ins;

        ins = ins->next;
    }

    // Loops must always have an end instruction
    assert(ins);

    // Check the last write instruction, if any
    return lastVarWrite ? !typesMatch(lastVarWrite, preLoopType) : false;
}

Compiler::StaticType LLVMLoopAnalyzer::optimizeRegisterType(LLVMRegister *reg) const
{
    // TODO: Move this method out if it's used in LLVMCodeBuilder too
    assert(reg);

    Compiler::StaticType ret = reg->type();

    // Optimize string constants that represent numbers
    if (reg->isConst() && reg->type() == Compiler::StaticType::String && reg->constValue().isValidNumber())
        ret = Compiler::StaticType::Number;

    return ret;
}

bool LLVMLoopAnalyzer::typesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const
{
    assert(ins);
    assert(!ins->args.empty());
    const auto arg = ins->args.back().second; // value is always the last argument in variable/list write instructions
    auto argIns = arg->instruction;

    // TODO: Handle cross-variable dependencies
    /*if (argIns && (argIns->type == LLVMInstruction::Type::ReadVariable || argIns->type == LLVMInstruction::Type::GetListItem))
        return isVarOrListTypeSafe(argIns.get(), expectedType, log, c);*/

    return (optimizeRegisterType(arg) == expectedType);
}
