// SPDX-License-Identifier: Apache-2.0

#include "control.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Control::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::Select:
            ret.next = buildSelect(ins);
            break;

        case LLVMInstruction::Type::Yield:
            ret.next = buildYield(ins);
            break;

        case LLVMInstruction::Type::BeginIf:
            ret.next = buildBeginIf(ins);
            break;

        case LLVMInstruction::Type::BeginElse:
            ret.next = buildBeginElse(ins);
            break;

        case LLVMInstruction::Type::EndIf:
            ret.next = buildEndIf(ins);
            break;

        case LLVMInstruction::Type::BeginRepeatLoop:
            ret.next = buildBeginRepeatLoop(ins);
            break;

        case LLVMInstruction::Type::LoopIndex:
            ret.next = buildLoopIndex(ins);
            break;

        case LLVMInstruction::Type::BeginWhileLoop:
            ret.next = buildBeginWhileLoop(ins);
            break;

        case LLVMInstruction::Type::BeginRepeatUntilLoop:
            ret.next = buildBeginRepeatUntilLoop(ins);
            break;

        case LLVMInstruction::Type::BeginLoopCondition:
            ret.next = buildBeginLoopCondition(ins);
            break;

        case LLVMInstruction::Type::EndLoop:
            ret.next = buildEndLoop(ins);
            break;

        case LLVMInstruction::Type::Stop:
            ret.next = buildStop(ins);
            break;

        case LLVMInstruction::Type::StopWithoutSync:
            ret.next = buildStopWithoutSync(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Control::buildSelect(LLVMInstruction *ins)
{
    assert(ins->args.size() == 3);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    const auto &arg3 = ins->args[2];
    auto type = arg2.first;
    llvm::Value *cond = m_utils.castValue(arg1.second, arg1.first);
    llvm::Value *trueValue;
    llvm::Value *falseValue;

    if (m_utils.isSingleType(type)) {
        trueValue = m_utils.castValue(arg2.second, type);
        falseValue = m_utils.castValue(arg3.second, type);
    } else {
        trueValue = m_utils.createValue(arg2.second);
        falseValue = m_utils.createValue(arg3.second);
    }

    ins->functionReturnReg->value = m_builder.CreateSelect(cond, trueValue, falseValue);
    ins->functionReturnReg->isInt = m_builder.CreateSelect(cond, arg2.second->isInt, arg3.second->isInt);
    ins->functionReturnReg->intValue = m_builder.CreateSelect(cond, arg2.second->intValue, arg3.second->intValue);
    return ins->next;
}

LLVMInstruction *Control::buildYield(LLVMInstruction *ins)
{
    m_utils.createSuspend();
    return ins->next;
}

LLVMInstruction *Control::buildBeginIf(LLVMInstruction *ins)
{
    LLVMIfStatement statement;
    statement.beforeIf = m_builder.GetInsertBlock();
    statement.body = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());

    // Use last reg
    assert(ins->args.size() == 1);
    const auto &reg = ins->args[0];
    assert(reg.first == Compiler::StaticType::Bool);
    statement.condition = m_utils.castValue(reg.second, reg.first);

    // Switch to body branch
    m_builder.SetInsertPoint(statement.body);

    m_utils.ifStatements().push_back(statement);
    m_utils.pushScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildBeginElse(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(!m_utils.ifStatements().empty());
    LLVMIfStatement &statement = m_utils.ifStatements().back();

    // Jump to the branch after the if statement
    assert(!statement.afterIf);
    statement.afterIf = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_utils.freeScopeHeap();
    m_builder.CreateBr(statement.afterIf);

    // Create else branch
    assert(!statement.elseBranch);
    statement.elseBranch = llvm::BasicBlock::Create(llvmCtx, "", function);

    // Since there's an else branch, the conditional instruction should jump to it
    m_builder.SetInsertPoint(statement.beforeIf);
    m_builder.CreateCondBr(statement.condition, statement.body, statement.elseBranch);

    // Switch to the else branch
    m_builder.SetInsertPoint(statement.elseBranch);

    return ins->next;
}

LLVMInstruction *Control::buildEndIf(LLVMInstruction *ins)
{
    auto &ifStatements = m_utils.ifStatements();
    assert(!ifStatements.empty());
    LLVMIfStatement &statement = ifStatements.back();
    m_utils.freeScopeHeap();

    // Jump to the branch after the if statement
    if (!statement.afterIf)
        statement.afterIf = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());

    m_builder.CreateBr(statement.afterIf);

    if (statement.elseBranch) {
    } else {
        // If there wasn't an 'else' branch, create a conditional instruction which skips the if statement if false
        m_builder.SetInsertPoint(statement.beforeIf);
        m_builder.CreateCondBr(statement.condition, statement.body, statement.afterIf);
    }

    // Switch to the branch after the if statement
    m_builder.SetInsertPoint(statement.afterIf);

    ifStatements.pop_back();
    m_utils.popScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildBeginRepeatLoop(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    LLVMLoop loop;
    loop.isRepeatLoop = true;

    // index = 0
    llvm::Constant *zero = llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true);
    loop.index = m_utils.addAlloca(m_builder.getInt64Ty());
    m_builder.CreateStore(zero, loop.index);

    // Create branches
    llvm::BasicBlock *roundBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
    loop.conditionBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
    loop.afterLoop = llvm::BasicBlock::Create(llvmCtx, "", function);

    // Use last reg for count
    assert(ins->args.size() == 1);
    const auto &reg = ins->args[0];
    assert(reg.first == Compiler::StaticType::Number);
    llvm::Value *count = m_utils.castValue(reg.second, reg.first);
    llvm::Value *isInf = m_builder.CreateFCmpOEQ(count, llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false));

    // Clamp count if <= 0 (we can skip the loop if count is not positive)
    llvm::Value *comparison = m_builder.CreateFCmpULE(count, llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0)));
    m_builder.CreateCondBr(comparison, loop.afterLoop, roundBranch);

    // Round (Scratch-specific behavior)
    m_builder.SetInsertPoint(roundBranch);
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::round, { count->getType() });
    count = m_builder.CreateCall(roundFunc, { count });
    count = m_builder.CreateFPToUI(count, m_builder.getInt64Ty()); // cast to unsigned integer
    count = m_builder.CreateSelect(isInf, zero, count);

    // Jump to condition branch
    m_builder.CreateBr(loop.conditionBranch);

    // Check index
    m_builder.SetInsertPoint(loop.conditionBranch);

    llvm::BasicBlock *body = llvm::BasicBlock::Create(llvmCtx, "", function);

    if (!loop.afterLoop)
        loop.afterLoop = llvm::BasicBlock::Create(llvmCtx, "", function);

    llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
    comparison = m_builder.CreateOr(isInf, m_builder.CreateICmpULT(currentIndex, count));
    m_builder.CreateCondBr(comparison, body, loop.afterLoop);

    // Switch to body branch
    m_builder.SetInsertPoint(body);

    m_utils.loops().push_back(loop);
    m_utils.pushScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildLoopIndex(LLVMInstruction *ins)
{
    assert(!m_utils.loops().empty());
    LLVMLoop &loop = m_utils.loops().back();
    llvm::Value *index = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
    ins->functionReturnReg->value = m_builder.CreateUIToFP(index, m_builder.getDoubleTy());
    ins->functionReturnReg->intValue = index;
    ins->functionReturnReg->isInt = m_builder.getInt1(true);

    return ins->next;
}

