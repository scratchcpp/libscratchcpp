// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/PassBuilder.h>

#include <scratchcpp/stage.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compilerconstant.h>

#include "llvmcodebuilder.h"
#include "llvmexecutablecode.h"
#include "llvmconstantregister.h"
#include "llvmifstatement.h"
#include "llvmloop.h"
#include "llvmtypes.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType>
    TYPE_MAP = { { ValueType::Number, Compiler::StaticType::Number }, { ValueType::Bool, Compiler::StaticType::Bool }, { ValueType::String, Compiler::StaticType::String } };

LLVMCodeBuilder::LLVMCodeBuilder(Target *target, const std::string &id, bool warp) :
    m_target(target),
    m_id(id),
    m_module(std::make_unique<llvm::Module>(id, m_ctx)),
    m_builder(m_ctx),
    m_defaultWarp(warp),
    m_warp(warp)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    initTypes();
    createVariableMap();
    createListMap();
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::finalize()
{
    // Do not create coroutine if there are no yield instructions
    if (!m_warp) {
        auto it = std::find_if(m_instructions.begin(), m_instructions.end(), [](const LLVMInstruction &step) { return step.type == LLVMInstruction::Type::Yield; });

        if (it == m_instructions.end())
            m_warp = true;
    }

    // Set fast math flags
    llvm::FastMathFlags fmf;
    fmf.setFast(true);
    fmf.setNoNaNs(false);
    fmf.setNoSignedZeros(false);
    m_builder.setFastMathFlags(fmf);

    // Create function
    // void *f(Target *, ValueData **, List **)
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    llvm::FunctionType *funcType = llvm::FunctionType::get(pointerType, { pointerType, pointerType, pointerType }, false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f", m_module.get());
    llvm::Value *targetPtr = func->getArg(0);
    llvm::Value *targetVariables = func->getArg(1);
    llvm::Value *targetLists = func->getArg(2);

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
    m_builder.SetInsertPoint(entry);

    // Init coroutine
    std::unique_ptr<LLVMCoroutine> coro;

    if (!m_warp)
        coro = std::make_unique<LLVMCoroutine>(m_module.get(), &m_builder, func);

    std::vector<LLVMIfStatement> ifStatements;
    std::vector<LLVMLoop> loops;
    m_heap.clear();

    // Create variable pointers
    for (auto &[var, varPtr] : m_variablePtrs) {
        llvm::Value *ptr = getVariablePtr(targetVariables, var);

        // Direct access
        varPtr.heapPtr = ptr;

        // All variables are currently created on the stack and synced later (seems to be faster)
        // NOTE: Strings are NOT copied, only the pointer and string size are copied
        varPtr.stackPtr = m_builder.CreateAlloca(m_valueDataType);
        varPtr.onStack = false; // use heap before the first assignment
    }

    // Create list pointers
    for (auto &[list, listPtr] : m_listPtrs) {
        listPtr.ptr = getListPtr(targetLists, list);

        listPtr.dataPtr = m_builder.CreateAlloca(m_valueDataType->getPointerTo());
        m_builder.CreateStore(m_builder.CreateCall(resolve_list_data(), listPtr.ptr), listPtr.dataPtr);

        listPtr.sizePtr = m_builder.CreateCall(resolve_list_size_ptr(), listPtr.ptr);
        listPtr.allocatedSizePtr = m_builder.CreateCall(resolve_list_alloc_size_ptr(), listPtr.ptr);

        listPtr.dataPtrDirty = m_builder.CreateAlloca(m_builder.getInt1Ty());
        m_builder.CreateStore(m_builder.getInt1(false), listPtr.dataPtrDirty);
    }

    m_scopeVariables.clear();
    m_scopeLists.clear();
    pushScopeLevel();

    // Execute recorded steps
    for (const LLVMInstruction &step : m_instructions) {
        switch (step.type) {
            case LLVMInstruction::Type::FunctionCall: {
                std::vector<llvm::Type *> types;
                std::vector<llvm::Value *> args;

                // Add target pointer arg
                types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0));
                args.push_back(targetPtr);

                // Args
                for (auto &arg : step.args) {
                    types.push_back(getType(arg.first));
                    args.push_back(castValue(arg.second, arg.first));
                }

                llvm::Type *retType = getType(step.functionReturnReg ? step.functionReturnReg->type() : Compiler::StaticType::Void);
                llvm::Value *ret = m_builder.CreateCall(resolveFunction(step.functionName, llvm::FunctionType::get(retType, types, false)), args);

                if (step.functionReturnReg) {
                    step.functionReturnReg->value = ret;

                    if (step.functionReturnReg->type() == Compiler::StaticType::String)
                        m_heap.push_back(step.functionReturnReg->value);
                }

                break;
            }

            case LLVMInstruction::Type::Add: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFAdd(num1, num2);
                break;
            }

            case LLVMInstruction::Type::Sub: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFSub(num1, num2);
                break;
            }

            case LLVMInstruction::Type::Mul: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFMul(num1, num2);
                break;
            }

            case LLVMInstruction::Type::Div: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFDiv(num1, num2);
                break;
            }

            case LLVMInstruction::Type::CmpEQ: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::EQ);
                break;
            }

            case LLVMInstruction::Type::CmpGT: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::GT);
                break;
            }

            case LLVMInstruction::Type::CmpLT: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::LT);
                break;
            }

            case LLVMInstruction::Type::And: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *bool1 = castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = castValue(arg2.second, arg2.first);
                step.functionReturnReg->value = m_builder.CreateAnd(bool1, bool2);
                break;
            }

            case LLVMInstruction::Type::Or: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *bool1 = castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = castValue(arg2.second, arg2.first);
                step.functionReturnReg->value = m_builder.CreateOr(bool1, bool2);
                break;
            }

            case LLVMInstruction::Type::Not: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Value *value = castValue(arg.second, arg.first);
                step.functionReturnReg->value = m_builder.CreateNot(value);
                break;
            }

            case LLVMInstruction::Type::Mod: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                // rem(a, b) / b < 0.0 ? rem(a, b) + b : rem(a, b)
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                llvm::Value *value = m_builder.CreateFRem(num1, num2);                                // rem(a, b)
                llvm::Value *cond = m_builder.CreateFCmpOLT(m_builder.CreateFDiv(value, num2), zero); // rem(a, b) / b < 0.0                                                            // rem(a, b)
                step.functionReturnReg->value = m_builder.CreateSelect(cond, m_builder.CreateFAdd(value, num2), value);
                break;
            }

            case LLVMInstruction::Type::Round: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // x >= 0.0 ? round(x) : (x >= -0.5 ? -0.0 : floor(x + 0.5))
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Constant *negativeZero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(-0.0));
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::floor, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                llvm::Value *notNegative = m_builder.CreateFCmpOGE(num, zero);                                                                             // num >= 0.0
                llvm::Value *roundNum = m_builder.CreateCall(roundFunc, num);                                                                              // round(num)
                llvm::Value *negativeCond = m_builder.CreateFCmpOGE(num, llvm::ConstantFP::get(m_ctx, llvm::APFloat(-0.5)));                               // num >= -0.5
                llvm::Value *negativeRound = m_builder.CreateCall(floorFunc, m_builder.CreateFAdd(num, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.5)))); // floor(x + 0.5)
                step.functionReturnReg->value = m_builder.CreateSelect(notNegative, roundNum, m_builder.CreateSelect(negativeCond, negativeZero, negativeRound));
                break;
            }

            case LLVMInstruction::Type::Abs: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *absFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::fabs, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(absFunc, num);
                break;
            }

            case LLVMInstruction::Type::Floor: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::floor, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(floorFunc, num);
                break;
            }

            case LLVMInstruction::Type::Ceil: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *ceilFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::ceil, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(ceilFunc, num);
                break;
            }

            case LLVMInstruction::Type::Sqrt: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // sqrt(x) + 0.0
                // This avoids negative zero
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Function *sqrtFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::sqrt, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateCall(sqrtFunc, num), zero);
                break;
            }

            case LLVMInstruction::Type::Sin: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // round(sin(x * pi / 180.0) * 1e10) / 1e10 + 0.0
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_ctx, llvm::APFloat(1e10));
                llvm::Function *sinFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::sin, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                llvm::Value *sinResult = m_builder.CreateCall(sinFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // sin(x * pi / 180)
                llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(sinResult, factor));                    // round(sin(x * 180) * 1e10)
                step.functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(rounded, factor), zero);
                break;
            }

            case LLVMInstruction::Type::Cos: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // round(cos(x * pi / 180.0) * 1e10) / 1e10
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_ctx, llvm::APFloat(1e10));
                llvm::Function *cosFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::cos, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                llvm::Value *cosResult = m_builder.CreateCall(cosFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // cos(x * pi / 180)
                llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(cosResult, factor));                    // round(cos(x * 180) * 1e10)
                step.functionReturnReg->value = m_builder.CreateFDiv(rounded, factor);
                break;
            }

            case LLVMInstruction::Type::Tan: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // ((mod = rem(x, 360.0)) == -270.0 || mod == 90.0) ? inf : ((mod == -90.0 || mod == 270.0) ? -inf : round(tan(x * pi / 180.0) * 1e10) / 1e10 + 0.0)
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Constant *full = llvm::ConstantFP::get(m_ctx, llvm::APFloat(360.0));
                llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
                llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
                llvm::Constant *undefined1 = llvm::ConstantFP::get(m_ctx, llvm::APFloat(-270.0));
                llvm::Constant *undefined2 = llvm::ConstantFP::get(m_ctx, llvm::APFloat(90.0));
                llvm::Constant *undefined3 = llvm::ConstantFP::get(m_ctx, llvm::APFloat(-90.0));
                llvm::Constant *undefined4 = llvm::ConstantFP::get(m_ctx, llvm::APFloat(270.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_ctx, llvm::APFloat(1e10));
                llvm::Function *tanFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::tan, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                llvm::Value *mod = m_builder.CreateFRem(num, full);
                llvm::Value *isUndefined1 = m_builder.CreateFCmpOEQ(mod, undefined1);                                               // rem(x, 360.0) == -270.0
                llvm::Value *isUndefined2 = m_builder.CreateFCmpOEQ(mod, undefined2);                                               // rem(x, 360.0) == 90.0
                llvm::Value *isUndefined3 = m_builder.CreateFCmpOEQ(mod, undefined3);                                               // rem(x, 360.0) == -90.0
                llvm::Value *isUndefined4 = m_builder.CreateFCmpOEQ(mod, undefined4);                                               // rem(x, 360.0) == 270.0
                llvm::Value *tanResult = m_builder.CreateCall(tanFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // tan(x * pi / 180)
                llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(tanResult, factor));                    // round(tan(x * 180) * 1e10)
                llvm::Value *result = m_builder.CreateFAdd(m_builder.CreateFDiv(rounded, factor), zero);                            // round(tan(x * pi / 180.0) * 1e10) / 1e10 + 0.0
                llvm::Value *inner = m_builder.CreateSelect(m_builder.CreateOr(isUndefined3, isUndefined4), negInf, result);
                step.functionReturnReg->value = m_builder.CreateSelect(m_builder.CreateOr(isUndefined1, isUndefined2), posInf, inner);
                break;
            }

            case LLVMInstruction::Type::Asin: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // asin(x) * 180.0 / pi + 0.0
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Function *asinFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::asin, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(asinFunc, num), piDeg), pi), zero);
                break;
            }

            case LLVMInstruction::Type::Acos: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // acos(x) * 180.0 / pi
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Function *acosFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::acos, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(acosFunc, num), piDeg), pi);
                break;
            }

            case LLVMInstruction::Type::Atan: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // atan(x) * 180.0 / pi
                llvm::Constant *pi = llvm::ConstantFP::get(m_ctx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_ctx, llvm::APFloat(180.0));
                llvm::Function *atanFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::atan, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(atanFunc, num), piDeg), pi);
                break;
            }

            case LLVMInstruction::Type::Ln: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // log(x)
                llvm::Function *logFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::log, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(logFunc, num);
                break;
            }

            case LLVMInstruction::Type::Log10: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // log10(x)
                llvm::Function *log10Func = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::log10, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(log10Func, num);
                break;
            }

            case LLVMInstruction::Type::Exp: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // exp(x)
                llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::exp, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(expFunc, num);
                break;
            }

            case LLVMInstruction::Type::Exp10: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                // exp10(x)
                llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::exp10, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(expFunc, num);
                break;
            }

            case LLVMInstruction::Type::WriteVariable: {
                assert(step.args.size() == 1);
                assert(m_variablePtrs.find(step.workVariable) != m_variablePtrs.cend());
                const auto &arg = step.args[0];
                Compiler::StaticType type = optimizeRegisterType(arg.second);
                LLVMVariablePtr &varPtr = m_variablePtrs[step.workVariable];
                varPtr.changed = true;

                // Initialize stack variable on first assignment
                if (!varPtr.onStack) {
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
                }

                createValueStore(arg.second, varPtr.stackPtr, type, varPtr.type);
                varPtr.type = type;
                m_scopeVariables.back()[&varPtr] = varPtr.type;
                break;
            }

            case LLVMInstruction::Type::ReadVariable: {
                assert(step.args.size() == 0);
                const LLVMVariablePtr &varPtr = m_variablePtrs[step.workVariable];
                step.functionReturnReg->value = varPtr.onStack ? varPtr.stackPtr : varPtr.heapPtr;
                step.functionReturnReg->setType(varPtr.type);
                break;
            }

            case LLVMInstruction::Type::ClearList: {
                assert(step.args.size() == 0);
                LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *oldAllocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
                m_builder.CreateCall(resolve_list_clear(), listPtr.ptr);
                m_builder.CreateStore(m_builder.getInt1(true), listPtr.dataPtrDirty);

                // Clearing may deallocate, so check if the allocated size changed
                llvm::Value *dataPtrDirty = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.dataPtrDirty);
                llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
                m_builder.CreateStore(m_builder.CreateOr(dataPtrDirty, m_builder.CreateICmpNE(allocatedSize, oldAllocatedSize)), listPtr.dataPtrDirty);

                m_scopeLists.back().erase(&listPtr);
                break;
            }

            case LLVMInstruction::Type::RemoveListItem: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                const LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *index = m_builder.CreateFPToUI(castValue(arg.second, arg.first), m_builder.getInt64Ty());
                m_builder.CreateCall(resolve_list_remove(), { listPtr.ptr, index });
                // NOTE: Removing doesn't deallocate (see List::removeAt()), so there's no need to update the data pointer
                break;
            }

            case LLVMInstruction::Type::AppendToList: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                Compiler::StaticType type = optimizeRegisterType(arg.second);
                LLVMListPtr &listPtr = m_listPtrs[step.workList];

                // Check if enough space is allocated
                llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                llvm::Value *isAllocated = m_builder.CreateICmpUGT(allocatedSize, size);
                llvm::BasicBlock *ifBlock = llvm::BasicBlock::Create(m_ctx, "", func);
                llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(m_ctx, "", func);
                llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_ctx, "", func);
                m_builder.CreateCondBr(isAllocated, ifBlock, elseBlock);

                // If there's enough space, use the allocated memory
                m_builder.SetInsertPoint(ifBlock);
                llvm::Value *itemPtr = getListItem(listPtr, size, func);
                createReusedValueStore(arg.second, itemPtr, type);
                m_builder.CreateStore(m_builder.CreateAdd(size, m_builder.getInt64(1)), listPtr.sizePtr);
                m_builder.CreateBr(nextBlock);

                // Otherwise call appendEmpty()
                m_builder.SetInsertPoint(elseBlock);
                itemPtr = m_builder.CreateCall(resolve_list_append_empty(), listPtr.ptr);
                createReusedValueStore(arg.second, itemPtr, type);
                m_builder.CreateStore(m_builder.getInt1(true), listPtr.dataPtrDirty);
                m_builder.CreateBr(nextBlock);

                m_builder.SetInsertPoint(nextBlock);
                auto &typeMap = m_scopeLists.back();

                if (typeMap.find(&listPtr) == typeMap.cend()) {
                    listPtr.type = type;
                    typeMap[&listPtr] = listPtr.type;
                } else if (listPtr.type != type) {
                    listPtr.type = Compiler::StaticType::Unknown;
                    typeMap[&listPtr] = listPtr.type;
                }

                break;
            }

            case LLVMInstruction::Type::InsertToList: {
                assert(step.args.size() == 2);
                const auto &indexArg = step.args[0];
                const auto &valueArg = step.args[1];
                Compiler::StaticType type = optimizeRegisterType(valueArg.second);
                LLVMListPtr &listPtr = m_listPtrs[step.workList];

                // dataPtrDirty
                llvm::Value *dataPtrDirty = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.dataPtrDirty);
                llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                m_builder.CreateStore(m_builder.CreateOr(dataPtrDirty, m_builder.CreateICmpEQ(allocatedSize, size)), listPtr.dataPtrDirty);

                // Insert
                llvm::Value *index = m_builder.CreateFPToUI(castValue(indexArg.second, indexArg.first), m_builder.getInt64Ty());
                llvm::Value *itemPtr = m_builder.CreateCall(resolve_list_insert_empty(), { listPtr.ptr, index });
                createReusedValueStore(valueArg.second, itemPtr, type);

                auto &typeMap = m_scopeLists.back();

                if (typeMap.find(&listPtr) == typeMap.cend()) {
                    listPtr.type = type;
                    typeMap[&listPtr] = listPtr.type;
                } else if (listPtr.type != type) {
                    listPtr.type = Compiler::StaticType::Unknown;
                    typeMap[&listPtr] = listPtr.type;
                }

                break;
            }

            case LLVMInstruction::Type::ListReplace: {
                assert(step.args.size() == 2);
                const auto &indexArg = step.args[0];
                const auto &valueArg = step.args[1];
                Compiler::StaticType type = optimizeRegisterType(valueArg.second);
                LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *index = m_builder.CreateFPToUI(castValue(indexArg.second, indexArg.first), m_builder.getInt64Ty());
                llvm::Value *itemPtr = getListItem(listPtr, index, func);
                createValueStore(valueArg.second, itemPtr, type, listPtr.type);

                auto &typeMap = m_scopeLists.back();

                if (typeMap.find(&listPtr) == typeMap.cend()) {
                    listPtr.type = type;
                    typeMap[&listPtr] = listPtr.type;
                } else if (listPtr.type != type) {
                    listPtr.type = Compiler::StaticType::Unknown;
                    typeMap[&listPtr] = listPtr.type;
                }

                break;
            }

            case LLVMInstruction::Type::GetListItem: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                const LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *index = m_builder.CreateFPToUI(castValue(arg.second, arg.first), m_builder.getInt64Ty());
                step.functionReturnReg->value = getListItem(listPtr, index, func);
                step.functionReturnReg->setType(listPtr.type);
                break;
            }

            case LLVMInstruction::Type::GetListSize: {
                assert(step.args.size() == 0);
                const LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                step.functionReturnReg->value = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
                break;
            }

            case LLVMInstruction::Type::GetListItemIndex: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                const LLVMListPtr &listPtr = m_listPtrs[step.workList];
                step.functionReturnReg->value = m_builder.CreateSIToFP(getListItemIndex(listPtr, arg.second, func), m_builder.getDoubleTy());
                break;
            }

            case LLVMInstruction::Type::ListContainsItem: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                const LLVMListPtr &listPtr = m_listPtrs[step.workList];
                llvm::Value *index = getListItemIndex(listPtr, arg.second, func);
                step.functionReturnReg->value = m_builder.CreateICmpSGT(index, llvm::ConstantInt::get(m_builder.getInt64Ty(), -1, true));
                break;
            }

            case LLVMInstruction::Type::Yield:
                if (!m_warp) {
                    freeHeap();
                    syncVariables(targetVariables);
                    coro->createSuspend();
                    reloadVariables(targetVariables);
                    reloadLists();
                }

                break;

            case LLVMInstruction::Type::BeginIf: {
                LLVMIfStatement statement;
                statement.beforeIf = m_builder.GetInsertBlock();
                statement.body = llvm::BasicBlock::Create(m_ctx, "", func);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                freeHeap();
                statement.condition = castValue(reg.second, reg.first);

                // Switch to body branch
                m_builder.SetInsertPoint(statement.body);

                ifStatements.push_back(statement);
                pushScopeLevel();
                break;
            }

            case LLVMInstruction::Type::BeginElse: {
                assert(!ifStatements.empty());
                LLVMIfStatement &statement = ifStatements.back();

                // Restore types from parent scope
                std::unordered_map<LLVMVariablePtr *, Compiler::StaticType> parentScopeVariables = m_scopeVariables[m_scopeVariables.size() - 2]; // no reference!
                popScopeLevel();

                for (auto &[ptr, type] : parentScopeVariables)
                    ptr->type = type;

                pushScopeLevel();

                // Jump to the branch after the if statement
                assert(!statement.afterIf);
                statement.afterIf = llvm::BasicBlock::Create(m_ctx, "", func);
                freeHeap();
                m_builder.CreateBr(statement.afterIf);

                // Create else branch
                assert(!statement.elseBranch);
                statement.elseBranch = llvm::BasicBlock::Create(m_ctx, "", func);

                // Since there's an else branch, the conditional instruction should jump to it
                m_builder.SetInsertPoint(statement.beforeIf);
                m_builder.CreateCondBr(statement.condition, statement.body, statement.elseBranch);

                // Switch to the else branch
                m_builder.SetInsertPoint(statement.elseBranch);
                break;
            }

            case LLVMInstruction::Type::EndIf: {
                assert(!ifStatements.empty());
                LLVMIfStatement &statement = ifStatements.back();

                // Jump to the branch after the if statement
                if (!statement.afterIf)
                    statement.afterIf = llvm::BasicBlock::Create(m_ctx, "", func);

                freeHeap();
                m_builder.CreateBr(statement.afterIf);

                if (statement.elseBranch) {
                } else {
                    // If there wasn't an 'else' branch, create a conditional instruction which skips the if statement if false
                    m_builder.SetInsertPoint(statement.beforeIf);
                    m_builder.CreateCondBr(statement.condition, statement.body, statement.afterIf);
                }

                // Switch to the branch after the if statement
                m_builder.SetInsertPoint(statement.afterIf);

                ifStatements.pop_back();
                popScopeLevel();
                break;
            }

            case LLVMInstruction::Type::BeginRepeatLoop: {
                LLVMLoop loop;
                loop.isRepeatLoop = true;

                // index = 0
                llvm::Constant *zero = llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true);
                loop.index = m_builder.CreateAlloca(m_builder.getInt64Ty());
                m_builder.CreateStore(zero, loop.index);

                // Create branches
                llvm::BasicBlock *roundBranch = llvm::BasicBlock::Create(m_ctx, "", func);
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", func);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", func);

                // Use last reg for count
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Number);
                llvm::Value *count = castValue(reg.second, reg.first);

                // Clamp count if <= 0 (we can skip the loop if count is not positive)
                llvm::Value *comparison = m_builder.CreateFCmpULE(count, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));
                freeHeap();
                m_builder.CreateCondBr(comparison, loop.afterLoop, roundBranch);

                // Round (Scratch-specific behavior)
                m_builder.SetInsertPoint(roundBranch);
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, { count->getType() });
                count = m_builder.CreateCall(roundFunc, { count });
                count = m_builder.CreateFPToSI(count, m_builder.getInt64Ty()); // cast to signed integer

                // Jump to condition branch
                m_builder.CreateBr(loop.conditionBranch);

                // Check index
                m_builder.SetInsertPoint(loop.conditionBranch);

                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", func);

                if (!loop.afterLoop)
                    loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", func);

                llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                comparison = m_builder.CreateICmpULT(currentIndex, count);
                m_builder.CreateCondBr(comparison, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);

                loops.push_back(loop);
                pushScopeLevel();
                break;
            }

            case LLVMInstruction::Type::BeginWhileLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", func);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", func);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = castValue(reg.second, reg.first);
                freeHeap();
                m_builder.CreateCondBr(condition, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                pushScopeLevel();
                break;
            }

            case LLVMInstruction::Type::BeginRepeatUntilLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", func);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", func);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = castValue(reg.second, reg.first);
                freeHeap();
                m_builder.CreateCondBr(condition, loop.afterLoop, body);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                pushScopeLevel();
                break;
            }

            case LLVMInstruction::Type::BeginLoopCondition: {
                LLVMLoop loop;
                loop.isRepeatLoop = false;
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", func);
                freeHeap();
                m_builder.CreateBr(loop.conditionBranch);
                m_builder.SetInsertPoint(loop.conditionBranch);
                loops.push_back(loop);
                break;
            }

            case LLVMInstruction::Type::EndLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                if (loop.isRepeatLoop) {
                    // Increment index
                    llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                    llvm::Value *incremented = m_builder.CreateAdd(currentIndex, llvm::ConstantInt::get(m_builder.getInt64Ty(), 1, true));
                    m_builder.CreateStore(incremented, loop.index);
                }

                // Jump to the condition branch
                freeHeap();
                m_builder.CreateBr(loop.conditionBranch);

                // Switch to the branch after the loop
                m_builder.SetInsertPoint(loop.afterLoop);

                loops.pop_back();
                popScopeLevel();
                break;
            }
        }
    }

    freeHeap();
    syncVariables(targetVariables);

    // End and verify the function
    if (m_warp)
        m_builder.CreateRet(llvm::ConstantPointerNull::get(pointerType));
    else
        coro->end();

    verifyFunction(func);

    // Create resume function
    // bool resume(void *)
    funcType = llvm::FunctionType::get(m_builder.getInt1Ty(), pointerType, false);
    func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "resume", m_module.get());

    entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
    m_builder.SetInsertPoint(entry);

    if (m_warp)
        m_builder.CreateRet(m_builder.getInt1(true));
    else
        m_builder.CreateRet(coro->createResume(func->getArg(0)));

    verifyFunction(func);

