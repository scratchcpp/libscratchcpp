// SPDX-License-Identifier: Apache-2.0

#include "string.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmcompilercontext.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult String::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::StringConcat:
            ret.next = buildStringConcat(ins);
            break;

        case LLVMInstruction::Type::StringChar:
            ret.next = buildStringChar(ins);
            break;

        case LLVMInstruction::Type::StringLength:
            ret.next = buildStringLength(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *String::buildStringConcat(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *str1 = m_utils.castValue(arg1.second, arg1.first);
    llvm::Value *str2 = m_utils.castValue(arg2.second, arg2.first);
    llvm::PointerType *charPointerType = m_builder.getInt16Ty()->getPointerTo();
    llvm::StructType *stringPtrType = m_utils.compilerCtx()->stringPtrType();
    llvm::Function *memcpyFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::memcpy_inline, { charPointerType, charPointerType, m_builder.getInt64Ty() });

    // StringPtr *result = (allocated string)
    llvm::Value *result = m_utils.addStringAlloca();

    // result->size = string1->size + string2->size
    llvm::Value *sizeField1 = m_builder.CreateStructGEP(stringPtrType, str1, 1);
    llvm::Value *sizeField2 = m_builder.CreateStructGEP(stringPtrType, str2, 1);
    llvm::Value *size1 = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField1);
    llvm::Value *size2 = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField2);
    llvm::Value *resultSize = m_builder.CreateAdd(size1, size2);
    llvm::Value *resultSizeField = m_builder.CreateStructGEP(stringPtrType, result, 1);
    m_builder.CreateStore(resultSize, resultSizeField);

    // string_alloc(result, result->size)
    m_builder.CreateCall(m_utils.functions().resolve_string_alloc(), { result, resultSize });

    // memcpy(result->data, string1->data, string1->size * sizeof(char16_t))
    llvm::Value *dataField1 = m_builder.CreateStructGEP(stringPtrType, str1, 0);
    llvm::Value *data1 = m_builder.CreateLoad(charPointerType, dataField1);
    llvm::Value *resultDataField = m_builder.CreateStructGEP(stringPtrType, result, 0);
    llvm::Value *writePtr = m_builder.CreateLoad(charPointerType, resultDataField);
    m_builder.CreateCall(memcpyFunc, { writePtr, data1, m_builder.CreateMul(size1, m_builder.getInt64(2)), m_builder.getInt1(false) });

    // memcpy(result->data + string1->size, string2->data, (string2->size + 1) * sizeof(char16_t))
    // +1: null-terminate
    llvm::Value *dataField2 = m_builder.CreateStructGEP(stringPtrType, str2, 0);
    llvm::Value *data2 = m_builder.CreateLoad(charPointerType, dataField2);
    writePtr = m_builder.CreateGEP(m_builder.getInt16Ty(), writePtr, size1);
    m_builder.CreateCall(memcpyFunc, { writePtr, data2, m_builder.CreateMul(m_builder.CreateAdd(size2, m_builder.getInt64(1)), m_builder.getInt64(2)), m_builder.getInt1(false) });

    ins->functionReturnReg->value = result;

    return ins->next;
}

LLVMInstruction *String::buildStringChar(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *str = m_utils.castValue(arg1.second, arg1.first);
    llvm::Value *index = m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Int);
    llvm::PointerType *charPointerType = m_builder.getInt16Ty()->getPointerTo();
    llvm::StructType *stringPtrType = m_utils.compilerCtx()->stringPtrType();

    // Get data ptr and size
    llvm::Value *dataField = m_builder.CreateStructGEP(stringPtrType, str, 0);
    llvm::Value *data = m_builder.CreateLoad(charPointerType, dataField);
    llvm::Value *sizeField = m_builder.CreateStructGEP(stringPtrType, str, 1);
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField);

    // Check range, get character ptr
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateICmpSGE(index, m_builder.getInt64(0)), m_builder.CreateICmpSLT(index, size));
    llvm::Value *charPtr = m_builder.CreateGEP(m_builder.getInt16Ty(), data, index);

    // Allocate string
    llvm::Value *result = m_utils.addStringAlloca();
    m_builder.CreateCall(m_utils.functions().resolve_string_alloc(), { result, m_builder.getInt64(1) }); // size 1 to avoid branching

    // Get result data ptr
    dataField = m_builder.CreateStructGEP(stringPtrType, result, 0);
    data = m_builder.CreateLoad(charPointerType, dataField);

    // Write result
    llvm::Value *char1 = m_builder.CreateGEP(m_builder.getInt16Ty(), data, m_builder.getInt64(0));
    llvm::Value *char2 = m_builder.CreateGEP(m_builder.getInt16Ty(), data, m_builder.getInt64(1));
    sizeField = m_builder.CreateStructGEP(stringPtrType, result, 1);
    m_builder.CreateStore(m_builder.CreateSelect(inRange, m_builder.CreateLoad(m_builder.getInt16Ty(), charPtr), m_builder.getInt16(0)), char1);
    m_builder.CreateStore(m_builder.getInt16(0), char2);
    m_builder.CreateStore(m_builder.CreateSelect(inRange, m_builder.getInt64(1), m_builder.getInt64(0)), sizeField);

    ins->functionReturnReg->value = result;

    return ins->next;
}

LLVMInstruction *String::buildStringLength(LLVMInstruction *ins)
{
    llvm::StructType *stringPtrType = m_utils.compilerCtx()->stringPtrType();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Value *str = m_utils.castValue(arg.second, arg.first);
    llvm::Value *sizeField = m_builder.CreateStructGEP(stringPtrType, str, 1);
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField);
    ins->functionReturnReg->value = m_builder.CreateSIToFP(size, m_builder.getDoubleTy());
    ins->functionReturnReg->intValue = size;
    ins->functionReturnReg->isInt = m_builder.getInt1(true);

    return ins->next;
}
