// SPDX-License-Identifier: Apache-2.0

#include "llvmcodeanalyzer.h"
#include "llvminstructionlist.h"
#include "llvminstruction.h"
#include "llvmbuildutils.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    BEGIN_LOOP_INSTRUCTIONS = { LLVMInstruction::Type::BeginRepeatLoop, LLVMInstruction::Type::BeginWhileLoop, LLVMInstruction::Type::BeginRepeatUntilLoop };

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
                    // The previous types can be ignored in if/else statements
                    overrideBranchTypes(primaryBranch, previousBranch);
                    mergeBranchTypes(primaryBranch->elseBranch.get(), previousBranch);
                } else
                    mergeBranchTypes(primaryBranch, previousBranch);

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

void LLVMCodeAnalyzer::mergeBranchTypes(Branch *branch, Branch *previousBranch) const
{
    // Variables
    for (const auto &[var, type] : branch->variableTypes) {
        auto it = previousBranch->variableTypes.find(var);

        if (it == previousBranch->variableTypes.cend())
            previousBranch->variableTypes[var] = type;
        else
            it->second |= type;
    }
}

void LLVMCodeAnalyzer::overrideBranchTypes(Branch *branch, Branch *previousBranch) const
{
    // Variables
    for (const auto &[var, type] : branch->variableTypes)
        previousBranch->variableTypes[var] = type;
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

Compiler::StaticType LLVMCodeAnalyzer::writeType(LLVMInstruction *ins) const
{
    assert(ins);
    assert(!ins->args.empty());
    auto &arg = ins->args.back(); // value is always the last argument in variable/list write instructions
    const LLVMRegister *argReg = arg.second;

    if (argReg->instruction) {
        // TODO: Handle list item
        if (isVariableRead(argReg->instruction.get())) {
            // Store the variable type in the value argument
            arg.first = argReg->instruction->functionReturnReg->type();
        }
    }

    return LLVMBuildUtils::optimizeRegisterType(argReg);
}