#ifdef PRINT_LLVM_IR
    std::cout << std::endl << "=== LLVM IR (" << m_module->getName().str() << ") ===" << std::endl;
    m_module->print(llvm::outs(), nullptr);
    std::cout << "==============" << std::endl << std::endl;
#endif

    // Optimize
    optimize();

    return std::make_shared<LLVMExecutableCode>(std::move(m_module));
}

CompilerValue *LLVMCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    assert(argTypes.size() == args.size());

    LLVMInstruction ins(LLVMInstruction::Type::FunctionCall);
    ins.functionName = functionName;

    for (size_t i = 0; i < args.size(); i++)
        ins.args.push_back({ argTypes[i], static_cast<LLVMRegister *>(args[i]) });

    if (returnType != Compiler::StaticType::Void) {
        auto reg = std::make_shared<LLVMRegister>(returnType);
        reg->isRawValue = true;
        ins.functionReturnReg = reg.get();
        m_instructions.push_back(ins);
        return addReg(reg);
    }

    m_instructions.push_back(ins);
    return nullptr;
}

CompilerConstant *LLVMCodeBuilder::addConstValue(const Value &value)
{
    auto constReg = std::make_shared<LLVMConstantRegister>(TYPE_MAP[value.type()], value);
    auto reg = std::reinterpret_pointer_cast<LLVMRegister>(constReg);
    return static_cast<CompilerConstant *>(addReg(reg));
}

