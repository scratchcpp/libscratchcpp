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
    llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    ins->functionReturnReg->value = m_builder.CreateFAdd(num1, num2);

    return ins->next;
}

LLVMInstruction *Math::buildSub(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    ins->functionReturnReg->value = m_builder.CreateFSub(num1, num2);

    return ins->next;
}

LLVMInstruction *Math::buildMul(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    ins->functionReturnReg->value = m_builder.CreateFMul(num1, num2);

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
    ins->functionReturnReg->value = m_builder.CreateCall(m_utils.functions().resolve_llvm_random_long(), { m_utils.executionContextPtr(), from, to });

    return ins->next;
}

LLVMInstruction *Math::buildMod(LLVMInstruction *ins)
{
    assert(ins->args.size() == 2);
    const auto &arg1 = ins->args[0];
    const auto &arg2 = ins->args[1];
    // rem(a, b) / b < 0.0 ? rem(a, b) + b : rem(a, b)
    llvm::Constant *zero = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
    llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
    llvm::Value *value = m_builder.CreateFRem(num1, num2);                                // rem(a, b)
    llvm::Value *cond = m_builder.CreateFCmpOLT(m_builder.CreateFDiv(value, num2), zero); // rem(a, b) / b < 0.0                                                            // rem(a, b)
    ins->functionReturnReg->value = m_builder.CreateSelect(cond, m_builder.CreateFAdd(value, num2), value);

    return ins->next;
}

LLVMInstruction *Math::buildRound(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Module *module = m_utils.module();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    // x >= 0.0 ? round(x) : (x >= -0.5 ? -0.0 : floor(x + 0.5))
    llvm::Constant *zero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Constant *negativeZero = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-0.0));
    llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::round, m_builder.getDoubleTy());
    llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::floor, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    llvm::Value *notNegative = m_builder.CreateFCmpOGE(num, zero);                                                                               // num >= 0.0
    llvm::Value *roundNum = m_builder.CreateCall(roundFunc, num);                                                                                // round(num)
    llvm::Value *negativeCond = m_builder.CreateFCmpOGE(num, llvm::ConstantFP::get(llvmCtx, llvm::APFloat(-0.5)));                               // num >= -0.5
    llvm::Value *negativeRound = m_builder.CreateCall(floorFunc, m_builder.CreateFAdd(num, llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.5)))); // floor(x + 0.5)
    ins->functionReturnReg->value = m_builder.CreateSelect(notNegative, roundNum, m_builder.CreateSelect(negativeCond, negativeZero, negativeRound));

    return ins->next;
}

LLVMInstruction *Math::buildAbs(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Function *absFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::fabs, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(absFunc, num);

    return ins->next;
}

LLVMInstruction *Math::buildFloor(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::floor, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(floorFunc, num);

    return ins->next;
}

LLVMInstruction *Math::buildCeil(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    llvm::Function *ceilFunc = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::ceil, m_builder.getDoubleTy());
    llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
    ins->functionReturnReg->value = m_builder.CreateCall(ceilFunc, num);

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
