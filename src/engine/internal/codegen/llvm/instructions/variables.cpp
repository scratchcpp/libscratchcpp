// SPDX-License-Identifier: Apache-2.0

#include "variables.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmconstantregister.h"
#include "../../optimizationutils.h"
#include "../../typeutils.h"

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
    LLVMLocalVariableInfo *info = ins->localVarInfo;

    info->isInt = m_utils.addAlloca(m_builder.getInt1Ty());
    info->intValue = m_utils.addAlloca(m_builder.getInt64Ty());
    m_builder.CreateStore(m_builder.getInt1(false), info->isInt);
    m_builder.CreateStore(llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true), info->isInt);

    LLVMConstantRegister null(ins->getFunctionReturnReg()->type(), Value());
    ins->getFunctionReturnReg()->value = m_utils.createValue(&null);
    return ins->getNext();
}

LLVMInstruction *Variables::buildWriteLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &args = ins->getArgs();
    const auto &arg1 = args[0];
    const auto &arg2 = args[1];
    LLVMLocalVariableInfo *info = ins->localVarInfo;
    llvm::Value *typeVar = m_utils.addAlloca(m_builder.getInt32Ty());
    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(TypeUtils::mapType(arg2.first))), typeVar);

    m_utils.createValueStore(arg1.second->value, typeVar, info->isInt, info->intValue, arg2.second, arg2.first, arg2.first);
    return ins->getNext();
}

LLVMInstruction *Variables::buildReadLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &args = ins->getArgs();
    const auto &arg = args[0];
    LLVMLocalVariableInfo *info = ins->localVarInfo;
    ins->getFunctionReturnReg()->value = m_utils.castValue(arg.second, ins->getFunctionReturnReg()->type());
    ins->getFunctionReturnReg()->isInt = m_builder.CreateLoad(m_builder.getInt1Ty(), info->isInt);
    ins->getFunctionReturnReg()->intValue = m_builder.CreateLoad(m_builder.getInt64Ty(), info->intValue);
    return ins->getNext();
}

LLVMInstruction *Variables::buildWriteVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &args = ins->getArgs();
    const auto &arg = args[0];
    Compiler::StaticType argType = OptimizationUtils::optimizeValueType(m_utils.compilerCtx(), arg.second);
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->targetVariable);

    m_utils.createValueStore(varPtr.stackPtr, m_utils.getValueTypePtr(varPtr.stackPtr), varPtr.isInt, varPtr.intValue, arg.second, ins->targetType, argType);
    m_builder.CreateStore(m_builder.getInt1(true), varPtr.changed);
    return ins->getNext();
}

LLVMInstruction *Variables::buildReadVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->targetVariable);

    ins->getFunctionReturnReg()->value = varPtr.stackPtr;
    ins->getFunctionReturnReg()->isInt = m_builder.CreateLoad(m_builder.getInt1Ty(), varPtr.isInt);
    ins->getFunctionReturnReg()->intValue = m_builder.CreateLoad(m_builder.getInt64Ty(), varPtr.intValue);
    return ins->getNext();
}
