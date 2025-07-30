// SPDX-License-Identifier: Apache-2.0

#include "llvmtypeanalyzer.h"
#include "llvminstruction.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginWhileLoop, LLVMInstruction::Type::BeginRepeatUntilLoop };

static const std::unordered_set<LLVMInstruction::Type> LIST_WRITE_INSTRUCTIONS = { LLVMInstruction::Type::AppendToList, LLVMInstruction::Type::InsertToList, LLVMInstruction::Type::ListReplace };

Compiler::StaticType LLVMTypeAnalyzer::variableType(Variable *var, LLVMInstruction *pos, Compiler::StaticType previousType) const
{
    InstructionSet visitedInstructions;
    return variableType(var, pos, previousType, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranch(Variable *var, LLVMInstruction *start, Compiler::StaticType previousType) const
{
    InstructionSet visitedInstructions;
    return variableTypeAfterBranch(var, start, previousType, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::listTypeAfterBranch(List *list, LLVMInstruction *start, Compiler::StaticType previousType, bool isEmpty) const
{
    if (!list || !start)
        return previousType;

    bool write = false; // only used internally (the compiler doesn't need this)
    return listTypeAfterBranch(list, start, previousType, isEmpty, write);
}

Compiler::StaticType LLVMTypeAnalyzer::variableType(Variable *var, LLVMInstruction *pos, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const
{
    if (!var || !pos)
        return Compiler::StaticType::Unknown;

    /*
     * If the given instruction has already been processed,
     * it means there's a case like this:
     * x = x
     *
     * or this:
     * x = y
     * ...
     * y = x
     */
    if (visitedInstructions.find(pos) != visitedInstructions.cend()) {
        // Circular dependencies are rare (and bad) so don't optimize them
        return Compiler::StaticType::Unknown;
    }

    visitedInstructions.insert(pos);

    // Check the last write operation before the instruction
    LLVMInstruction *ins = pos;
    LLVMInstruction *write = nullptr;
    LLVMInstruction *firstBranch = nullptr;
    LLVMInstruction *firstElseBranch = nullptr;
    LLVMInstruction *ourBranch = nullptr;
    int level = 0;

    while (ins) {
        if (isLoopEnd(ins) || isIfEnd(ins))
            level++;
        else if (isLoopStart(ins) || isIfStart(ins)) {
            if (!ourBranch && level == 0)
                ourBranch = ins;

            level--;
            firstBranch = ins;
        } else if (isElse(ins)) {
            // Skip if branch if coming from else
            firstElseBranch = ins;
            ins = branchStart(ins);
            continue;
        } else if (isVariableWrite(ins, var) && !isWriteNoOp(ins)) {
            if (level <= 0) { // ignore nested branches (they're handled by the branch analyzer)
                write = ins;
                break;
            }
        }

        ins = ins->previous;
    }

    if (firstBranch) {
        // Analyze the first branch and else branch
        bool ignoreWriteAfterPos = (isIfStart(firstBranch) && firstBranch == ourBranch);

        if (write)
            previousType = writeValueType(write, visitedInstructions); // write operation overrides previous type

        Compiler::StaticType firstBranchType = previousType;
        Compiler::StaticType elseBranchType = previousType;

        if (!ignoreWriteAfterPos) {
            firstBranchType = variableTypeAfterBranch(var, firstBranch, previousType, visitedInstructions);
            elseBranchType = variableTypeAfterBranch(var, firstElseBranch, previousType, visitedInstructions);
        }

        if (typesMatch(firstBranchType, elseBranchType))
            return firstBranchType;
        else
            return Compiler::StaticType::Unknown;
    } else if (write) {
        // There wasn't any branch found, so we can just check the last write operation
        return writeValueType(write, visitedInstructions);
    }

    // No write operation found
    return previousType;
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranch(Variable *var, LLVMInstruction *start, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const
{
    if (!var || !start)
        return previousType;

    LLVMInstruction *end = branchEnd(start);

    if (!end)
        return Compiler::StaticType::Unknown;

    // Process the branch from end
    bool write = false; // only used internally (the compiler doesn't need this)
    return variableTypeAfterBranchFromEnd(var, end, previousType, write, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranchFromEnd(Variable *var, LLVMInstruction *end, Compiler::StaticType previousType, bool &write, InstructionSet &visitedInstructions) const
{
    // Find the last write instruction
    LLVMInstruction *ins = end->previous;
    bool typeMightReset = false;

    while (ins && !isLoopStart(ins) && !isIfStart(ins)) {
        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            // Process the nested loop or if statement
            Compiler::StaticType ret = variableTypeAfterBranchFromEnd(var, ins, previousType, write, visitedInstructions);

            if (typesMatch(ret, previousType)) {
                if (write)
                    typeMightReset = true;
            } else
                return Compiler::StaticType::Unknown;

            ins = branchStart(ins);

            if (isElse(ins)) {
                // Process if branch (the else branch is already processed)
                ret = variableTypeAfterBranchFromEnd(var, ins, previousType, write, visitedInstructions);

                if (typesMatch(ret, previousType)) {
                    if (write) {
                        if (typeMightReset)
                            return previousType;

                        typeMightReset = true;
                    }
                } else
                    return Compiler::StaticType::Unknown;

                ins = branchStart(ins);
            }
        } else if (isVariableWrite(ins, var) && !isWriteNoOp(ins)) {
            // Variable write instruction
            Compiler::StaticType writeType = writeValueType(ins, visitedInstructions);
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

Compiler::StaticType LLVMTypeAnalyzer::listTypeAfterBranch(List *list, LLVMInstruction *start, Compiler::StaticType previousType, bool isEmpty, bool &write) const
{
    assert(isLoopStart(start) || isIfStart(start) || isElse(start));

    InstructionSet visitedInstructions; // TODO: Handle cross-variable/list dependencies

    LLVMInstruction *ins = start->next;
    write = false;

    while (ins && !(isLoopEnd(ins) || isIfEnd(ins) || isElse(ins))) {
        if (isLoopStart(ins) || isIfStart(ins)) {
            do {
                Compiler::StaticType type = listTypeAfterBranch(list, ins, previousType, isEmpty, write);

                // If there was a write, the list is no longer empty
                if (write) {
                    isEmpty = false;

                    // The write could change the type
                    if (!typesMatch(type, previousType))
                        return Compiler::StaticType::Unknown;
                }

                // Skip the branch
                ins = branchEnd(ins);
            } while (isElse(ins)); // handle else branch
        } else if (isListWrite(ins, list)) {
            // List write instruction
            Compiler::StaticType writeType = writeValueType(ins, visitedInstructions);
            write = true;

            if (isEmpty) {
                // In empty lists, writes of the same type determine the final type
                // This is the first write found, it might determine the final type
                previousType = writeType;

                // The list is no longer empty
                isEmpty = false;
            } else if (!typesMatch(writeType, previousType)) {
                // For non-empty lists, we can just check the write type
                return Compiler::StaticType::Unknown;
            }
        } else if (ins->type == LLVMInstruction::Type::ClearList && ins->workList == list) {
            // The list is now empty
            isEmpty = true;
            write = false; // the write variable is only used to check if the list is still empty
        }

        ins = ins->next;
    }

    assert(ins);
    return previousType;
}

LLVMInstruction *LLVMTypeAnalyzer::branchEnd(LLVMInstruction *start) const
{
    assert(start);
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

    assert(ins);
    return ins;
}

LLVMInstruction *LLVMTypeAnalyzer::branchStart(LLVMInstruction *end) const
{
    assert(end);
    assert(isLoopEnd(end) || isIfEnd(end) || isElse(end));

    // Find loop/if statement/else branch start
    LLVMInstruction *ins = end->previous;
    int level = 0;

    while (ins && !((isLoopStart(ins) || isIfStart(ins)) && level == 0)) {
        if (isLoopStart(ins) || isIfStart(ins)) {
            assert(level > 0);
            level--;
        }

        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            if (isElse(ins) && level == 0)
                break;

            level++;
        }

        ins = ins->previous;
    };

    assert(ins);
    return ins;
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

bool LLVMTypeAnalyzer::isVariableRead(LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::ReadVariable);
}

bool LLVMTypeAnalyzer::isVariableWrite(LLVMInstruction *ins, Variable *var) const
{
    return (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == var);
}

bool LLVMTypeAnalyzer::isListWrite(LLVMInstruction *ins, List *list) const
{
    return (LIST_WRITE_INSTRUCTIONS.find(ins->type) != LIST_WRITE_INSTRUCTIONS.cend() && ins->workList == list);
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

bool LLVMTypeAnalyzer::isWriteNoOp(LLVMInstruction *ins) const
{
    assert(ins);
    assert(!ins->args.empty());
    const auto arg = ins->args.back().second; // value is always the last argument in variable/list write instructions

    if (arg->instruction) {
        // TODO: Handle list item
        if (isVariableRead(arg->instruction.get())) {
            // Self-assignment is a no-op
            return (ins->workVariable == arg->instruction->workVariable);
        }
    }

    return false;
}

Compiler::StaticType LLVMTypeAnalyzer::writeValueType(LLVMInstruction *ins, InstructionSet &visitedInstructions) const
{
    assert(ins);
    assert(!ins->args.empty());
    const auto arg = ins->args.back().second; // value is always the last argument in variable/list write instructions

    if (arg->instruction) {
        // TODO: Handle list item
        if (isVariableRead(arg->instruction.get())) {
            // If this is a variable read instruction, recursively get the variable type
            return variableType(arg->instruction->workVariable, arg->instruction.get(), Compiler::StaticType::Unknown, visitedInstructions);
        } else {
            // The write argument already has the instruction return type
            return optimizeRegisterType(arg);
        }
    } else
        return optimizeRegisterType(arg);
}

bool LLVMTypeAnalyzer::typesMatch(Compiler::StaticType a, Compiler::StaticType b) const
{
    // Equal unknown types are not considered a match
    return (a == b) && (a != Compiler::StaticType::Unknown);
}

bool LLVMTypeAnalyzer::writeTypesMatch(LLVMInstruction *ins, Compiler::StaticType expectedType, InstructionSet &visitedInstructions) const
{
    return typesMatch(writeValueType(ins, visitedInstructions), expectedType);
}