CompilerValue *LLVMCodeBuilder::addVariableValue(Variable *variable)
{
    LLVMInstruction ins(LLVMInstruction::Type::ReadVariable);
    ins.workVariable = variable;
    m_variablePtrs[variable] = LLVMVariablePtr();

    auto ret = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    ret->isRawValue = false;
    ins.functionReturnReg = ret.get();

    m_instructions.push_back(ins);
    return addReg(ret);
}

CompilerValue *LLVMCodeBuilder::addListContents(List *list)
{
    return addConstValue(Value());
}

CompilerValue *LLVMCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListItem);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();

    ins.args.push_back({ Compiler::StaticType::Number, static_cast<LLVMRegister *>(index) });

    auto ret = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    ret->isRawValue = false;
    ins.functionReturnReg = ret.get();

    m_instructions.push_back(ins);
    return addReg(ret);
}

CompilerValue *LLVMCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListItemIndex);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    return createOp(ins, Compiler::StaticType::Number, Compiler::StaticType::Unknown, { item });
}

CompilerValue *LLVMCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListContainsItem);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    return createOp(ins, Compiler::StaticType::Bool, Compiler::StaticType::Unknown, { item });
}

CompilerValue *LLVMCodeBuilder::addListSize(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListSize);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    return createOp(ins, Compiler::StaticType::Number);
}

