// SPDX-License-Identifier: Apache-2.0

#include "variables.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmconstantregister.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Variables::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::CreateLocalVariable:
            ret.next = buildCreateLocalVariable(ins);
            break;

        case LLVMInstruction::Type::WriteLocalVariable:
            ret.next = buildWriteLocalVariable(ins);
            break;

        case LLVMInstruction::Type::ReadLocalVariable:
            ret.next = buildReadLocalVariable(ins);
            break;

        case LLVMInstruction::Type::WriteVariable:
            ret.next = buildWriteVariable(ins);
            break;

        case LLVMInstruction::Type::ReadVariable:
            ret.next = buildReadVariable(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Variables::buildCreateLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.empty());

    LLVMConstantRegister null(ins->functionReturnReg->type(), Value());
    ins->functionReturnReg->value = m_utils.createValue(&null);
    return ins->next;
}

LLVMInstruction *Variables::buildWriteLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *typeVar = m_utils.addAlloca(m_builder.getInt32Ty());
    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(m_utils.mapType(arg2.first))), typeVar);

    // TODO: Add integer support for local variables
    llvm::Value *isIntVar = m_utils.addAlloca(m_builder.getInt1Ty());
    llvm::Value *intVar = m_utils.addAlloca(m_builder.getInt64Ty());

    m_utils.createValueStore(arg1.second->value, typeVar, isIntVar, intVar, arg2.second, arg2.first, arg2.first);
    return ins->next;
}

LLVMInstruction *Variables::buildReadLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    ins->functionReturnReg->value = m_utils.castValue(arg.second, ins->functionReturnReg->type());
    return ins->next;
}

LLVMInstruction *Variables::buildWriteVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    Compiler::StaticType argType = m_utils.optimizeRegisterType(arg.second);
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->targetVariable);

    m_utils.createValueStore(varPtr.stackPtr, m_utils.getValueTypePtr(varPtr.stackPtr), varPtr.isInt, varPtr.intValue, arg.second, ins->targetType, argType);
    return ins->next;
}

LLVMInstruction *Variables::buildReadVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->targetVariable);

    ins->functionReturnReg->value = varPtr.stackPtr;
    ins->functionReturnReg->isInt = m_builder.CreateLoad(m_builder.getInt1Ty(), varPtr.isInt);
    ins->functionReturnReg->intValue = m_builder.CreateLoad(m_builder.getInt64Ty(), varPtr.intValue);
    return ins->next;
}
