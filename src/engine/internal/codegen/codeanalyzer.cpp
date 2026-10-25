// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilervalue.h>

#include "codeanalyzer.h"
#include "instructionlistbase.h"
#include "instructionbase.h"
#include "optimizationutils.h"

using namespace libscratchcpp;

// NOTE: The loop condition in repeat until and while loops is considered a part of the loop body
static const std::unordered_set<InstructionBase::Type> BEGIN_LOOP_INSTRUCTIONS = { InstructionBase::Type::BeginRepeatLoop, InstructionBase::Type::BeginLoopCondition };

static const std::unordered_set<InstructionBase::Type> LIST_WRITE_INSTRUCTIONS = { InstructionBase::Type::AppendToList, InstructionBase::Type::InsertToList, InstructionBase::Type::ListReplace };

CodeAnalyzer::CodeAnalyzer(CompilerContext *ctx) :
    m_ctx(ctx)
{
}

void CodeAnalyzer::analyzeScript(const InstructionListBase &script) const
{
    std::unordered_set<InstructionBase *> typeAssignedInstructions;
    std::vector<std::unique_ptr<Branch>> branches;
    InstructionBase *ins = script.first();

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

            assert(!currentBranch->isAtElse);
        } else if (isElse(ins)) {
            assert(!currentBranch->elseBranch);

            // Enter else branch with type information from the previous branch
            Branch *previousBranch = branches[branches.size() - 2].get();

            if (previousBranch->isAtElse) {
                previousBranch = previousBranch->elseBranch.get();
                assert(previousBranch);
            }

            currentBranch->elseBranch = std::make_unique<Branch>();
            currentBranch->isAtElse = true;
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

                if (previousBranch->isAtElse) {
                    previousBranch = previousBranch->elseBranch.get();
                    assert(previousBranch);
                }

                if (primaryBranch->typeChanges || (primaryBranch->elseBranch && primaryBranch->elseBranch->typeChanges))
                    previousBranch->typeChanges = true;

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

void CodeAnalyzer::updateVariableType(Branch *branch, InstructionBase *ins, std::unordered_set<InstructionBase *> &typeAssignedInstructions, bool isWrite) const
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

void CodeAnalyzer::updateListType(Branch *branch, InstructionBase *ins, std::unordered_set<InstructionBase *> &typeAssignedInstructions, bool isWrite) const
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

void CodeAnalyzer::mergeVariableTypes(Branch *branch, Branch *previousBranch) const
{
    for (const auto &[var, type] : branch->variableTypes) {
        auto it = previousBranch->variableTypes.find(var);

        if (it == previousBranch->variableTypes.cend())
            previousBranch->variableTypes[var] = Compiler::StaticType::Unknown;
        else
            it->second |= type;
    }
}

void CodeAnalyzer::overrideVariableTypes(Branch *branch, Branch *previousBranch) const
{
    for (const auto &[var, type] : branch->variableTypes)
        previousBranch->variableTypes[var] = type;
}

void CodeAnalyzer::mergeListTypes(Branch *branch, Branch *previousBranch, bool firstUnknown) const
{
    for (const auto &[list, type] : branch->listTypes) {
        auto it = previousBranch->listTypes.find(list);

        if (it == previousBranch->listTypes.cend())
            previousBranch->listTypes[list] = firstUnknown ? Compiler::StaticType::Unknown : type;
        else
            it->second |= type;
    }
}

bool CodeAnalyzer::isLoopStart(const InstructionBase *ins) const
{
    return (BEGIN_LOOP_INSTRUCTIONS.find(ins->type) != BEGIN_LOOP_INSTRUCTIONS.cend());
}

bool CodeAnalyzer::isLoopEnd(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::EndLoop);
}

bool CodeAnalyzer::isIfStart(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::BeginIf);
}

bool CodeAnalyzer::isElse(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::BeginElse);
}

bool CodeAnalyzer::isIfEnd(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::EndIf);
}

bool CodeAnalyzer::isVariableRead(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::ReadVariable);
}

bool CodeAnalyzer::isVariableWrite(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::WriteVariable);
}

bool CodeAnalyzer::isListRead(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::GetListItem);
}

bool CodeAnalyzer::isListWrite(const InstructionBase *ins) const
{
    return (LIST_WRITE_INSTRUCTIONS.find(ins->type) != LIST_WRITE_INSTRUCTIONS.cend());
}

bool CodeAnalyzer::isListClear(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::ClearList);
}

bool CodeAnalyzer::isProcedureCall(const InstructionBase *ins) const
{
    return (ins->type == InstructionBase::Type::CallProcedure);
}

Compiler::StaticType CodeAnalyzer::writeType(InstructionBase *ins) const
{
    assert(ins);
    assert(!ins->args.empty());
    auto &arg = ins->args.back(); // value is always the last argument in variable/list write instructions
    const CompilerValue *argValue = arg.second;

    Compiler::StaticType ret = OptimizationUtils::optimizeValueType(m_ctx, argValue);
    arg.first = ret; // store the return type in the value argument
    return ret;
}
