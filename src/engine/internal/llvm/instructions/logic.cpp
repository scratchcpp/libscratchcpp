// SPDX-License-Identifier: Apache-2.0

#include "logic.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Logic::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::And:
            ret.next = buildAnd(ins);
            break;

        case LLVMInstruction::Type::Or:
            ret.next = buildOr(ins);
            break;

        case LLVMInstruction::Type::Not:
            ret.next = buildNot(ins);
            break;

        default:
            ret.match = false;
            ret.next = (ins);
            break;
    }

    return ret;
}

LLVMInstruction *Logic::buildAnd(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *bool1 = m_utils.castValue(arg1.second, arg1.first);
    llvm::Value *bool2 = m_utils.castValue(arg2.second, arg2.first);
    ins->functionReturnReg->value = m_builder.CreateAnd(bool1, bool2);

    return ins->next;
}

LLVMInstruction *Logic::buildOr(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *bool1 = m_utils.castValue(arg1.second, arg1.first);
    llvm::Value *bool2 = m_utils.castValue(arg2.second, arg2.first);
    ins->functionReturnReg->value = m_builder.CreateOr(bool1, bool2);

    return ins->next;
}

LLVMInstruction *Logic::buildNot(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Value *value = m_utils.castValue(arg.second, arg.first);
    ins->functionReturnReg->value = m_builder.CreateNot(value);

    return ins->next;
}
