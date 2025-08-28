// SPDX-License-Identifier: Apache-2.0

#include "math.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Math::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::Add:
            ret.next = buildAdd(ins);
            break;

        case LLVMInstruction::Type::Sub:
            ret.next = buildSub(ins);
            break;

        case LLVMInstruction::Type::Mul:
            ret.next = buildMul(ins);
            break;

        case LLVMInstruction::Type::Div:
            ret.next = buildDiv(ins);
            break;

        case LLVMInstruction::Type::Random:
            ret.next = buildRandom(ins);
            break;

        case LLVMInstruction::Type::RandomInt:
            ret.next = buildRandomInt(ins);
            break;

        case LLVMInstruction::Type::Mod:
            ret.next = buildMod(ins);
            break;

        case LLVMInstruction::Type::Round:
            ret.next = buildRound(ins);
            break;

        case LLVMInstruction::Type::Abs:
            ret.next = buildAbs(ins);
            break;

        case LLVMInstruction::Type::Floor:
            ret.next = buildFloor(ins);
            break;

        case LLVMInstruction::Type::Ceil:
            ret.next = buildCeil(ins);
            break;

        case LLVMInstruction::Type::Sqrt:
            ret.next = buildSqrt(ins);
            break;

        case LLVMInstruction::Type::Sin:
            ret.next = buildSin(ins);
            break;

        case LLVMInstruction::Type::Cos:
            ret.next = buildCos(ins);
            break;

        case LLVMInstruction::Type::Tan:
            ret.next = buildTan(ins);
            break;

        case LLVMInstruction::Type::Asin:
            ret.next = buildAsin(ins);
            break;

        case LLVMInstruction::Type::Acos:
            ret.next = buildAcos(ins);
            break;

        case LLVMInstruction::Type::Atan:
            ret.next = buildAtan(ins);
            break;

        case LLVMInstruction::Type::Ln:
            ret.next = buildLn(ins);
            break;

        case LLVMInstruction::Type::Log10:
            ret.next = buildLog10(ins);
            break;

        case LLVMInstruction::Type::Exp:
            ret.next = buildExp(ins);
            break;

        case LLVMInstruction::Type::Exp10:
            ret.next = buildExp10(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Math::buildAdd(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];

    llvm::Value *double1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Double));
    llvm::Value *double2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Double));
    ins->functionReturnReg->value = m_builder.CreateFAdd(double1, double2);

    llvm::Value *int1 = m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Int);
    llvm::Value *int2 = m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Int);
    ins->functionReturnReg->isInt = m_builder.CreateAnd(arg1.second->isInt, arg2.second->isInt);
    ins->functionReturnReg->intValue = m_builder.CreateAdd(int1, int2);

    return ins->next;
}

LLVMInstruction *Math::buildSub(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];

    llvm::Value *double1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Double));
    llvm::Value *double2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Double));
    ins->functionReturnReg->value = m_builder.CreateFSub(double1, double2);

    llvm::Value *int1 = m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Int);
    llvm::Value *int2 = m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Int);
    ins->functionReturnReg->isInt = m_builder.CreateAnd(arg1.second->isInt, arg2.second->isInt);
    ins->functionReturnReg->intValue = m_builder.CreateSub(int1, int2);

    return ins->next;
}

LLVMInstruction *Math::buildMul(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];

    llvm::Value *double1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Double));
    llvm::Value *double2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Double));
    ins->functionReturnReg->value = m_builder.CreateFMul(double1, double2);

    llvm::Value *int1 = m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Int);
    llvm::Value *int2 = m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Int);
    ins->functionReturnReg->isInt = m_builder.CreateAnd(arg1.second->isInt, arg2.second->isInt);
    ins->functionReturnReg->intValue = m_builder.CreateMul(int1, int2);

    return ins->next;
}

LLVMInstruction *Math::buildDiv(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    ins->functionReturnReg->value = m_builder.CreateFDiv(num1, num2);

    return ins->next;
}