LLVMInstruction *Control::buildBeginWhileLoop(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(!m_utils.loops().empty());
    LLVMLoop &loop = m_utils.loops().back();

    // Create branches
    llvm::BasicBlock *body = llvm::BasicBlock::Create(llvmCtx, "", function);
    loop.afterLoop = llvm::BasicBlock::Create(llvmCtx, "", function);

    // Use last reg
    assert(ins->args.size() == 1);
    const auto &reg = ins->args[0];
    assert(reg.first == Compiler::StaticType::Bool);
    llvm::Value *condition = m_utils.castValue(reg.second, reg.first);
    m_builder.CreateCondBr(condition, body, loop.afterLoop);

    // Switch to body branch
    m_builder.SetInsertPoint(body);
    m_utils.pushScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildBeginRepeatUntilLoop(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(!m_utils.loops().empty());
    LLVMLoop &loop = m_utils.loops().back();

    // Create branches
    llvm::BasicBlock *body = llvm::BasicBlock::Create(llvmCtx, "", function);
    loop.afterLoop = llvm::BasicBlock::Create(llvmCtx, "", function);

    // Use last reg
    assert(ins->args.size() == 1);
    const auto &reg = ins->args[0];
    assert(reg.first == Compiler::StaticType::Bool);
    llvm::Value *condition = m_utils.castValue(reg.second, reg.first);
    m_builder.CreateCondBr(condition, loop.afterLoop, body);

    // Switch to body branch
    m_builder.SetInsertPoint(body);
    m_utils.pushScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildBeginLoopCondition(LLVMInstruction *ins)
{
    LLVMLoop loop;
    loop.isRepeatLoop = false;
    loop.conditionBranch = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());
    m_builder.CreateBr(loop.conditionBranch);
    m_builder.SetInsertPoint(loop.conditionBranch);
    m_utils.loops().push_back(loop);

    return ins->next;
}

LLVMInstruction *Control::buildEndLoop(LLVMInstruction *ins)
{
    auto &loops = m_utils.loops();
    assert(!loops.empty());
    LLVMLoop &loop = loops.back();

    if (loop.isRepeatLoop) {
        // Increment index
        llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
        llvm::Value *incremented = m_builder.CreateAdd(currentIndex, llvm::ConstantInt::get(m_builder.getInt64Ty(), 1, false));
        m_builder.CreateStore(incremented, loop.index);
    }

    // Jump to the condition branch
    m_utils.freeScopeHeap();
    m_builder.CreateBr(loop.conditionBranch);

    // Switch to the branch after the loop
    m_builder.SetInsertPoint(loop.afterLoop);

    loops.pop_back();
    m_utils.popScopeLevel();

    return ins->next;
}

LLVMInstruction *Control::buildStop(LLVMInstruction *ins)
{
    m_utils.syncVariables();
    return buildStopWithoutSync(ins);
}

LLVMInstruction *Control::buildStopWithoutSync(LLVMInstruction *ins)
{
    m_utils.freeScopeHeap();
    m_builder.CreateBr(m_utils.endBranch());
    llvm::BasicBlock *nextBranch = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());
    m_builder.SetInsertPoint(nextBranch);

    return ins->next;
}
