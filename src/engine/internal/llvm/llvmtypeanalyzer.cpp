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
    LLVMInstruction *firstBranch = nullptr;
    LLVMInstruction *firstElseBranch = nullptr;
    int level = 0;

    while (ins) {
        if (isLoopEnd(ins) || isIfEnd(ins))
            level++;
        else if (isLoopStart(ins) || isIfStart(ins)) {
            level--;
            firstBranch = ins;
        } else if (isElse(ins)) {
            // Skip if branch if coming from else
            firstElseBranch = ins;
            ins = skipBranch(ins);
            continue;
        } else if (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == varPtr->var) {
            if (level <= 0) { // ignore nested branches (they're handled by the branch analyzer)
                write = ins;
                break;
            }
        }

        ins = ins->previous;
    }

    if (firstBranch) {
        // Analyze the first branch and else branch
        if (write)
            previousType = writeValueType(write); // write operation overrides previous type

        Compiler::StaticType firstBranchType = variableTypeAfterBranch(varPtr, firstBranch, previousType);
        Compiler::StaticType elseBranchType = variableTypeAfterBranch(varPtr, firstElseBranch, previousType);

        if (typesMatch(firstBranchType, elseBranchType))
            return firstBranchType;
        else
            return Compiler::StaticType::Unknown;
    } else if (write) {
        // There wasn't any branch found, so we can just check the last write operation
        return writeValueType(write);
    }

    // No write operation found
    return previousType;
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranch(LLVMVariablePtr *varPtr, LLVMInstruction *start, Compiler::StaticType previousType) const
{
    if (!varPtr || !start)
        return previousType;

    assert(isLoopStart(start) || isIfStart(start) || isElse(start));

    // Find loop/if statement end or else branch
    LLVMInstruction *ins = start->next;
    int level = 0;

    while (ins && !((isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) && level == 0)) {
        if (isLoopStart(ins) || isIfStart(ins))
            level++;
        else if (isLoopEnd(ins) || isIfEnd(ins)) {
            assert(level > 0);
            level--;
        }

        ins = ins->next;
    }

    if (!ins) {
        assert(false);
        return Compiler::StaticType::Unknown;
    }

    // Process the branch from end
    bool write = false; // only used internally (the compiler doesn't need this)
    return variableTypeAfterBranchFromEnd(varPtr, ins, previousType, write);
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranchFromEnd(LLVMVariablePtr *varPtr, LLVMInstruction *end, Compiler::StaticType previousType, bool &write) const
{
    // Find the last write instruction
    LLVMInstruction *ins = end->previous;
    bool typeMightReset = false;

    while (ins && !isLoopStart(ins) && !isIfStart(ins)) {
        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            // Process the nested loop or if statement
            Compiler::StaticType ret = variableTypeAfterBranchFromEnd(varPtr, ins, previousType, write);

            if (typesMatch(ret, previousType)) {
                if (write)
                    typeMightReset = true;
            } else
                return Compiler::StaticType::Unknown;

            ins = skipBranch(ins);

            if (isElse(ins)) {
                // Process if branch (the else branch is already processed)
                ret = variableTypeAfterBranchFromEnd(varPtr, ins, previousType, write);

                if (typesMatch(ret, previousType)) {
                    if (write) {
                        if (typeMightReset)
                            return previousType;

                        typeMightReset = true;
                    }
                } else
                    return Compiler::StaticType::Unknown;

                ins = skipBranch(ins);
            }
        } else if (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == varPtr->var) {
            // Variable write instruction
            Compiler::StaticType writeType = writeValueType(ins);
            write = true;

            if (typesMatch(writeType, previousType))
                return writeType;
            else
                return typeMightReset ? Compiler::StaticType::Unknown : writeType;
        }

        ins = ins->previous;
    }

    write = false;
    return previousType;
}

LLVMInstruction *LLVMTypeAnalyzer::skipBranch(LLVMInstruction *pos) const
{
    int level = 0;
    pos = pos->previous;

    while (pos && !((isLoopStart(pos) || isIfStart(pos)) && level == 0)) {
        if (isLoopStart(pos) || isIfStart(pos)) {
            assert(level > 0);
            level--;
        }

        if (isLoopEnd(pos) || isIfEnd(pos) || isElse(pos)) {
            if (isElse(pos) && level == 0)
                break;

            level++;
        }

        pos = pos->previous;
    };

    assert(pos);
    return pos;
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

bool LLVMTypeAnalyzer::typesMatch(Compiler::StaticType a, Compiler::StaticType b) const
{
    // Equal unknown types are not considered a match
    return (a == b) && (a != Compiler::StaticType::Unknown);
}

bool LLVMTypeAnalyzer::writeTypesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType) const
{
    // auto argIns = arg->instruction;

    // TODO: Handle cross-variable dependencies
    /*if (argIns && (argIns->type == LLVMInstruction::Type::ReadVariable || argIns->type == LLVMInstruction::Type::GetListItem))
        return isVarOrListTypeSafe(argIns.get(), expectedType, log, c);*/

    return typesMatch(writeValueType(ins), expectedType);
    if (expectedType == Compiler::StaticType::Unknown) {
        // Equal unknown types are not considered a match
        return false;
    } else
        return (writeValueType(ins) == expectedType);
}