LLVMInstruction *Math::buildRandom(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    LLVMRegister *reg1 = arg1.second;
    LLVMRegister *reg2 = arg2.second;

    if (reg1->type() == Compiler::StaticType::Bool && reg2->type() == Compiler::StaticType::Bool) {
        llvm::Value *bool1 = m_utils.castValue(arg1.second, Compiler::StaticType::Bool);
        llvm::Value *bool2 = m_utils.castValue(arg2.second, Compiler::StaticType::Bool);
        ins->functionReturnReg->value = m_builder.CreateCall(m_utils.functions().resolve_llvm_random_bool(), { m_utils.executionContextPtr(), bool1, bool2 });
    } else {
        llvm::Constant *inf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
        llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, Compiler::StaticType::Number));
        llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, Compiler::StaticType::Number));
        llvm::Value *sum = m_builder.CreateFAdd(num1, num2);
        llvm::Value *sumDiv = m_builder.CreateFDiv(sum, inf);
        llvm::Value *isInfOrNaN = m_utils.isNaN(sumDiv);

        // NOTE: The random function will be called even in edge cases where it isn't needed, but they're rare, so it shouldn't be an issue
        if (reg1->type() == Compiler::StaticType::Number && reg2->type() == Compiler::StaticType::Number)
            ins->functionReturnReg->value =
                m_builder.CreateSelect(isInfOrNaN, sum, m_builder.CreateCall(m_utils.functions().resolve_llvm_random_double(), { m_utils.executionContextPtr(), num1, num2 }));
        else {
            llvm::Value *value1 = m_utils.createValue(reg1);
            llvm::Value *value2 = m_utils.createValue(reg2);
            ins->functionReturnReg->value = m_builder.CreateSelect(isInfOrNaN, sum, m_builder.CreateCall(m_utils.functions().resolve_llvm_random(), { m_utils.executionContextPtr(), value1, value2 }));
        }
    }

    return ins->next;
}

LLVMInstruction *Math::buildRandomInt(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *from = m_builder.CreateFPToSI(m_utils.castValue(arg1.second, arg1.first), m_builder.getInt64Ty());
    llvm::Value *to = m_builder.CreateFPToSI(m_utils.castValue(arg2.second, arg2.first), m_builder.getInt64Ty());
    llvm::Value *intValue = m_builder.CreateCall(m_utils.functions().resolve_llvm_random_int64(), { m_utils.executionContextPtr(), from, to });
    ins->functionReturnReg->value = m_builder.CreateSIToFP(intValue, m_builder.getDoubleTy());
    ins->functionReturnReg->intValue = intValue;
    ins->functionReturnReg->isInt = m_builder.getInt1(true);

    return ins->next;
}

LLVMInstruction *Math::buildMod(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];

    // double: rem(a, b) / b < 0.0 ? rem(a, b) + b : rem(a, b)
    llvm::Constant *doubleZero = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Value *double1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *double2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    llvm::Value *doubleRem = m_builder.CreateFRem(double1, double2);                                         // rem(a, b)
    llvm::Value *doubleCond = m_builder.CreateFCmpOLT(m_builder.CreateFDiv(doubleRem, double2), doubleZero); // rem(a, b) / b < 0.0
    ins->functionReturnReg->value = m_builder.CreateSelect(doubleCond, m_builder.CreateFAdd(doubleRem, double2), doubleRem);

    // int: b == 0 ? 0 (double fallback) : ((rem(a, b) < 0) != (b < 0) ? rem(a, b) + b : rem(a, b))
    llvm::Constant *intZero = llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true);
    llvm::Value *int1 = m_utils.castValue(arg1.second, arg1.first, LLVMBuildUtils::NumberType::Int);
    llvm::Value *int2 = m_utils.castValue(arg2.second, arg2.first, LLVMBuildUtils::NumberType::Int);
    llvm::Value *nanResult = m_builder.CreateICmpEQ(int2, intZero);

    llvm::BasicBlock *nanBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());
    llvm::BasicBlock *intBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "", m_utils.function());
    m_builder.CreateCondBr(nanResult, nanBlock, intBlock);

    m_builder.SetInsertPoint(nanBlock);
    llvm::Value *noInt = m_builder.getInt1(false);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(intBlock);
    llvm::Value *isInt = m_builder.CreateAnd(arg1.second->isInt, arg2.second->isInt);
    llvm::Value *intRem = m_builder.CreateSRem(int1, int2);                                      // rem(a, b)
    llvm::Value *intCond = m_builder.CreateICmpSLT(m_builder.CreateSDiv(intRem, int2), intZero); // rem(a, b) / b < 0
    llvm::Value *intResult = m_builder.CreateSelect(intCond, m_builder.CreateAdd(intRem, int2), intRem);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);

    llvm::PHINode *resultPhi = m_builder.CreatePHI(m_builder.getInt64Ty(), 2);
    resultPhi->addIncoming(intZero, nanBlock);
    resultPhi->addIncoming(intResult, intBlock);

    llvm::PHINode *isIntPhi = m_builder.CreatePHI(m_builder.getInt1Ty(), 2);
    isIntPhi->addIncoming(noInt, nanBlock);
    isIntPhi->addIncoming(isInt, intBlock);

    ins->functionReturnReg->intValue = resultPhi;
    ins->functionReturnReg->isInt = isIntPhi;

    return ins->next;
}

