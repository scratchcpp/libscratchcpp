// SPDX-License-Identifier: Apache-2.0

#include "functions.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../../typeutils.h"

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
    m_utils.syncVariables();

    // Strings are returned through an output parameter
    llvm::Value *stringRet = nullptr;

    if (ins->getFunctionReturnReg() && ins->getFunctionReturnReg()->type() == Compiler::StaticType::String) {
        stringRet = m_utils.addStringAlloca();
        types.push_back(m_utils.getType(Compiler::StaticType::String, false));
        args.push_back(stringRet);
    }

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
    const auto &insArgs = ins->getArgs();

    for (auto &arg : insArgs) {
        types.push_back(m_utils.getType(arg.first, false));
        args.push_back(m_utils.castValue(arg.second, arg.first));
    }

    llvm::Type *retType = m_utils.getType(ins->getFunctionReturnReg() ? ins->getFunctionReturnReg()->type() : Compiler::StaticType::Void, true);
    llvm::Value *ret = m_builder.CreateCall(m_utils.functions().resolveFunction(ins->functionName, llvm::FunctionType::get(retType, types, false)), args);

    if (ins->getFunctionReturnReg()) {
        if (ins->getFunctionReturnReg()->type() == Compiler::StaticType::String)
            ins->getFunctionReturnReg()->value = stringRet;
        else if (TypeUtils::isSingleType(ins->getFunctionReturnReg()->type()))
            ins->getFunctionReturnReg()->value = ret;
        else {
            ins->getFunctionReturnReg()->value = m_utils.addAlloca(retType);
            m_builder.CreateStore(ret, ins->getFunctionReturnReg()->value);
        }
    }

    return ins->getNext();
}
