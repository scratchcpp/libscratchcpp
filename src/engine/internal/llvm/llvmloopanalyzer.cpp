// SPDX-License-Identifier: Apache-2.0

#include "llvmloopanalyzer.h"
#include "llvminstruction.h"
#include "llvmvariableptr.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginWhileLoop, LLVMInstruction::Type::BeginRepeatUntilLoop };

bool LLVMLoopAnalyzer::variableTypeChanges(LLVMVariablePtr *varPtr, LLVMInstruction *loopBody, Compiler::StaticType preLoopType) const
{
    if (!varPtr || !loopBody)
        return false;

    if (preLoopType == Compiler::StaticType::Unknown)
        return true;

    // Find loop end
    LLVMInstruction *ins = loopBody->next;
    int loopLevel = 0;

    while (ins && !(isLoopEnd(ins) && loopLevel == 0)) {
        if (isLoopStart(ins))
            loopLevel++;
        else if (isLoopEnd(ins)) {
            assert(loopLevel > 0);
            loopLevel--;
        }

        ins = ins->next;
    }

    // Loops must always have an end instruction
    if (!ins) {
        assert(false);
        return true;
    }

    return variableTypeChangesFromEnd(varPtr, ins, preLoopType);
}

bool LLVMLoopAnalyzer::variableTypeChangesFromEnd(LLVMVariablePtr *varPtr, LLVMInstruction *loopEnd, Compiler::StaticType preLoopType) const
{
    // Find the last write instruction
    LLVMInstruction *ins = loopEnd->previous;

    while (ins && !isLoopStart(ins)) {
        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            // Nested loop or if statement
            if (variableTypeChangesFromEnd(varPtr, ins, preLoopType))
                return true;

            // Skip the loop or if statement
            int level = 0;
            ins = ins->previous;

            while (ins && !((isLoopStart(ins) || isIfStart(ins) || isElse(ins)) && level == 0)) {
                if (isLoopStart(ins) || isIfStart(ins)) {
                    assert(level > 0);
                    level--;
                }

                if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins))
                    level++;

                ins = ins->previous;
            };

            if (!ins) {
                assert(false);
                return true;
            }
        } else if (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == varPtr->var) {
            // Variable write instruction
            return !typesMatch(ins, preLoopType);
        }

        ins = ins->previous;
    }

    return false;
}

bool LLVMLoopAnalyzer::isLoopStart(LLVMInstruction *ins) const
{
    return (BEGIN_LOOP_INSTRUCTIONS.find(ins->type) != BEGIN_LOOP_INSTRUCTIONS.cend());
}

bool LLVMLoopAnalyzer::isLoopEnd(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndLoop);
}

bool LLVMLoopAnalyzer::isIfStart(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginIf);
}

bool LLVMLoopAnalyzer::isElse(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginElse);
}

bool LLVMLoopAnalyzer::isIfEnd(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndIf);
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
