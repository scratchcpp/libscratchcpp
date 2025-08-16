// SPDX-License-Identifier: Apache-2.0

#include "functions.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Functions::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::FunctionCall:
            ret.next = buildFunctionCall(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Functions::buildFunctionCall(LLVMInstruction *ins)
{
    std::vector<llvm::Type *> types;
    std::vector<llvm::Value *> args;

    // Variables must be synchronized because the function can read them
    m_utils.syncVariables(m_utils.targetVariables());

    // Add execution context arg
    if (ins->functionCtxArg) {
        types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_utils.llvmCtx()), 0));
        args.push_back(m_utils.executionContextPtr());
    }

    // Add target pointer arg
    if (ins->functionTargetArg) {
        types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_utils.llvmCtx()), 0));
        args.push_back(m_utils.targetPtr());
    }

    // Args
    for (auto &arg : ins->args) {
        types.push_back(m_utils.getType(arg.first, false));
        args.push_back(m_utils.castValue(arg.second, arg.first));
    }

    llvm::Type *retType = m_utils.getType(ins->functionReturnReg ? ins->functionReturnReg->type() : Compiler::StaticType::Void, true);
    llvm::Value *ret = m_builder.CreateCall(m_utils.functions().resolveFunction(ins->functionName, llvm::FunctionType::get(retType, types, false)), args);

    if (ins->functionReturnReg) {
        if (m_utils.isSingleType(ins->functionReturnReg->type()))
            ins->functionReturnReg->value = ret;
        else {
            ins->functionReturnReg->value = m_utils.addAlloca(retType);
            m_builder.CreateStore(ret, ins->functionReturnReg->value);
        }

        if (ins->functionReturnReg->type() == Compiler::StaticType::String)
            m_utils.freeStringLater(ins->functionReturnReg->value);
    }

    return ins->next;
}