CompilerValue *LLVMCodeBuilder::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::Add, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::Sub, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::Mul, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::Div, Compiler::StaticType::Number, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::CmpEQ, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::CmpGT, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::CmpLT, Compiler::StaticType::Bool, Compiler::StaticType::Number, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::And, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return createOp(LLVMInstruction::Type::Or, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand1, operand2 });
}

CompilerValue *LLVMCodeBuilder::createNot(CompilerValue *operand)
{
    return createOp(LLVMInstruction::Type::Not, Compiler::StaticType::Bool, Compiler::StaticType::Bool, { operand });
}

CompilerValue *LLVMCodeBuilder::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return createOp(LLVMInstruction::Type::Mod, Compiler::StaticType::Number, Compiler::StaticType::Number, { num1, num2 });
}

CompilerValue *LLVMCodeBuilder::createRound(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Round, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createAbs(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Abs, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createFloor(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Floor, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createCeil(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Ceil, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createSqrt(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Sqrt, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createSin(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Sin, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createCos(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Cos, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createTan(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Tan, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createAsin(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Asin, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createAcos(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Acos, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createAtan(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Atan, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createLn(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Ln, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createLog10(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Log10, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createExp(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Exp, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

CompilerValue *LLVMCodeBuilder::createExp10(CompilerValue *num)
{
    return createOp(LLVMInstruction::Type::Exp10, Compiler::StaticType::Number, Compiler::StaticType::Number, { num });
}

void LLVMCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
    LLVMInstruction ins(LLVMInstruction::Type::WriteVariable);
    ins.workVariable = variable;
    m_variablePtrs[variable] = LLVMVariablePtr();
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { value });
}

void LLVMCodeBuilder::createListClear(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::ClearList);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    createOp(ins, Compiler::StaticType::Void);
}

void LLVMCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    LLVMInstruction ins(LLVMInstruction::Type::RemoveListItem);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Number, { index });
}

void LLVMCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::AppendToList);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { item });
}

void LLVMCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::InsertToList);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    createOp(ins, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Unknown }, { index, item });
}

void LLVMCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListReplace);
    ins.workList = list;
    m_listPtrs[list] = LLVMListPtr();
    createOp(ins, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Unknown }, { index, item });
}

