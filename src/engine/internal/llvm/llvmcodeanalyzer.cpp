// SPDX-License-Identifier: Apache-2.0

#include "llvmcodeanalyzer.h"
#include "llvminstructionlist.h"
#include "llvminstruction.h"
#include "llvmbuildutils.h"

using namespace libscratchcpp;

// NOTE: The loop condition in repeat until and while loops is considered a part of the loop body
static const std::unordered_set<LLVMInstruction::Type> BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginLoopCondition };

static const std::unordered_set<LLVMInstruction::Type> LIST_WRITE_INSTRUCTIONS = { LLVMInstruction::Type::AppendToList, LLVMInstruction::Type::InsertToList, LLVMInstruction::Type::ListReplace };

LLVMCodeAnalyzer::LLVMCodeAnalyzer(const LLVMBuildUtils &utils) :
    m_utils(utils)
{
}

void LLVMCodeAnalyzer::analyzeScript(const LLVMInstructionList &script) const
{
    std::unordered_set<LLVMInstruction *> typeAssignedInstructions;
    std::vector<std::unique_ptr<Branch>> branches;
    LLVMInstruction *ins = script.first();

    auto topBranch = std::make_unique<Branch>();
    topBranch->start = ins;
    Branch *topBranchPtr = topBranch.get();
    branches.push_back(std::move(topBranch));

    Branch *currentBranch = branches.back().get();

    while (ins) {
        if (isIfStart(ins) || isLoopStart(ins)) {
            auto branch = std::make_unique<Branch>();
            branch->start = ins;
            branch->variableTypes = currentBranch->variableTypes;
            branch->listTypes = currentBranch->listTypes;
            currentBranch = branch.get();
            branches.push_back(std::move(branch));
        } else if (isElse(ins)) {
            assert(!currentBranch->elseBranch);

            // Enter else branch with type information from the previous branch
            Branch *previousBranch = branches[branches.size() - 2].get();
            currentBranch->elseBranch = std::make_unique<Branch>();
            currentBranch = currentBranch->elseBranch.get();
            currentBranch->start = ins;
            currentBranch->variableTypes = previousBranch->variableTypes;
            currentBranch->listTypes = previousBranch->listTypes;
        } else if (isIfEnd(ins) || isLoopEnd(ins)) {
            if (isLoopEnd(ins) && currentBranch->typeChanges) {
                // Next iteration
                ins = currentBranch->start;
                currentBranch->typeChanges = false;
            } else {
                // Merge/override types
                Branch *previousBranch = branches[branches.size() - 2].get();
                Branch *primaryBranch = branches.back().get();

                assert(primaryBranch);

                if (primaryBranch && primaryBranch->elseBranch) {
                    // The previous variable types can be ignored in if/else statements
                    overrideVariableTypes(primaryBranch, previousBranch);
                    mergeListTypes(primaryBranch, previousBranch, false);

                    mergeVariableTypes(primaryBranch->elseBranch.get(), previousBranch);
                    mergeListTypes(primaryBranch->elseBranch.get(), previousBranch, true);
                } else {
                    mergeVariableTypes(primaryBranch, previousBranch);
                    mergeListTypes(primaryBranch, previousBranch, true);
                }

                // Remove the branch
                branches.pop_back();
                currentBranch = previousBranch;
            }
        } else if (isVariableWrite(ins)) {
            // Type before the write
            updateVariableType(currentBranch, ins, typeAssignedInstructions, true);

            // Type after the write
            currentBranch->variableTypes[ins->targetVariable] = writeType(ins);
        } else if (isVariableRead(ins)) {
            // Type before the read
            updateVariableType(currentBranch, ins, typeAssignedInstructions, false);

            // Store the type in the return register
            ins->functionReturnReg->setType(ins->targetType);
        } else if (isListClear(ins)) {
            // Type before the read
            updateListType(currentBranch, ins, typeAssignedInstructions, true);

            // Clear the list type
            currentBranch->listTypes[ins->targetList] = Compiler::StaticType::Void;
        } else if (isListWrite(ins)) {
            // Type before the write
            updateListType(currentBranch, ins, typeAssignedInstructions, true);

            // Type after the write
            currentBranch->listTypes[ins->targetList] |= writeType(ins);
        } else if (isListRead(ins)) {
            // Type before the read
            updateListType(currentBranch, ins, typeAssignedInstructions, false);

            // Store the type in the return register
            // NOTE: Get list item returns empty string if index is out of range
            ins->functionReturnReg->setType(ins->targetType | Compiler::StaticType::String);
        } else if (isProcedureCall(ins)) {
            // Variables/lists may change in procedures
            for (auto &[var, type] : currentBranch->variableTypes) {
                if (type != Compiler::StaticType::Unknown) {
                    type = Compiler::StaticType::Unknown;

                    if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend())
                        currentBranch->typeChanges = true;
                }
            }

            for (auto &[list, type] : currentBranch->listTypes) {
                if (type != Compiler::StaticType::Unknown) {
                    type = Compiler::StaticType::Unknown;

                    if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend()) {
                        typeAssignedInstructions.insert(ins);
                        currentBranch->typeChanges = true;
                    }
                }
            }

            typeAssignedInstructions.insert(ins);
        }

        ins = ins->next;
    }

    assert(branches.size() == 1);
    assert(branches.back().get() == topBranchPtr);
}