LLVMInstruction *Math::buildRound(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Module *module = m_utils.module();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];

    // double: x >= 0.0 ? round(x) : (x >= -0.5 ? -0.0 : floor(x + 0.5))
    llvm::Constant *zero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Constant *negativeZero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-0.0));
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::round, m_builder.getDoubleTy());
    llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::floor, m_builder.getDoubleTy());
    llvm::Value *doubleValue = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    llvm::Value *notNegative = m_builder.CreateFCmpOGE(doubleValue, zero);                                                                               // num >= 0.0
    llvm::Value *roundNum = m_builder.CreateCall(roundFunc, doubleValue);                                                                                // round(num)
    llvm::Value *negativeCond = m_builder.CreateFCmpOGE(doubleValue, llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-0.5)));                               // num >= -0.5
    llvm::Value *negativeRound = m_builder.CreateCall(floorFunc, m_builder.CreateFAdd(doubleValue, llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.5)))); // floor(x + 0.5)
    ins->functionReturnReg->value = m_builder.CreateSelect(notNegative, roundNum, m_builder.CreateSelect(negativeCond, negativeZero, negativeRound));

    // int: doubleX == inf || doubleX == -inf ? doubleX : intX
    llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
    llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
    llvm::Value *isInt = arg.second->isInt;
    llvm::Value *intValue = arg.second->intValue;
    llvm::Value *isNotInf = m_builder.CreateAnd(m_builder.CreateFCmpONE(doubleValue, posInf), m_builder.CreateFCmpONE(doubleValue, negInf));
    llvm::Value *cast = m_builder.CreateFPToSI(ins->functionReturnReg->value, m_builder.getInt64Ty());
    ins->functionReturnReg->isInt = isNotInf;
    ins->functionReturnReg->intValue = m_builder.CreateSelect(isInt, intValue, cast);

    return ins->next;
}

LLVMInstruction *Math::buildAbs(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];

    llvm::Function *fabsFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::fabs, m_builder.getDoubleTy());
    llvm::Value *doubleValue = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(fabsFunc, doubleValue);

    llvm::Function *absFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::abs, m_builder.getInt64Ty());
    llvm::Value *intValue = arg.second->intValue;
    ins->functionReturnReg->isInt = arg.second->isInt;
    ins->functionReturnReg->intValue = m_builder.CreateCall(absFunc, { intValue, m_builder.getInt1(false) });

    return ins->next;
}

LLVMInstruction *Math::buildFloor(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];

    // double: floor(doubleX)
    llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::floor, m_builder.getDoubleTy());
    llvm::Value *doubleValue = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(floorFunc, doubleValue);

    // int: doubleX == inf || doubleX == -inf ? doubleX : intX
    llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
    llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
    llvm::Value *isInt = arg.second->isInt;
    llvm::Value *intValue = arg.second->intValue;
    llvm::Value *isNotInf = m_builder.CreateAnd(m_builder.CreateFCmpONE(doubleValue, posInf), m_builder.CreateFCmpONE(doubleValue, negInf));
    llvm::Value *cast = m_builder.CreateFPToSI(ins->functionReturnReg->value, m_builder.getInt64Ty());
    ins->functionReturnReg->isInt = isNotInf;
    ins->functionReturnReg->intValue = m_builder.CreateSelect(isInt, intValue, cast);

    return ins->next;
}

LLVMInstruction *Math::buildCeil(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];

    // double: ceil(doubleX)
    llvm::Function *ceilFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::ceil, m_builder.getDoubleTy());
    llvm::Value *doubleValue = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(ceilFunc, doubleValue);

    // int: doubleX == inf || doubleX == -inf ? doubleX : intX
    llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
    llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
    llvm::Value *isInt = arg.second->isInt;
    llvm::Value *intValue = arg.second->intValue;
    llvm::Value *isNotInf = m_builder.CreateAnd(m_builder.CreateFCmpONE(doubleValue, posInf), m_builder.CreateFCmpONE(doubleValue, negInf));
    llvm::Value *cast = m_builder.CreateFPToSI(ins->functionReturnReg->value, m_builder.getInt64Ty());
    ins->functionReturnReg->isInt = isNotInf;
    ins->functionReturnReg->intValue = m_builder.CreateSelect(isInt, intValue, cast);

    return ins->next;
}

LLVMInstruction *Math::buildSqrt(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // sqrt(x) + 0.0
    // This avoids negative zero
    llvm::Constant *zero = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Function *sqrtFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::sqrt, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateCall(sqrtFunc, num), zero);

    return ins->next;
}