void LLVMCodeBuilder::beginIfStatement(CompilerValue *cond)
{
    LLVMInstruction ins(LLVMInstruction::Type::BeginIf);
    ins.args.push_back({ Compiler::StaticType::Bool, static_cast<LLVMRegister *>(cond) });
    m_instructions.push_back(ins);
}

void LLVMCodeBuilder::beginElseBranch()
{
    m_instructions.push_back(LLVMInstruction(LLVMInstruction::Type::BeginElse));
}

void LLVMCodeBuilder::endIf()
{
    m_instructions.push_back(LLVMInstruction(LLVMInstruction::Type::EndIf));
}

void LLVMCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
    LLVMInstruction ins(LLVMInstruction::Type::BeginRepeatLoop);
    ins.args.push_back({ Compiler::StaticType::Number, static_cast<LLVMRegister *>(count) });
    m_instructions.push_back(ins);
}

void LLVMCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
    LLVMInstruction ins(LLVMInstruction::Type::BeginWhileLoop);
    ins.args.push_back({ Compiler::StaticType::Bool, static_cast<LLVMRegister *>(cond) });
    m_instructions.push_back(ins);
}

void LLVMCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
    LLVMInstruction ins(LLVMInstruction::Type::BeginRepeatUntilLoop);
    ins.args.push_back({ Compiler::StaticType::Bool, static_cast<LLVMRegister *>(cond) });
    m_instructions.push_back(ins);
}

void LLVMCodeBuilder::beginLoopCondition()
{
    m_instructions.push_back({ LLVMInstruction::Type::BeginLoopCondition });
}

void LLVMCodeBuilder::endLoop()
{
    if (!m_warp)
        m_instructions.push_back(LLVMInstruction(LLVMInstruction::Type::Yield));

    m_instructions.push_back(LLVMInstruction(LLVMInstruction::Type::EndLoop));
}

void LLVMCodeBuilder::yield()
{
    m_instructions.push_back({ LLVMInstruction::Type::Yield });
}

void LLVMCodeBuilder::initTypes()
{
    m_valueDataType = LLVMTypes::createValueDataType(&m_builder);
}

void LLVMCodeBuilder::createVariableMap()
{
    if (!m_target)
        return;

    // Map variable pointers to variable data array indices
    const auto &variables = m_target->variables();
    ValueData **variableData = m_target->variableData();
    const size_t len = variables.size();
    m_targetVariableMap.clear();
    m_targetVariableMap.reserve(len);

    size_t i, j;

    for (i = 0; i < len; i++) {
        Variable *var = variables[i].get();

        // Find the data for this variable
        for (j = 0; j < len; j++) {
            if (variableData[j] == &var->valuePtr()->data())
                break;
        }

        if (j < len)
            m_targetVariableMap[var] = j;
        else
            assert(false);
    }
}

void LLVMCodeBuilder::createListMap()
{
    if (!m_target)
        return;

    // Map list pointers to list array indices
    const auto &lists = m_target->lists();
    List **listData = m_target->listData();
    const size_t len = lists.size();
    m_targetListMap.clear();
    m_targetListMap.reserve(len);

    size_t i, j;

    for (i = 0; i < len; i++) {
        List *list = lists[i].get();

        // Find this list
        for (j = 0; j < len; j++) {
            if (listData[j] == list)
                break;
        }

        if (j < len)
            m_targetListMap[list] = j;
        else
            assert(false);
    }
}

void LLVMCodeBuilder::pushScopeLevel()
{
    m_scopeVariables.push_back({});

    if (m_scopeLists.empty()) {
        std::unordered_map<LLVMListPtr *, Compiler::StaticType> listTypes;

        for (auto &[list, listPtr] : m_listPtrs)
            listTypes[&listPtr] = Compiler::StaticType::Unknown;

        m_scopeLists.push_back(listTypes);
    } else
        m_scopeLists.push_back(m_scopeLists.back());
}

void LLVMCodeBuilder::popScopeLevel()
{
    for (size_t i = 0; i < m_scopeVariables.size() - 1; i++) {
        for (auto &[ptr, type] : m_scopeVariables[i]) {
            if (ptr->type != type)
                ptr->type = Compiler::StaticType::Unknown;
        }
    }

    m_scopeVariables.pop_back();

    for (size_t i = 0; i < m_scopeLists.size() - 1; i++) {
        for (auto &[ptr, type] : m_scopeLists[i]) {
            if (ptr->type != type)
                ptr->type = Compiler::StaticType::Unknown;
        }
    }

    m_scopeLists.pop_back();
}

void LLVMCodeBuilder::verifyFunction(llvm::Function *func)
{
    if (llvm::verifyFunction(*func, &llvm::errs())) {
        llvm::errs() << "error: LLVM function verficiation failed!\n";
        llvm::errs() << "script hat ID: " << m_id << "\n";
    }
}

void LLVMCodeBuilder::optimize()
{
    llvm::PassBuilder passBuilder;
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cGSCCAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cGSCCAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cGSCCAnalysisManager, moduleAnalysisManager);

    llvm::ModulePassManager modulePassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
    modulePassManager.run(*m_module, moduleAnalysisManager);
}

CompilerValue *LLVMCodeBuilder::addReg(std::shared_ptr<LLVMRegister> reg)
{
    m_regs.push_back(reg);
    return reg.get();
}

void LLVMCodeBuilder::freeHeap()
{
    // Free dynamically allocated memory
    for (llvm::Value *ptr : m_heap)
        m_builder.CreateFree(ptr);

    m_heap.clear();
}

