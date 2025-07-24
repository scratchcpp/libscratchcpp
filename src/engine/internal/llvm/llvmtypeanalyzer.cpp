// SPDX-License-Identifier: Apache-2.0

#include "llvmtypeanalyzer.h"
#include "llvminstruction.h"
#include "llvmvariableptr.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginWhileLoop, LLVMInstruction::Type::BeginRepeatUntilLoop };

Compiler::StaticType LLVMTypeAnalyzer::variableType(LLVMVariablePtr *varPtr, LLVMInstruction *pos, Compiler::StaticType previousType) const
{
    if (!varPtr || !pos)
        return Compiler::StaticType::Unknown;

    // Check the last write operation before the instruction
    LLVMInstruction *ins = pos;
    LLVMInstruction *write = nullptr;
    LLVMInstruction *loopStart = nullptr;
    int level = 0;

    while (ins) {
        if (isLoopEnd(ins))
            level++;
        else if (isLoopStart(ins)) {
            level--;

            if (!loopStart)
                loopStart = ins;
        } else if (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == varPtr->var) {
            if (level <= 0) { // ignore nested loops (they're handled later)
                write = ins;
                break;
            }
        }

        ins = ins->previous;
    }

    if (loopStart) {
        // Analyze the first loop that was found
        if (variableTypeChangesInLoop(varPtr, loopStart, previousType))
            return write ? writeValueType(write) : Compiler::StaticType::Unknown;
    } else if (write) {
        // There wasn't any loop found, so we can just check the last write operation
        return writeValueType(write);
    }

    // No write operation found
    return previousType;
}

bool LLVMTypeAnalyzer::variableTypeChangesInLoop(LLVMVariablePtr *varPtr, LLVMInstruction *loopBody, Compiler::StaticType preLoopType) const
{
    if (!varPtr || !loopBody)
        return false;

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

    return variableTypeChangesInLoopFromEnd(varPtr, ins, preLoopType);
}

bool LLVMTypeAnalyzer::variableTypeChangesInLoopFromEnd(LLVMVariablePtr *varPtr, LLVMInstruction *loopEnd, Compiler::StaticType preLoopType) const
{
    // Find the last write instruction
    LLVMInstruction *ins = loopEnd->previous;

    while (ins && !isLoopStart(ins)) {
        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            // Nested loop or if statement
            if (variableTypeChangesInLoopFromEnd(varPtr, ins, preLoopType))
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

bool LLVMTypeAnalyzer::isLoopStart(LLVMInstruction *ins) const
{
    return (BEGIN_LOOP_INSTRUCTIONS.find(ins->type) != BEGIN_LOOP_INSTRUCTIONS.cend());
}

bool LLVMTypeAnalyzer::isLoopEnd(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndLoop);
}

bool LLVMTypeAnalyzer::isIfStart(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginIf);
}

bool LLVMTypeAnalyzer::isElse(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginElse);
}

bool LLVMTypeAnalyzer::isIfEnd(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndIf);
}

Compiler::StaticType LLVMTypeAnalyzer::optimizeRegisterType(LLVMRegister *reg) const
{
    // TODO: Move this method out if it's used in LLVMCodeBuilder too
    assert(reg);

    Compiler::StaticType ret = reg->type();

    // Optimize string constants that represent numbers
    if (reg->isConst() && reg->type() == Compiler::StaticType::String && reg->constValue().isValidNumber())
        ret = Compiler::StaticType::Number;

    return ret;
}

Compiler::StaticType LLVMTypeAnalyzer::writeValueType(LLVMInstruction *ins) const
{
    assert(ins);
    assert(!ins->args.empty());
    const auto arg = ins->args.back().second; // value is always the last argument in variable/list write instructions
    return optimizeRegisterType(arg);
}

bool LLVMTypeAnalyzer::typesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const
{
    // auto argIns = arg->instruction;

    // TODO: Handle cross-variable dependencies
    /*if (argIns && (argIns->type == LLVMInstruction::Type::ReadVariable || argIns->type == LLVMInstruction::Type::GetListItem))
        return isVarOrListTypeSafe(argIns.get(), expectedType, log, c);*/

    if (expectedType == Compiler::StaticType::Unknown) {
        // Equal unknown types are not considered a match
        return false;
    } else
        return (writeValueType(ins) == expectedType);
}