LLVMInstruction *Math::buildSin(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Module *module = m_utils.module();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // round(sin(x * pi / 180.0) * 1e10) / 1e10 + 0.0
    // +0.0 to avoid -0.0
    llvm::Constant *zero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Constant *factor = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(1e10));
    llvm::Function *sinFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::sin, m_builder.getDoubleTy());
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::round, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    llvm::Value *sinResult = m_builder.CreateCall(sinFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // sin(x * pi / 180)
    llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(sinResult, factor));                    // round(sin(x * 180) * 1e10)
    ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(rounded, factor), zero);

    return ins->next;
}

LLVMInstruction *Math::buildCos(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Module *module = m_utils.module();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // round(cos(x * pi / 180.0) * 1e10) / 1e10
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Constant *factor = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(1e10));
    llvm::Function *cosFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::cos, m_builder.getDoubleTy());
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::round, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    llvm::Value *cosResult = m_builder.CreateCall(cosFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // cos(x * pi / 180)
    llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(cosResult, factor));                    // round(cos(x * 180) * 1e10)
    ins->functionReturnReg->value = m_builder.CreateFDiv(rounded, factor);

    return ins->next;
}

LLVMInstruction *Math::buildTan(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Module *module = m_utils.module();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // ((mod = rem(x, 360.0)) == -270.0 || mod == 90.0) ? inf : ((mod == -90.0 || mod == 270.0) ? -inf : round(tan(x * pi / 180.0) * 1e10) / 1e10 + 0.0)
    // +0.0 to avoid -0.0
    llvm::Constant *zero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Constant *full = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(360.0));
    llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
    llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
    llvm::Constant *undefined1 = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-270.0));
    llvm::Constant *undefined2 = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(90.0));
    llvm::Constant *undefined3 = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-90.0));
    llvm::Constant *undefined4 = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(270.0));
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Constant *factor = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(1e10));
    llvm::Function *tanFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::tan, m_builder.getDoubleTy());
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::round, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    llvm::Value *mod = m_builder.CreateFRem(num, full);
    llvm::Value *isUndefined1 = m_builder.CreateFCmpOEQ(mod, undefined1);                                               // rem(x, 360.0) == -270.0
    llvm::Value *isUndefined2 = m_builder.CreateFCmpOEQ(mod, undefined2);                                               // rem(x, 360.0) == 90.0
    llvm::Value *isUndefined3 = m_builder.CreateFCmpOEQ(mod, undefined3);                                               // rem(x, 360.0) == -90.0
    llvm::Value *isUndefined4 = m_builder.CreateFCmpOEQ(mod, undefined4);                                               // rem(x, 360.0) == 270.0
    llvm::Value *tanResult = m_builder.CreateCall(tanFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // tan(x * pi / 180)
    llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(tanResult, factor));                    // round(tan(x * 180) * 1e10)
    llvm::Value *result = m_builder.CreateFAdd(m_builder.CreateFDiv(rounded, factor), zero);                            // round(tan(x * pi / 180.0) * 1e10) / 1e10 + 0.0
    llvm::Value *inner = m_builder.CreateSelect(m_builder.CreateOr(isUndefined3, isUndefined4), negInf, result);
    ins->functionReturnReg->value = m_builder.CreateSelect(m_builder.CreateOr(isUndefined1, isUndefined2), posInf, inner);

    return ins->next;
}

LLVMInstruction *Math::buildAsin(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // asin(x) * 180.0 / pi + 0.0
    // +0.0 to avoid -0.0
    llvm::Constant *zero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Function *asinFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::asin, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(asinFunc, num), piDeg), pi), zero);

    return ins->next;
}

LLVMInstruction *Math::buildAcos(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // acos(x) * 180.0 / pi
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Function *acosFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::acos, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(acosFunc, num), piDeg), pi);

    return ins->next;
}

LLVMInstruction *Math::buildAtan(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // atan(x) * 180.0 / pi
    llvm::Constant *pi = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(std::acos(-1.0)));
    llvm::Constant *piDeg = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(180.0));
    llvm::Function *atanFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::atan, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(atanFunc, num), piDeg), pi);

    return ins->next;
}

LLVMInstruction *Math::buildLn(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // log(x)
    llvm::Function *logFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::log, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(logFunc, num);

    return ins->next;
}

LLVMInstruction *Math::buildLog10(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // log10(x)
    llvm::Function *log10Func = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::log10, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(log10Func, num);

    return ins->next;
}

LLVMInstruction *Math::buildExp(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // exp(x)
    llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::exp, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(expFunc, num);

    return ins->next;
}

LLVMInstruction *Math::buildExp10(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // exp10(x)
    llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::exp10, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(expFunc, num);

    return ins->next;
}