llvm::Value *LLVMCodeBuilder::castValue(LLVMRegister *reg, Compiler::StaticType targetType)
{
    if (reg->isConst())
        return castConstValue(reg->constValue(), targetType);

    if (reg->isRawValue)
        return castRawValue(reg, targetType);

    assert(reg->type() != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type()) {
                case Compiler::StaticType::Number: {
                    // Read number directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                }

                case Compiler::StaticType::Bool: {
                    // Read boolean and cast to double
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    llvm::Value *boolValue = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                    return m_builder.CreateSIToFP(boolValue, m_builder.getDoubleTy());
                }

                case Compiler::StaticType::String:
                case Compiler::StaticType::Unknown: {
                    // Convert to double
                    return m_builder.CreateCall(resolve_value_toDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type()) {
                case Compiler::StaticType::Number: {
                    // True if != 0
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    llvm::Value *numberValue = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                    return m_builder.CreateFCmpONE(numberValue, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));
                }

                case Compiler::StaticType::Bool: {
                    // Read boolean directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                }

                case Compiler::StaticType::String:
                case Compiler::StaticType::Unknown:
                    // Convert to bool
                    return m_builder.CreateCall(resolve_value_toBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type()) {
                case Compiler::StaticType::Number:
                case Compiler::StaticType::Bool:
                case Compiler::StaticType::Unknown: {
                    // Cast to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_toCString(), reg->value);
                    m_heap.push_back(ptr); // deallocate later
                    return ptr;
                }

                case Compiler::StaticType::String: {
                    // Read string pointer directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), ptr);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Value *LLVMCodeBuilder::castRawValue(LLVMRegister *reg, Compiler::StaticType targetType)
{
    if (reg->type() == targetType)
        return reg->value;

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type()) {
                case Compiler::StaticType::Bool:
                    // Cast bool to double
                    return m_builder.CreateUIToFP(reg->value, m_builder.getDoubleTy());

                case Compiler::StaticType::String: {
                    // Convert string to double
                    return m_builder.CreateCall(resolve_value_stringToDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type()) {
                case Compiler::StaticType::Number:
                    // Cast double to bool (true if != 0)
                    return m_builder.CreateFCmpONE(reg->value, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));

                case Compiler::StaticType::String:
                    // Convert string to bool
                    return m_builder.CreateCall(resolve_value_stringToBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type()) {
                case Compiler::StaticType::Number: {
                    // Convert double to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_doubleToCString(), reg->value);
                    m_heap.push_back(ptr); // deallocate later
                    return ptr;
                }

                case Compiler::StaticType::Bool: {
                    // Convert bool to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_boolToCString(), reg->value);
                    // NOTE: Dot not deallocate later
                    return ptr;
                }

                default:
                    assert(false);
                    return nullptr;
            }

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Constant *LLVMCodeBuilder::castConstValue(const Value &value, Compiler::StaticType targetType)
{
    switch (targetType) {
        case Compiler::StaticType::Number: {
            const double nan = std::numeric_limits<double>::quiet_NaN();
            return llvm::ConstantFP::get(m_ctx, llvm::APFloat(value.isNaN() ? nan : value.toDouble()));
        }

        case Compiler::StaticType::Bool:
            return m_builder.getInt1(value.toBool());

        case Compiler::StaticType::String:
            return m_builder.CreateGlobalStringPtr(value.toString());

        default:
            assert(false);
            return nullptr;
    }
}

Compiler::StaticType LLVMCodeBuilder::optimizeRegisterType(LLVMRegister *reg)
{
    Compiler::StaticType ret = reg->type();

    // Optimize string constants that represent numbers
    if (reg->isConst() && reg->type() == Compiler::StaticType::String && reg->constValue().isValidNumber())
        ret = Compiler::StaticType::Number;

    return ret;
}

llvm::Type *LLVMCodeBuilder::getType(Compiler::StaticType type)
{
    switch (type) {
        case Compiler::StaticType::Void:
            return m_builder.getVoidTy();

        case Compiler::StaticType::Number:
            return m_builder.getDoubleTy();

        case Compiler::StaticType::Bool:
            return m_builder.getInt1Ty();

        case Compiler::StaticType::String:
            return llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Value *LLVMCodeBuilder::isNaN(llvm::Value *num)
{
    return m_builder.CreateFCmpUNO(num, num);
}

llvm::Value *LLVMCodeBuilder::removeNaN(llvm::Value *num)
{
    // Replace NaN with zero
    return m_builder.CreateSelect(isNaN(num), llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)), num);
}

llvm::Value *LLVMCodeBuilder::getVariablePtr(llvm::Value *targetVariables, Variable *variable)
{
    if (!m_target->isStage() && variable->target() == m_target) {
        // If this is a local sprite variable, use the variable array at runtime (for clones)
        assert(m_targetVariableMap.find(variable) != m_targetVariableMap.cend());
        const size_t index = m_targetVariableMap[variable];
        llvm::Value *ptr = m_builder.CreateGEP(m_valueDataType->getPointerTo(), targetVariables, m_builder.getInt64(index));
        return m_builder.CreateLoad(m_valueDataType->getPointerTo(), ptr);
    }

    // Otherwise create a raw pointer at compile time
    llvm::Value *addr = m_builder.getInt64((uintptr_t)&variable->value().data());
    return m_builder.CreateIntToPtr(addr, m_valueDataType->getPointerTo());
}

llvm::Value *LLVMCodeBuilder::getListPtr(llvm::Value *targetLists, List *list)
{
    if (!m_target->isStage() && list->target() == m_target) {
        // If this is a local sprite list, use the list array at runtime (for clones)
        assert(m_targetListMap.find(list) != m_targetListMap.cend());
        const size_t index = m_targetListMap[list];
        auto pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
        llvm::Value *ptr = m_builder.CreateGEP(pointerType, targetLists, m_builder.getInt64(index));
        return m_builder.CreateLoad(pointerType, ptr);
    }

    // Otherwise create a raw pointer at compile time
    llvm::Value *addr = m_builder.getInt64((uintptr_t)list);
    return m_builder.CreateIntToPtr(addr, m_valueDataType->getPointerTo());
}

void LLVMCodeBuilder::syncVariables(llvm::Value *targetVariables)
{
    // Copy stack variables to the actual variables
    for (auto &[var, varPtr] : m_variablePtrs) {
        if (varPtr.onStack && varPtr.changed)
            createValueCopy(varPtr.stackPtr, getVariablePtr(targetVariables, var));

        varPtr.changed = false;
    }
}

void LLVMCodeBuilder::reloadVariables(llvm::Value *targetVariables)
{
    // Reset variables to use heap
    for (auto &[var, varPtr] : m_variablePtrs) {
        varPtr.onStack = false;
        varPtr.changed = false;
        varPtr.type = Compiler::StaticType::Unknown;
    }
}

void LLVMCodeBuilder::reloadLists()
{
    // Reset list data dirty and list types
    auto &typeMap = m_scopeLists.back();

    for (auto &[list, listPtr] : m_listPtrs) {
        m_builder.CreateStore(m_builder.getInt1(true), listPtr.dataPtrDirty);
        listPtr.type = Compiler::StaticType::Unknown;
        typeMap[&listPtr] = listPtr.type;
    }
}

void LLVMCodeBuilder::updateListDataPtr(const LLVMListPtr &listPtr, llvm::Function *func)
{
    // dataPtr = dirty ? list_data(list) : dataPtr
    // dirty = false
    llvm::Value *dirty = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.dataPtrDirty);
    llvm::Value *dataPtr = m_builder.CreateSelect(dirty, m_builder.CreateCall(resolve_list_data(), listPtr.ptr), m_builder.CreateLoad(m_valueDataType->getPointerTo(), listPtr.dataPtr));
    m_builder.CreateStore(dataPtr, listPtr.dataPtr);
    m_builder.CreateStore(m_builder.getInt1(false), listPtr.dataPtrDirty);
}

CompilerValue *LLVMCodeBuilder::createOp(const LLVMInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args)
{
    std::vector<Compiler::StaticType> types;
    types.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++)
        types.push_back(argType);

    return createOp(ins, retType, types, args);
}

CompilerValue *LLVMCodeBuilder::createOp(const LLVMInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    m_instructions.push_back(ins);
    LLVMInstruction &createdIns = m_instructions.back();

    for (size_t i = 0; i < args.size(); i++)
        createdIns.args.push_back({ argTypes[i], static_cast<LLVMRegister *>(args[i]) });

    if (retType != Compiler::StaticType::Void) {
        auto ret = std::make_shared<LLVMRegister>(retType);
        ret->isRawValue = true;
        createdIns.functionReturnReg = ret.get();
        return addReg(ret);
    }

    return nullptr;
}

void LLVMCodeBuilder::createValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType)
{
    llvm::Value *converted = nullptr;

    if (sourceType != Compiler::StaticType::Unknown)
        converted = castValue(reg, sourceType);

    auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [sourceType](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == sourceType; });
    const ValueType mappedType = it == TYPE_MAP.cend() ? ValueType::Number : it->first; // unknown type can be ignored

    switch (sourceType) {
        case Compiler::StaticType::Number:
            switch (targetType) {
                case Compiler::StaticType::Number: {
                    // Write number to number directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    break;
                }

                case Compiler::StaticType::Bool: {
                    // Write number to bool value directly and change type
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    llvm::Value *typePtr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 1);
                    m_builder.CreateStore(converted, ptr);
                    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typePtr);
                    break;
                }

                default:
                    m_builder.CreateCall(resolve_value_assign_double(), { targetPtr, converted });
                    break;
            }

            break;

        case Compiler::StaticType::Bool:
            switch (targetType) {
                case Compiler::StaticType::Number: {
                    // Write bool to number value directly and change type
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    llvm::Value *typePtr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 1);
                    m_builder.CreateStore(converted, ptr);
                    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typePtr);
                    break;
                }

                case Compiler::StaticType::Bool: {
                    // Write bool to bool directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    break;
                }

                default:
                    m_builder.CreateCall(resolve_value_assign_bool(), { targetPtr, converted });
                    break;
            }

            break;

        case Compiler::StaticType::String:
            m_builder.CreateCall(resolve_value_assign_cstring(), { targetPtr, converted });
            break;

        case Compiler::StaticType::Unknown:
            m_builder.CreateCall(resolve_value_assign_copy(), { targetPtr, reg->value });
            break;

        default:
            assert(false);
            break;
    }
}

void LLVMCodeBuilder::createReusedValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType)
{
    llvm::Value *converted = nullptr;

    if (sourceType != Compiler::StaticType::Unknown)
        converted = castValue(reg, sourceType);

    switch (sourceType) {
        case Compiler::StaticType::Number:
            m_builder.CreateCall(resolve_value_assign_double(), { targetPtr, converted });
            break;

        case Compiler::StaticType::Bool:
            m_builder.CreateCall(resolve_value_assign_bool(), { targetPtr, converted });
            break;

        case Compiler::StaticType::String:
            m_builder.CreateCall(resolve_value_assign_cstring(), { targetPtr, converted });
            break;

        case Compiler::StaticType::Unknown:
            m_builder.CreateCall(resolve_value_assign_copy(), { targetPtr, reg->value });
            break;

        default:
            assert(false);
            break;
    }
}

