// SPDX-License-Identifier: Apache-2.0

#include "llvmtypeanalyzer.h"
#include "llvminstruction.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginWhileLoop, LLVMInstruction::Type::BeginRepeatUntilLoop };

static const std::unordered_set<LLVMInstruction::Type> LIST_WRITE_INSTRUCTIONS = { LLVMInstruction::Type::AppendToList, LLVMInstruction::Type::InsertToList, LLVMInstruction::Type::ListReplace };

Compiler::StaticType LLVMTypeAnalyzer::variableType(const Variable *var, const LLVMInstruction *pos, Compiler::StaticType previousType) const
{
    InstructionSet visitedInstructions;
    std::unordered_map<const List *, Compiler::StaticType> listTypes;
    return variableType(var, pos, previousType, listTypes, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranch(const Variable *var, const LLVMInstruction *start, Compiler::StaticType previousType) const
{
    InstructionSet visitedInstructions;
    return variableTypeAfterBranch(var, start, previousType, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::listType(const List *list, const LLVMInstruction *pos, Compiler::StaticType previousType, bool isEmpty) const
{
    InstructionSet visitedInstructions;
    std::unordered_map<const List *, Compiler::StaticType> listTypes;
    return listType(list, pos, previousType, isEmpty, listTypes, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::listTypeAfterBranch(const List *list, const LLVMInstruction *start, Compiler::StaticType previousType, bool isEmpty) const
{
    bool write = false; // only used internally (the compiler doesn't need this)
    InstructionSet visitedInstructions;
    std::unordered_map<const List *, Compiler::StaticType> listTypes;
    return listTypeAfterBranch(list, start, previousType, isEmpty, nullptr, write, listTypes, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::variableType(
    const Variable *var,
    const LLVMInstruction *pos,
    Compiler::StaticType previousType,
    std::unordered_map<const List *, Compiler::StaticType> listTypes,
    InstructionSet &visitedInstructions) const
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
    const LLVMInstruction *ins = pos->previous;
    const LLVMInstruction *write = nullptr;
    std::pair<const LLVMInstruction *, int> firstBranch = { nullptr, 0 };
    std::pair<const LLVMInstruction *, int> firstElseBranch = { nullptr, 0 };
    const LLVMInstruction *ourBranch = nullptr;
    int level = 0;

    while (ins) {
        if (isLoopEnd(ins) || isIfEnd(ins))
            level++;
        else if (isLoopStart(ins) || isIfStart(ins)) {
            if (!ourBranch && level == 0)
                ourBranch = ins;

            if (!firstBranch.first || level < firstBranch.second)
                firstBranch = { ins, level };

            level--;
        } else if (isElse(ins)) {
            // Skip if branch if coming from else
            if (!firstElseBranch.first || level < firstElseBranch.second)
                firstElseBranch = { ins, level };

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

    if (firstBranch.first) {
        // Analyze the first branch and else branch
        bool ignoreWriteAfterPos = (isIfStart(firstBranch.first) && firstBranch.first == ourBranch);

        if (write)
            previousType = writeValueType(write, listTypes, visitedInstructions); // write operation overrides previous type

        Compiler::StaticType firstBranchType = previousType;
        Compiler::StaticType elseBranchType = previousType;

        if (!ignoreWriteAfterPos) {
            firstBranchType = variableTypeAfterBranch(var, firstBranch.first, previousType, visitedInstructions);
            elseBranchType = variableTypeAfterBranch(var, firstElseBranch.first, previousType, visitedInstructions);
        }

        if (typesMatch(firstBranchType, elseBranchType))
            return firstBranchType;
        else
            return Compiler::StaticType::Unknown;
    } else if (write) {
        // There wasn't any branch found, so we can just check the last write operation
        return writeValueType(write, listTypes, visitedInstructions);
    }

    // No write operation found
    return previousType;
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranch(const Variable *var, const LLVMInstruction *start, Compiler::StaticType previousType, InstructionSet &visitedInstructions) const
{
    if (!var || !start)
        return previousType;

    const LLVMInstruction *end = branchEnd(start);

    if (!end)
        return Compiler::StaticType::Unknown;

    // Process the branch from end
    bool write = false; // only used internally (the compiler doesn't need this)
    std::unordered_map<const List *, Compiler::StaticType> listTypes;
    return variableTypeAfterBranchFromEnd(var, end, previousType, write, listTypes, visitedInstructions);
}

Compiler::StaticType LLVMTypeAnalyzer::variableTypeAfterBranchFromEnd(
    const Variable *var,
    const LLVMInstruction *end,
    Compiler::StaticType previousType,
    bool &write,
    std::unordered_map<const List *, Compiler::StaticType> listTypes,
    InstructionSet &visitedInstructions) const
{
    // Find the last write instruction
    const LLVMInstruction *ins = end->previous;
    bool typeMightReset = false;

    while (ins && !isLoopStart(ins) && !isIfStart(ins)) {
        if (isLoopEnd(ins) || isIfEnd(ins) || isElse(ins)) {
            // Process the nested loop or if statement
            Compiler::StaticType ret = variableTypeAfterBranchFromEnd(var, ins, previousType, write, listTypes, visitedInstructions);

            if (typesMatch(ret, previousType)) {
                if (write)
                    typeMightReset = true;
            } else
                return Compiler::StaticType::Unknown;

            ins = branchStart(ins);

            if (isElse(ins)) {
                // Process if branch (the else branch is already processed)
                ret = variableTypeAfterBranchFromEnd(var, ins, previousType, write, listTypes, visitedInstructions);

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
            Compiler::StaticType writeType = writeValueType(ins, listTypes, visitedInstructions);
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

Compiler::StaticType LLVMTypeAnalyzer::listType(
    const List *list,
    const LLVMInstruction *pos,
    Compiler::StaticType previousType,
    bool isEmpty,
    std::unordered_map<const List *, Compiler::StaticType> listTypes,
    InstructionSet &visitedInstructions) const
{
    if (!list || !pos)
        return Compiler::StaticType::Unknown;

    /*
     * If the given instruction has already been processed,
     * it means there's a circular dependency with an unknown type.
     * See variableType()
     */
    if (visitedInstructions.find(pos) != visitedInstructions.cend()) {
        // Circular dependencies are rare (and bad) so don't optimize them
        // TODO: The (previousType != Compiler::StaticType::Unknown) case isn't handled in tests
        // Add a test case for it if you find it...
        assert(previousType == Compiler::StaticType::Unknown);
        return /*Compiler::StaticType::Unknown*/ previousType;
    }

    visitedInstructions.insert(pos);
    listTypes[list] = previousType;

    const LLVMInstruction *ins = pos;
    const LLVMInstruction *previous = nullptr;
    std::pair<const LLVMInstruction *, int> firstBranch = { nullptr, 0 };
    std::pair<const LLVMInstruction *, int> lastClear = { nullptr, 0 };
    int level = 0;

    // Find a start instruction (list clear in the top level or the first instruction)
    while (ins) {
        if (isLoopEnd(ins) || isIfEnd(ins))
            level++;
        else if (isLoopStart(ins) || isIfStart(ins) || isElse(ins)) {
            if (!isElse(ins))
                level--;

            if (!firstBranch.first || level < firstBranch.second)
                firstBranch = { ins, level };
        } else if (isListClear(ins, list)) {
            if (!lastClear.first || level < lastClear.second)
                lastClear = { ins, level };
        }

        previous = ins;
        ins = ins->previous;
    }

    if (firstBranch.first) {
        assert(firstBranch.second == level);

        // The first branch must be above the query point level
        if (firstBranch.second == 0)
            firstBranch.first = nullptr;
    }

    // Clear must be in the top level
    if (lastClear.second != level)
        lastClear.first = nullptr;

    if (lastClear.first) {
        ins = lastClear.first;
        isEmpty = true;
    } else
        ins = previous;

    // Process from the start instruction
    while (ins && ins != pos) {
        if (isLoopStart(ins) || isIfStart(ins)) {
            do {
                bool write;
                Compiler::StaticType type = listTypeAfterBranch(list, ins, previousType, isEmpty, pos, write, listTypes, visitedInstructions);

                // If this branch contains the query point, return the final type
                if (ins == firstBranch.first)
                    return type;

                // If there was a write, the list is no longer empty
                if (write) {
                    isEmpty = false;

                    // The write could change the type
                    if (!typesMatch(type, previousType))
                        previousType = type;
                }

                // Skip the branch
                ins = branchEnd(ins);
            } while (isElse(ins)); // handle else branch
        } else if (isListWrite(ins, list)) {
            // List write instruction
            Compiler::StaticType writeType = writeValueType(ins, listTypes, visitedInstructions);

            if (!handleListWrite(writeType, previousType, isEmpty))
                return Compiler::StaticType::Unknown;
        }

        ins = ins->next;
    }

    assert(ins);
    return previousType;
}

Compiler::StaticType LLVMTypeAnalyzer::listTypeAfterBranch(
    const List *list,
    const LLVMInstruction *start,
    Compiler::StaticType previousType,
    bool isEmpty,
    const LLVMInstruction *query,
    bool &write,
    std::unordered_map<const List *, Compiler::StaticType> listTypes,
    InstructionSet &visitedInstructions) const
{
    write = false;

    if (!list || !start)
        return previousType;

    assert(isLoopStart(start) || isIfStart(start) || isElse(start));

    bool isLoop = isLoopStart(start);
    bool clearBeforeQueryPoint = false;
    const LLVMInstruction *ins = start->next;

    while (ins && !(isLoopEnd(ins) || isIfEnd(ins) || isElse(ins))) {
        if (isLoopStart(ins) || isIfStart(ins)) {
            do {
                Compiler::StaticType type = listTypeAfterBranch(list, ins, previousType, isEmpty, query, write, listTypes, visitedInstructions);

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
            Compiler::StaticType writeType = writeValueType(ins, listTypes, visitedInstructions);
            write = true;

            if (!handleListWrite(writeType, previousType, isEmpty))
                return Compiler::StaticType::Unknown;
        } else if (isListClear(ins, list)) {
            // The list is now empty
            isEmpty = true;
            clearBeforeQueryPoint = true;
            write = false; // the write variable is only used to check if the list is still empty
        } else if (ins == query) {
            if (!isLoop || clearBeforeQueryPoint)
                break;

            clearBeforeQueryPoint = false;
        }

        ins = ins->next;
    }

    assert(ins);
    return previousType;
}

bool LLVMTypeAnalyzer::handleListWrite(Compiler::StaticType writeType, Compiler::StaticType &previousType, bool &isEmpty) const
{
    if (isEmpty) {
        // In empty lists, writes of the same type determine the final type
        // This is the first write found, it might determine the final type
        previousType = writeType;

        // The list is no longer empty
        isEmpty = false;
    } else if (!typesMatch(writeType, previousType)) {
        // For non-empty lists, we can just check the write type
        return false;
    }

    return true;
}

const LLVMInstruction *LLVMTypeAnalyzer::branchEnd(const LLVMInstruction *start) const
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

const LLVMInstruction *LLVMTypeAnalyzer::branchStart(const LLVMInstruction *end) const
{
    assert(end);
    assert(isLoopEnd(end) || isIfEnd(end) || isElse(end));

    // Find loop/if statement/else branch start
    LLVMInstruction *ins = end->previous;
    int level = 0;

    while (ins && !((isLoopStart(ins) || isIfStart(ins) || isElse(ins)) && level == 0)) {
        if (isLoopStart(ins) || isIfStart(ins)) {
            assert(level > 0);
            level--;
        }

        if (isLoopEnd(ins) || isIfEnd(ins))
            level++;

        ins = ins->previous;
    };

    assert(ins);
    return ins;
}

bool LLVMTypeAnalyzer::isLoopStart(const LLVMInstruction *ins) const
{
    return (BEGIN_LOOP_INSTRUCTIONS.find(ins->type) != BEGIN_LOOP_INSTRUCTIONS.cend());
}

bool LLVMTypeAnalyzer::isLoopEnd(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndLoop);
}

bool LLVMTypeAnalyzer::isIfStart(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginIf);
}

bool LLVMTypeAnalyzer::isElse(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginElse);
}

bool LLVMTypeAnalyzer::isIfEnd(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndIf);
}

bool LLVMTypeAnalyzer::isVariableRead(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::ReadVariable);
}

bool LLVMTypeAnalyzer::isVariableWrite(const LLVMInstruction *ins, const Variable *var) const
{
    return (ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == var);
}

bool LLVMTypeAnalyzer::isListRead(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::GetListItem);
}

bool LLVMTypeAnalyzer::isListWrite(const LLVMInstruction *ins, const List *list) const
{
    return (LIST_WRITE_INSTRUCTIONS.find(ins->type) != LIST_WRITE_INSTRUCTIONS.cend() && ins->workList == list);
}

bool LLVMTypeAnalyzer::isListClear(const LLVMInstruction *ins, const List *list) const
{
    return (ins->type == LLVMInstruction::Type::ClearList && ins->workList == list);
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

bool LLVMTypeAnalyzer::isWriteNoOp(const LLVMInstruction *ins) const
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

Compiler::StaticType LLVMTypeAnalyzer::writeValueType(const LLVMInstruction *ins, std::unordered_map<const List *, Compiler::StaticType> listTypes, InstructionSet &visitedInstructions) const
{
    assert(ins);
    assert(!ins->args.empty());
    const auto arg = ins->args.back().second; // value is always the last argument in variable/list write instructions

    if (arg->instruction) {
        // TODO: Handle list item
        if (isVariableRead(arg->instruction.get())) {
            // If this is a variable read instruction, recursively get the variable type
            return variableType(arg->instruction->workVariable, arg->instruction.get(), Compiler::StaticType::Unknown, listTypes, visitedInstructions);
        } else if (isListRead(arg->instruction.get())) {
            // If this is a list read instruction, recursively get the list type
            Compiler::StaticType type = Compiler::StaticType::Unknown;
            auto it = listTypes.find(arg->instruction->workList);

            if (it != listTypes.cend())
                type = it->second;

            // NOTE: The isEmpty parameter is useless here
            return listType(arg->instruction->workList, arg->instruction.get(), type, false, listTypes, visitedInstructions);
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

bool LLVMTypeAnalyzer::
    writeTypesMatch(const LLVMInstruction *ins, Compiler::StaticType expectedType, std::unordered_map<const List *, Compiler::StaticType> listTypes, InstructionSet &visitedInstructions) const
{
    return typesMatch(writeValueType(ins, listTypes, visitedInstructions), expectedType);
}
