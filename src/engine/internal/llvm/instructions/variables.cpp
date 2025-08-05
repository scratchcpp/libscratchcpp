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
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->workVariable);
    varPtr.changed = true; // TODO: Handle loops and if statements

    Compiler::StaticType varType = Compiler::StaticType::Unknown;

    if (m_utils.warp())
        varType = m_utils.typeAnalyzer().variableType(ins->workVariable, ins, Compiler::StaticType::Unknown);

    // Initialize stack variable on first assignment
    // TODO: Use stack in the top level (outside loops and if statements)
    /*if (!varPtr.onStack) {
        varPtr.onStack = true;
        varPtr.type = type; // don't care about unknown type on first assignment

                      ValueType mappedType;

                        if (type == Compiler::StaticType::String || type == Compiler::StaticType::Unknown) {
                            // Value functions are used for these types, so don't break them
                            mappedType = ValueType::Number;
                        } else {
                            auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [type](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == type; });
                            assert(it != TYPE_MAP.cend());
                            mappedType = it->first;
                        }

                        llvm::Value *typeField = m_builder.CreateStructGEP(m_valueDataType, varPtr.stackPtr, 1);
                        m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typeField);
                    }*/

    m_utils.createValueStore(arg.second, varPtr.stackPtr, argType, varType);
    return ins->next;
}

LLVMInstruction *Variables::buildReadVariable(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->workVariable);
    Compiler::StaticType type = Compiler::StaticType::Unknown;

    if (m_utils.warp())
        type = m_utils.typeAnalyzer().variableType(ins->workVariable, ins, Compiler::StaticType::Unknown);

    ins->functionReturnReg->value = varPtr.onStack && !(ins->loopCondition && !m_utils.warp()) ? varPtr.stackPtr : varPtr.heapPtr;
    ins->functionReturnReg->setType(type);
    return ins->next;
}