void LLVMCodeBuilder::createValueCopy(llvm::Value *source, llvm::Value *target)
{
    // NOTE: This doesn't copy strings, but only the pointers
    copyStructField(source, target, 0, m_valueDataType, m_builder.getInt64Ty()); // value
    copyStructField(source, target, 1, m_valueDataType, m_builder.getInt32Ty()); // type
    /* 2: padding */
    copyStructField(source, target, 3, m_valueDataType, m_builder.getInt64Ty()); // string size
}

void LLVMCodeBuilder::copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType)
{
    llvm::Value *sourceField = m_builder.CreateStructGEP(structType, source, index);
    llvm::Value *targetField = m_builder.CreateStructGEP(structType, target, index);
    m_builder.CreateStore(m_builder.CreateLoad(fieldType, sourceField), targetField);
}

llvm::Value *LLVMCodeBuilder::getListItem(const LLVMListPtr &listPtr, llvm::Value *index, llvm::Function *func)
{
    updateListDataPtr(listPtr, func);
    return m_builder.CreateGEP(m_valueDataType, m_builder.CreateLoad(m_valueDataType->getPointerTo(), listPtr.dataPtr), index);
}

llvm::Value *LLVMCodeBuilder::getListItemIndex(const LLVMListPtr &listPtr, LLVMRegister *item, llvm::Function *func)
{
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
    llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(m_ctx, "", func);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(m_ctx, "", func);
    llvm::BasicBlock *cmpIfBlock = llvm::BasicBlock::Create(m_ctx, "", func);
    llvm::BasicBlock *cmpElseBlock = llvm::BasicBlock::Create(m_ctx, "", func);
    llvm::BasicBlock *notFoundBlock = llvm::BasicBlock::Create(m_ctx, "", func);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_ctx, "", func);

    // index = 0
    llvm::Value *index = m_builder.CreateAlloca(m_builder.getInt64Ty());
    m_builder.CreateStore(m_builder.getInt64(0), index);
    m_builder.CreateBr(condBlock);

    // while (index < size)
    m_builder.SetInsertPoint(condBlock);
    llvm::Value *cond = m_builder.CreateICmpULT(m_builder.CreateLoad(m_builder.getInt64Ty(), index), size);
    m_builder.CreateCondBr(cond, bodyBlock, notFoundBlock);

    // if (list[index] == item)
    m_builder.SetInsertPoint(bodyBlock);
    LLVMRegister currentItem(listPtr.type);
    currentItem.isRawValue = false;
    currentItem.value = getListItem(listPtr, m_builder.CreateLoad(m_builder.getInt64Ty(), index), func);
    llvm::Value *cmp = createComparison(&currentItem, item, Comparison::EQ);
    m_builder.CreateCondBr(cmp, cmpIfBlock, cmpElseBlock);

    // goto nextBlock
    m_builder.SetInsertPoint(cmpIfBlock);
    m_builder.CreateBr(nextBlock);

    // else index++
    m_builder.SetInsertPoint(cmpElseBlock);
    m_builder.CreateStore(m_builder.CreateAdd(m_builder.CreateLoad(m_builder.getInt64Ty(), index), m_builder.getInt64(1)), index);
    m_builder.CreateBr(condBlock);

    // notFoundBlock:
    // index = -1
    // goto nextBlock
    m_builder.SetInsertPoint(notFoundBlock);
    m_builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_ctx), -1, true), index);
    m_builder.CreateBr(nextBlock);

    // nextBlock:
    m_builder.SetInsertPoint(nextBlock);

    return m_builder.CreateLoad(m_builder.getInt64Ty(), index);
}

llvm::Value *LLVMCodeBuilder::createValue(LLVMRegister *reg)
{
    if (reg->isConst()) {
        // Create a constant ValueData instance and store it
        llvm::Constant *value = castConstValue(reg->constValue(), TYPE_MAP[reg->constValue().type()]);
        llvm::Value *ret = m_builder.CreateAlloca(m_valueDataType);

        switch (reg->constValue().type()) {
            case ValueType::Number:
                value = llvm::ConstantExpr::getBitCast(value, m_valueDataType->getElementType(0));
                break;

            case ValueType::Bool:
                // Assuming union type is int64
                value = m_builder.getInt64(reg->constValue().toBool());
                break;

            case ValueType::String:
                value = llvm::ConstantExpr::getPtrToInt(value, m_valueDataType->getElementType(0));
                break;

            default:
                assert(false);
                break;
        }

        llvm::Constant *type = m_builder.getInt32(static_cast<uint32_t>(reg->constValue().type()));
        llvm::Constant *padding = m_builder.getInt32(0);
        llvm::Constant *constValue = llvm::ConstantStruct::get(m_valueDataType, { value, type, padding, m_builder.getInt64(0) });
        m_builder.CreateStore(constValue, ret);

        return ret;
    } else if (reg->isRawValue) {
        llvm::Value *value = castRawValue(reg, reg->type());
        llvm::Value *ret = m_builder.CreateAlloca(m_valueDataType);

        // Store value
        llvm::Value *valueField = m_builder.CreateStructGEP(m_valueDataType, ret, 0);
        m_builder.CreateStore(value, valueField);

        auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [&reg](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == reg->type(); });

        if (it == TYPE_MAP.end()) {
            assert(false);
            return nullptr;
        }

        // Store type
        llvm::Value *typeField = m_builder.CreateStructGEP(m_valueDataType, ret, 1);
        ValueType type = it->first;
        m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(type)), typeField);

        return ret;
    } else
        return reg->value;
}

