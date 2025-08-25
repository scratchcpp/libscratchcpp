// SPDX-License-Identifier: Apache-2.0

#include "variables.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

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
    llvm::Type *type = nullptr;

    switch (ins->functionReturnReg->type()) {
        case Compiler::StaticType::Number:
            type = m_builder.getDoubleTy();
            break;

        case Compiler::StaticType::Bool:
            type = m_builder.getInt1Ty();
            break;

        case Compiler::StaticType::String:
            std::cerr << "error: local variables do not support string type" << std::endl;
            break;

        case Compiler::StaticType::Pointer:
            std::cerr << "error: local variables do not support pointer type" << std::endl;
            break;

        default:
            assert(false);
            break;
    }

    ins->functionReturnReg->value = m_utils.addAlloca(type);
    return ins->next;
}

LLVMInstruction *Variables::buildWriteLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *converted = m_utils.castValue(arg2.second, arg2.first);
    m_builder.CreateStore(converted, arg1.second->value);
    return ins->next;
}

LLVMInstruction *Variables::buildReadLocalVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Type *type = nullptr;

    switch (ins->functionReturnReg->type()) {
        case Compiler::StaticType::Number:
            type = m_builder.getDoubleTy();
            break;

        case Compiler::StaticType::Bool:
            type = m_builder.getInt1Ty();
            break;

        default:
            assert(false);
            break;
    }

    ins->functionReturnReg->value = m_builder.CreateLoad(type, arg.second->value);
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