void LLVMCodeAnalyzer::updateVariableType(Branch *branch, LLVMInstruction *ins, std::unordered_set<LLVMInstruction *> &typeAssignedInstructions, bool isWrite) const
{
    auto it = branch->variableTypes.find(ins->targetVariable);

    if (it == branch->variableTypes.cend()) {
        if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend()) {
            if (isWrite)
                branch->typeChanges = true;

            typeAssignedInstructions.insert(ins);
        }
    } else {
        if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend()) {
            if (isWrite)
                branch->typeChanges = true;

            ins->targetType = it->second;
            typeAssignedInstructions.insert(ins);
        } else {
            if (isWrite && ((ins->targetType | it->second) != ins->targetType))
                branch->typeChanges = true;

            ins->targetType |= it->second;
        }
    }
}

void LLVMCodeAnalyzer::updateListType(Branch *branch, LLVMInstruction *ins, std::unordered_set<LLVMInstruction *> &typeAssignedInstructions, bool isWrite) const
{
    auto it = branch->listTypes.find(ins->targetList);

    if (it == branch->listTypes.cend()) {
        if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend()) {
            if (isWrite)
                branch->typeChanges = true;

            typeAssignedInstructions.insert(ins);
            ins->targetType = Compiler::StaticType::Unknown;
            branch->listTypes[ins->targetList] = ins->targetType;
        }
    } else {
        if (typeAssignedInstructions.find(ins) == typeAssignedInstructions.cend()) {
            if (isWrite)
                branch->typeChanges = true;

            ins->targetType = it->second;
            typeAssignedInstructions.insert(ins);
        } else {
            if (isWrite && ((ins->targetType | it->second) != ins->targetType))
                branch->typeChanges = true;

            ins->targetType |= it->second;
        }
    }
}

void LLVMCodeAnalyzer::mergeVariableTypes(Branch *branch, Branch *previousBranch) const
{
    for (const auto &[var, type] : branch->variableTypes) {
        auto it = previousBranch->variableTypes.find(var);

        if (it == previousBranch->variableTypes.cend())
            previousBranch->variableTypes[var] = Compiler::StaticType::Unknown;
        else
            it->second |= type;
    }
}

void LLVMCodeAnalyzer::overrideVariableTypes(Branch *branch, Branch *previousBranch) const
{
    for (const auto &[var, type] : branch->variableTypes)
        previousBranch->variableTypes[var] = type;
}

void LLVMCodeAnalyzer::mergeListTypes(Branch *branch, Branch *previousBranch, bool firstUnknown) const
{
    for (const auto &[list, type] : branch->listTypes) {
        auto it = previousBranch->listTypes.find(list);

        if (it == previousBranch->listTypes.cend())
            previousBranch->listTypes[list] = firstUnknown ? Compiler::StaticType::Unknown : type;
        else
            it->second |= type;
    }
}

bool LLVMCodeAnalyzer::isLoopStart(const LLVMInstruction *ins) const
{
    return (BEGIN_LOOP_INSTRUCTIONS.find(ins->type) != BEGIN_LOOP_INSTRUCTIONS.cend());
}

bool LLVMCodeAnalyzer::isLoopEnd(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndLoop);
}

bool LLVMCodeAnalyzer::isIfStart(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginIf);
}

bool LLVMCodeAnalyzer::isElse(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::BeginElse);
}

bool LLVMCodeAnalyzer::isIfEnd(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::EndIf);
}

bool LLVMCodeAnalyzer::isVariableRead(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::ReadVariable);
}

bool LLVMCodeAnalyzer::isVariableWrite(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::WriteVariable);
}

bool LLVMCodeAnalyzer::isListRead(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::GetListItem);
}

bool LLVMCodeAnalyzer::isListWrite(const LLVMInstruction *ins) const
{
    return (LIST_WRITE_INSTRUCTIONS.find(ins->type) != LIST_WRITE_INSTRUCTIONS.cend());
}

bool LLVMCodeAnalyzer::isListClear(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::ClearList);
}

bool LLVMCodeAnalyzer::isProcedureCall(const LLVMInstruction *ins) const
{
    return (ins->type == LLVMInstruction::Type::CallProcedure);
}

Compiler::StaticType LLVMCodeAnalyzer::writeType(LLVMInstruction *ins) const
{
    assert(ins);
    assert(!ins->args.empty());
    auto &arg = ins->args.back(); // value is always the last argument in variable/list write instructions
    const LLVMRegister *argReg = arg.second;

    if (argReg->instruction) {
        if (isVariableRead(argReg->instruction.get()) || isListRead(argReg->instruction.get())) {
            // Store the variable/list type in the value argument
            arg.first = argReg->instruction->functionReturnReg->type();
        }
    }

    return m_utils.optimizeRegisterType(argReg);
}
