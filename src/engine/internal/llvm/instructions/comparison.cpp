// SPDX-License-Identifier: Apache-2.0

#include "comparison.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Comparison::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::CmpEQ:
            ret.next = buildCmpEQ(ins);
            break;

        case LLVMInstruction::Type::CmpGT:
            ret.next = buildCmpGT(ins);
            break;

        case LLVMInstruction::Type::CmpLT:
            ret.next = buildCmpLT(ins);
            break;

        case LLVMInstruction::Type::StrCmpEQCS:
            ret.next = buildStrCmpEQCS(ins);
            break;

        case LLVMInstruction::Type::StrCmpEQCI:
            ret.next = buildStrCmpEQCI(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Comparison::buildCmpEQ(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0].second;
    const auto &arg2 = ins->args[1].second;
    ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::EQ);

    return ins->next;
}

LLVMInstruction *Comparison::buildCmpGT(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0].second;
    const auto &arg2 = ins->args[1].second;
    ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::GT);

    return ins->next;
}

LLVMInstruction *Comparison::buildCmpLT(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0].second;
    const auto &arg2 = ins->args[1].second;
    ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::LT);

    return ins->next;
}

LLVMInstruction *Comparison::buildStrCmpEQCS(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0].second;
    const auto &arg2 = ins->args[1].second;
    ins->functionReturnReg->value = m_utils.createStringComparison(arg1, arg2, true);

    return ins->next;
}

LLVMInstruction *Comparison::buildStrCmpEQCI(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0].second;
    const auto &arg2 = ins->args[1].second;
    ins->functionReturnReg->value = m_utils.createStringComparison(arg1, arg2, false);

    return ins->next;
}