llvm::Value *LLVMCodeBuilder::createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type)
{
    auto type1 = arg1->type();
    auto type2 = arg2->type();

    if (arg1->isConst() && arg2->isConst()) {
        // If both operands are constant, perform the comparison at compile time
        bool result = false;

        switch (type) {
            case Comparison::EQ:
                result = arg1->constValue() == arg2->constValue();
                break;

            case Comparison::GT:
                result = arg1->constValue() > arg2->constValue();
                break;

            case Comparison::LT:
                result = arg1->constValue() < arg2->constValue();
                break;

            default:
                assert(false);
                return nullptr;
        }

        return m_builder.getInt1(result);
    } else {
        // Optimize comparison of constant with number/bool
        if (arg1->isConst() && arg1->constValue().isValidNumber() && (type2 == Compiler::StaticType::Number || type2 == Compiler::StaticType::Bool))
            type1 = Compiler::StaticType::Number;

        if (arg2->isConst() && arg2->constValue().isValidNumber() && (type1 == Compiler::StaticType::Number || type1 == Compiler::StaticType::Bool))
            type2 = Compiler::StaticType::Number;

        // Optimize number and bool comparison
        int optNumberBool = 0;

        if (type1 == Compiler::StaticType::Number && type2 == Compiler::StaticType::Bool) {
            type2 = Compiler::StaticType::Number;
            optNumberBool = 2; // operand 2 was bool
        }

        if (type1 == Compiler::StaticType::Bool && type2 == Compiler::StaticType::Number) {
            type1 = Compiler::StaticType::Number;
            optNumberBool = 1; // operand 1 was bool
        }

        if (type1 != type2 || type1 == Compiler::StaticType::Unknown || type2 == Compiler::StaticType::Unknown) {
            // If the types are different or at least one of them
            // is unknown, we must use value functions
            llvm::Value *value1 = createValue(arg1);
            llvm::Value *value2 = createValue(arg2);

            switch (type) {
                case Comparison::EQ:
                    return m_builder.CreateCall(resolve_value_equals(), { value1, value2 });

                case Comparison::GT:
                    return m_builder.CreateCall(resolve_value_greater(), { value1, value2 });

                case Comparison::LT:
                    return m_builder.CreateCall(resolve_value_lower(), { value1, value2 });

                default:
                    assert(false);
                    return nullptr;
            }
        } else {
            // Compare raw values
            llvm::Value *value1 = castValue(arg1, type1);
            llvm::Value *value2 = castValue(arg2, type2);
            assert(type1 == type2);

            switch (type1) {
                case Compiler::StaticType::Number: {
                    // Compare two numbers
                    switch (type) {
                        case Comparison::EQ: {
                            llvm::Value *nan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOEQ(value1, value2);
                            return m_builder.CreateSelect(nan, m_builder.getInt1(true), cmp);
                        }

                        case Comparison::GT: {
                            llvm::Value *bothNan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOGT(value1, value2);
                            llvm::Value *nan;
                            llvm::Value *nanCmp;

                            if (optNumberBool == 1) {
                                nan = isNaN(value2);
                                nanCmp = castValue(arg1, Compiler::StaticType::Bool);
                            } else if (optNumberBool == 2) {
                                nan = isNaN(value1);
                                nanCmp = m_builder.CreateNot(castValue(arg2, Compiler::StaticType::Bool));
                            } else {
                                nan = isNaN(value1);
                                nanCmp = m_builder.CreateFCmpUGT(value1, value2);
                            }

                            return m_builder.CreateAnd(m_builder.CreateNot(bothNan), m_builder.CreateSelect(nan, nanCmp, cmp));
                        }

                        case Comparison::LT: {
                            llvm::Value *bothNan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOLT(value1, value2);
                            llvm::Value *nan;
                            llvm::Value *nanCmp;

                            if (optNumberBool == 1) {
                                nan = isNaN(value2);
                                nanCmp = m_builder.CreateNot(castValue(arg1, Compiler::StaticType::Bool));
                            } else if (optNumberBool == 2) {
                                nan = isNaN(value1);
                                nanCmp = castValue(arg2, Compiler::StaticType::Bool);
                            } else {
                                nan = isNaN(value2);
                                nanCmp = m_builder.CreateFCmpULT(value1, value2);
                            }

                            return m_builder.CreateAnd(m_builder.CreateNot(bothNan), m_builder.CreateSelect(nan, nanCmp, cmp));
                        }

                        default:
                            assert(false);
                            return nullptr;
                    }
                }

                case Compiler::StaticType::Bool:
                    // Compare two booleans
                    switch (type) {
                        case Comparison::EQ:
                            return m_builder.CreateICmpEQ(value1, value2);

                        case Comparison::GT:
                            // value1 && !value2
                            return m_builder.CreateAnd(value1, m_builder.CreateNot(value2));

                        case Comparison::LT:
                            // value2 && !value1
                            return m_builder.CreateAnd(value2, m_builder.CreateNot(value1));

                        default:
                            assert(false);
                            return nullptr;
                    }

                case Compiler::StaticType::String: {
                    // Compare two strings
                    llvm::Value *cmpRet = m_builder.CreateCall(resolve_strcasecmp(), { value1, value2 });

                    switch (type) {
                        case Comparison::EQ:
                            return m_builder.CreateICmpEQ(cmpRet, m_builder.getInt32(0));

                        case Comparison::GT:
                            return m_builder.CreateICmpSGT(cmpRet, m_builder.getInt32(0));

                        case Comparison::LT:
                            return m_builder.CreateICmpSLT(cmpRet, m_builder.getInt32(0));

                        default:
                            assert(false);
                            return nullptr;
                    }
                }

                default:
                    assert(false);
                    return nullptr;
            }
        }
    }
}

llvm::FunctionCallee LLVMCodeBuilder::resolveFunction(const std::string name, llvm::FunctionType *type)
{
    return m_module->getOrInsertFunction(name, type);
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_init()
{
    return resolveFunction("value_init", llvm::FunctionType::get(m_builder.getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_free()
{
    return resolveFunction("value_free", llvm::FunctionType::get(m_builder.getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_long()
{
    return resolveFunction("value_assign_long", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_double()
{
    return resolveFunction("value_assign_double", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getDoubleTy() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_bool()
{
    return resolveFunction("value_assign_bool", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_cstring()
{
    return resolveFunction("value_assign_cstring", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0) }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_special()
{
    return resolveFunction("value_assign_special", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt32Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_copy()
{
    return resolveFunction("value_assign_copy", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_valueDataType->getPointerTo() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toDouble()
{
    return resolveFunction("value_toDouble", llvm::FunctionType::get(m_builder.getDoubleTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toBool()
{
    return resolveFunction("value_toBool", llvm::FunctionType::get(m_builder.getInt1Ty(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toCString()
{
    return resolveFunction("value_toCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_doubleToCString()
{
    return resolveFunction("value_doubleToCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_builder.getDoubleTy(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_boolToCString()
{
    return resolveFunction("value_boolToCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_builder.getInt1Ty(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_stringToDouble()
{
    return resolveFunction("value_stringToDouble", llvm::FunctionType::get(m_builder.getDoubleTy(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_stringToBool()
{
    return resolveFunction("value_stringToBool", llvm::FunctionType::get(m_builder.getInt1Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_equals()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    return resolveFunction("value_equals", llvm::FunctionType::get(m_builder.getInt1Ty(), { valuePtr, valuePtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_greater()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    return resolveFunction("value_greater", llvm::FunctionType::get(m_builder.getInt1Ty(), { valuePtr, valuePtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_lower()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    return resolveFunction("value_lower", llvm::FunctionType::get(m_builder.getInt1Ty(), { valuePtr, valuePtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_clear()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_clear", llvm::FunctionType::get(m_builder.getVoidTy(), { listPtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_remove()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_remove", llvm::FunctionType::get(m_builder.getVoidTy(), { listPtr, m_builder.getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_append_empty()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_append_empty", llvm::FunctionType::get(m_valueDataType->getPointerTo(), { listPtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_insert_empty()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_insert_empty", llvm::FunctionType::get(m_valueDataType->getPointerTo(), { listPtr, m_builder.getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_data()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_data", llvm::FunctionType::get(m_valueDataType->getPointerTo(), { listPtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_size_ptr()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_size_ptr", llvm::FunctionType::get(m_builder.getInt64Ty()->getPointerTo()->getPointerTo(), { listPtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_list_alloc_size_ptr()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("list_alloc_size_ptr", llvm::FunctionType::get(m_builder.getInt64Ty()->getPointerTo()->getPointerTo(), { listPtr }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_strcasecmp()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("strcasecmp", llvm::FunctionType::get(m_builder.getInt32Ty(), { pointerType, pointerType }, false));
}
