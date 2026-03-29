// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/blockprototype.h>

#include "procedures.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmcompilercontext.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Procedures::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::CallProcedure:
            ret.next = buildCallProcedure(ins);
            break;

        case LLVMInstruction::Type::ProcedureArg:
            ret.next = buildProcedureArg(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Procedures::buildCallProcedure(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->procedurePrototype);
    assert(ins->args.size() == ins->procedurePrototype->argumentTypes().size());
    m_utils.syncVariables();

    std::string name = m_utils.scriptFunctionName(ins->procedurePrototype);
    llvm::FunctionType *type = m_utils.scriptFunctionType(ins->procedurePrototype);
    std::vector<llvm::Value *> args;

    m_utils.compilerCtx()->addUsedProcedure(ins->procedurePrototype, name);

    llvm::FunctionType *funcType = m_utils.scriptFunctionType(nullptr);
    int passArgCount = funcType->getNumParams();

    for (size_t i = 0; i < passArgCount; i++)
        args.push_back(function->getArg(i));

    // Add warp arg
    if (m_utils.warp())
        args.push_back(m_builder.getInt1(true));
    else
        args.push_back(m_utils.procedurePrototype() ? m_utils.warpArg() : m_builder.getInt1(false));

    // Add procedure args
    for (const auto &arg : ins->args) {
        if (m_utils.isSingleType(arg.first))
            args.push_back(m_utils.castValue(arg.second, arg.first));
        else
            args.push_back(m_utils.createValue(arg.second));
    }

    // Call the procedure
    llvm::Value *handle = m_builder.CreateCall(m_utils.functions().resolveFunction(name, type), args);

    // Check for end thread sentinel value
    llvm::BasicBlock *nextBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::Value *endThread = m_builder.CreateICmpEQ(handle, m_utils.threadEndSentinel());
    m_builder.CreateCondBr(endThread, m_utils.endThreadBranch(), nextBranch);
    m_builder.SetInsertPoint(nextBranch);

    if (!m_utils.warp() && !ins->procedurePrototype->warp()) {
        // Handle suspend
        llvm::BasicBlock *suspendBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
        llvm::BasicBlock *nextBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
        m_builder.CreateCondBr(m_builder.CreateIsNull(handle), nextBranch, suspendBranch);

        m_builder.SetInsertPoint(suspendBranch);
        m_utils.createSuspend();
        llvm::Value *done = m_builder.CreateCall(m_utils.compilerCtx()->coroutineResumeFunction(), { handle });
        m_builder.CreateCondBr(done, nextBranch, suspendBranch);

        m_builder.SetInsertPoint(nextBranch);

        // The thread could be stopped from the coroutine
        llvm::BasicBlock *afterResumeBranch = llvm::BasicBlock::Create(llvmCtx, "", function);
        llvm::Value *isFinished = m_builder.CreateCall(m_utils.functions().resolve_llvm_is_thread_finished(), m_utils.executionContextPtr());
        m_builder.CreateCondBr(isFinished, m_utils.endThreadBranch(), afterResumeBranch);

        m_builder.SetInsertPoint(afterResumeBranch);
    }

    m_utils.reloadVariables();
    m_utils.reloadLists();
    return ins->next;
}

LLVMInstruction *Procedures::buildProcedureArg(LLVMInstruction *ins)
{
    assert(m_utils.procedurePrototype());
    llvm::FunctionType *funcType = m_utils.scriptFunctionType(nullptr);
    int passArgCount = funcType->getNumParams();
    llvm::Value *arg = m_utils.function()->getArg(passArgCount + 1 + ins->procedureArgIndex); // omit warp arg
    ins->functionReturnReg->value = arg;

    return ins->next;
}
