// SPDX-License-Identifier: Apache-2.0

#include <llvm/IR/Verifier.h>

#include <scratchcpp/stage.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/compilerlocalvariable.h>

#include "llvmcodebuilder.h"
#include "llvmcompilercontext.h"
#include "llvmexecutablecode.h"
#include "llvmconstantregister.h"
#include "llvmifstatement.h"
#include "llvmloop.h"

using namespace libscratchcpp;

static const std::unordered_set<LLVMInstruction::Type>
    VAR_LIST_READ_INSTRUCTIONS = { LLVMInstruction::Type::ReadVariable, LLVMInstruction::Type::GetListItem, LLVMInstruction::Type::GetListItemIndex, LLVMInstruction::Type::ListContainsItem };

LLVMCodeBuilder::LLVMCodeBuilder(LLVMCompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_target(ctx->target()),
    m_llvmCtx(*ctx->llvmCtx()),
    m_module(ctx->module()),
    m_builder(m_llvmCtx),
    m_utils(ctx, m_builder),
    m_procedurePrototype(procedurePrototype),
    m_defaultWarp(procedurePrototype ? procedurePrototype->warp() : false),
    m_warp(m_defaultWarp),
    m_codeType(codeType)
{
    initTypes();

    llvm::FunctionType *funcType = getMainFunctionType(nullptr);
    m_defaultArgCount = funcType->getNumParams();
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::finalize()
{
    if (!m_warp) {
        // Do not create coroutine if there are no yield instructions nor non-warp procedure calls
        if (!m_instructions.containsInstruction([](const LLVMInstruction *step) {
                return step->type == LLVMInstruction::Type::Yield || (step->type == LLVMInstruction::Type::CallProcedure && step->procedurePrototype && !step->procedurePrototype->warp());
            })) {
            m_warp = true;
        }

        // Only create coroutines in scripts
        if (m_codeType != Compiler::CodeType::Script)
            m_warp = true;
    }

    // Set fast math flags
    llvm::FastMathFlags fmf;
    fmf.setFast(true);
    fmf.setNoInfs(false);
    fmf.setNoNaNs(false);
    fmf.setNoSignedZeros(false);
    m_builder.setFastMathFlags(fmf);

    // Create function
    std::string funcName = getMainFunctionName(m_procedurePrototype);
    llvm::FunctionType *funcType = getMainFunctionType(m_procedurePrototype);

    if (m_procedurePrototype)
        m_function = getOrCreateFunction(funcName, funcType);
    else
        m_function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcName, m_module);

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_llvmCtx, "entry", m_function);
    llvm::BasicBlock *endBranch = llvm::BasicBlock::Create(m_llvmCtx, "end", m_function);
    m_builder.SetInsertPoint(entry);

    // Init coroutine
    std::unique_ptr<LLVMCoroutine> coro;

    if (!m_warp)
        coro = std::make_unique<LLVMCoroutine>(m_module, &m_builder, m_function);

    std::vector<LLVMIfStatement> ifStatements;
    std::vector<LLVMLoop> loops;

    m_utils.init(m_function, m_procedurePrototype, m_warp);

    // Execute recorded instructions
    LLVMInstruction *ins = m_instructions.first();

    while (ins) {
        switch (ins->type) {
            case LLVMInstruction::Type::FunctionCall: {
                std::vector<llvm::Type *> types;
                std::vector<llvm::Value *> args;

                // Variables must be synchronized because the function can read them
                m_utils.syncVariables(m_utils.targetVariables());

                // Add execution context arg
                if (ins->functionCtxArg) {
                    types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0));
                    args.push_back(m_utils.executionContextPtr());
                }

                // Add target pointer arg
                if (ins->functionTargetArg) {
                    types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0));
                    args.push_back(m_utils.targetPtr());
                }

                // Args
                for (auto &arg : ins->args) {
                    types.push_back(m_utils.getType(arg.first));
                    args.push_back(m_utils.castValue(arg.second, arg.first));
                }

                llvm::Type *retType = m_utils.getType(ins->functionReturnReg ? ins->functionReturnReg->type() : Compiler::StaticType::Void);
                llvm::Value *ret = m_builder.CreateCall(m_utils.functions().resolveFunction(ins->functionName, llvm::FunctionType::get(retType, types, false)), args);

                if (ins->functionReturnReg) {
                    ins->functionReturnReg->value = ret;

                    if (ins->functionReturnReg->type() == Compiler::StaticType::String)
                        m_utils.freeStringLater(ins->functionReturnReg->value);
                }

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Add: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
                llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
                ins->functionReturnReg->value = m_builder.CreateFAdd(num1, num2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Sub: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
                llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
                ins->functionReturnReg->value = m_builder.CreateFSub(num1, num2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Mul: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
                llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
                ins->functionReturnReg->value = m_builder.CreateFMul(num1, num2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Div: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
                llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
                ins->functionReturnReg->value = m_builder.CreateFDiv(num1, num2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Random: {
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
                        ins->functionReturnReg->value =
                            m_builder.CreateSelect(isInfOrNaN, sum, m_builder.CreateCall(m_utils.functions().resolve_llvm_random(), { m_utils.executionContextPtr(), value1, value2 }));
                    }
                }

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::RandomInt: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *from = m_builder.CreateFPToSI(m_utils.castValue(arg1.second, arg1.first), m_builder.getInt64Ty());
                llvm::Value *to = m_builder.CreateFPToSI(m_utils.castValue(arg2.second, arg2.first), m_builder.getInt64Ty());
                ins->functionReturnReg->value = m_builder.CreateCall(m_utils.functions().resolve_llvm_random_long(), { m_utils.executionContextPtr(), from, to });

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::CmpEQ: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0].second;
                const auto &arg2 = ins->args[1].second;
                ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::EQ);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::CmpGT: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0].second;
                const auto &arg2 = ins->args[1].second;
                ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::GT);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::CmpLT: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0].second;
                const auto &arg2 = ins->args[1].second;
                ins->functionReturnReg->value = m_utils.createComparison(arg1, arg2, LLVMBuildUtils::Comparison::LT);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::StrCmpEQCS: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0].second;
                const auto &arg2 = ins->args[1].second;
                ins->functionReturnReg->value = m_utils.createStringComparison(arg1, arg2, true);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::StrCmpEQCI: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0].second;
                const auto &arg2 = ins->args[1].second;
                ins->functionReturnReg->value = m_utils.createStringComparison(arg1, arg2, false);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::And: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *bool1 = m_utils.castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = m_utils.castValue(arg2.second, arg2.first);
                ins->functionReturnReg->value = m_builder.CreateAnd(bool1, bool2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Or: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *bool1 = m_utils.castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = m_utils.castValue(arg2.second, arg2.first);
                ins->functionReturnReg->value = m_builder.CreateOr(bool1, bool2);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Not: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                llvm::Value *value = m_utils.castValue(arg.second, arg.first);
                ins->functionReturnReg->value = m_builder.CreateNot(value);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Mod: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                // rem(a, b) / b < 0.0 ? rem(a, b) + b : rem(a, b)
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Value *num1 = m_utils.removeNaN(m_utils.castValue(arg1.second, arg1.first));
                llvm::Value *num2 = m_utils.removeNaN(m_utils.castValue(arg2.second, arg2.first));
                llvm::Value *value = m_builder.CreateFRem(num1, num2);                                // rem(a, b)
                llvm::Value *cond = m_builder.CreateFCmpOLT(m_builder.CreateFDiv(value, num2), zero); // rem(a, b) / b < 0.0                                                            // rem(a, b)
                ins->functionReturnReg->value = m_builder.CreateSelect(cond, m_builder.CreateFAdd(value, num2), value);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Round: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // x >= 0.0 ? round(x) : (x >= -0.5 ? -0.0 : floor(x + 0.5))
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Constant *negativeZero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(-0.0));
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::floor, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                llvm::Value *notNegative = m_builder.CreateFCmpOGE(num, zero);                                                                                 // num >= 0.0
                llvm::Value *roundNum = m_builder.CreateCall(roundFunc, num);                                                                                  // round(num)
                llvm::Value *negativeCond = m_builder.CreateFCmpOGE(num, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(-0.5)));                               // num >= -0.5
                llvm::Value *negativeRound = m_builder.CreateCall(floorFunc, m_builder.CreateFAdd(num, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.5)))); // floor(x + 0.5)
                ins->functionReturnReg->value = m_builder.CreateSelect(notNegative, roundNum, m_builder.CreateSelect(negativeCond, negativeZero, negativeRound));

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Abs: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                llvm::Function *absFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::fabs, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(absFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Floor: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::floor, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(floorFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Ceil: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                llvm::Function *ceilFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::ceil, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(ceilFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Sqrt: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // sqrt(x) + 0.0
                // This avoids negative zero
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Function *sqrtFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::sqrt, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateCall(sqrtFunc, num), zero);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Sin: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // round(sin(x * pi / 180.0) * 1e10) / 1e10 + 0.0
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(1e10));
                llvm::Function *sinFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::sin, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                llvm::Value *sinResult = m_builder.CreateCall(sinFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // sin(x * pi / 180)
                llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(sinResult, factor));                    // round(sin(x * 180) * 1e10)
                ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(rounded, factor), zero);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Cos: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // round(cos(x * pi / 180.0) * 1e10) / 1e10
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(1e10));
                llvm::Function *cosFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::cos, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::round, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                llvm::Value *cosResult = m_builder.CreateCall(cosFunc, m_builder.CreateFDiv(m_builder.CreateFMul(num, pi), piDeg)); // cos(x * pi / 180)
                llvm::Value *rounded = m_builder.CreateCall(roundFunc, m_builder.CreateFMul(cosResult, factor));                    // round(cos(x * 180) * 1e10)
                ins->functionReturnReg->value = m_builder.CreateFDiv(rounded, factor);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Tan: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // ((mod = rem(x, 360.0)) == -270.0 || mod == 90.0) ? inf : ((mod == -90.0 || mod == 270.0) ? -inf : round(tan(x * pi / 180.0) * 1e10) / 1e10 + 0.0)
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Constant *full = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(360.0));
                llvm::Constant *posInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false);
                llvm::Constant *negInf = llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), true);
                llvm::Constant *undefined1 = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(-270.0));
                llvm::Constant *undefined2 = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(90.0));
                llvm::Constant *undefined3 = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(-90.0));
                llvm::Constant *undefined4 = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(270.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Constant *factor = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(1e10));
                llvm::Function *tanFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::tan, m_builder.getDoubleTy());
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::round, m_builder.getDoubleTy());
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

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Asin: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // asin(x) * 180.0 / pi + 0.0
                // +0.0 to avoid -0.0
                llvm::Constant *zero = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Function *asinFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::asin, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateFAdd(m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(asinFunc, num), piDeg), pi), zero);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Acos: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // acos(x) * 180.0 / pi
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Function *acosFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::acos, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(acosFunc, num), piDeg), pi);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Atan: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // atan(x) * 180.0 / pi
                llvm::Constant *pi = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(std::acos(-1.0)));
                llvm::Constant *piDeg = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(180.0));
                llvm::Function *atanFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::atan, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateFDiv(m_builder.CreateFMul(m_builder.CreateCall(atanFunc, num), piDeg), pi);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Ln: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // log(x)
                llvm::Function *logFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::log, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(logFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Log10: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // log10(x)
                llvm::Function *log10Func = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::log10, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(log10Func, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Exp: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // exp(x)
                llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::exp, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(expFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Exp10: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                // exp10(x)
                llvm::Function *expFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::exp10, m_builder.getDoubleTy());
                llvm::Value *num = m_utils.removeNaN(m_utils.castValue(arg.second, arg.first));
                ins->functionReturnReg->value = m_builder.CreateCall(expFunc, num);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::StringConcat: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *str1 = m_utils.castValue(arg1.second, arg1.first);
                llvm::Value *str2 = m_utils.castValue(arg2.second, arg2.first);
                llvm::PointerType *charPointerType = m_builder.getInt16Ty()->getPointerTo();
                llvm::Function *memcpyFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::memcpy_inline, { charPointerType, charPointerType, m_builder.getInt64Ty() });

                // StringPtr *result = string_pool_new(true)
                llvm::Value *result = m_builder.CreateCall(m_utils.functions().resolve_string_pool_new(), m_builder.getInt1(true));
                m_utils.freeStringLater(result);

                // result->size = string1->size + string2->size
                llvm::Value *sizeField1 = m_builder.CreateStructGEP(m_stringPtrType, str1, 1);
                llvm::Value *sizeField2 = m_builder.CreateStructGEP(m_stringPtrType, str2, 1);
                llvm::Value *size1 = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField1);
                llvm::Value *size2 = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField2);
                llvm::Value *resultSize = m_builder.CreateAdd(size1, size2);
                llvm::Value *resultSizeField = m_builder.CreateStructGEP(m_stringPtrType, result, 1);
                m_builder.CreateStore(resultSize, resultSizeField);

                // string_alloc(result, result->size)
                m_builder.CreateCall(m_utils.functions().resolve_string_alloc(), { result, resultSize });

                // memcpy(result->data, string1->data, string1->size * sizeof(char16_t))
                llvm::Value *dataField1 = m_builder.CreateStructGEP(m_stringPtrType, str1, 0);
                llvm::Value *data1 = m_builder.CreateLoad(charPointerType, dataField1);
                llvm::Value *resultDataField = m_builder.CreateStructGEP(m_stringPtrType, result, 0);
                llvm::Value *writePtr = m_builder.CreateLoad(charPointerType, resultDataField);
                m_builder.CreateCall(memcpyFunc, { writePtr, data1, m_builder.CreateMul(size1, m_builder.getInt64(2)), m_builder.getInt1(false) });

                // memcpy(result->data + string1->size, string2->data, (string2->size + 1) * sizeof(char16_t))
                // +1: null-terminate
                llvm::Value *dataField2 = m_builder.CreateStructGEP(m_stringPtrType, str2, 0);
                llvm::Value *data2 = m_builder.CreateLoad(charPointerType, dataField2);
                writePtr = m_builder.CreateGEP(m_builder.getInt16Ty(), writePtr, size1);
                m_builder.CreateCall(memcpyFunc, { writePtr, data2, m_builder.CreateMul(m_builder.CreateAdd(size2, m_builder.getInt64(1)), m_builder.getInt64(2)), m_builder.getInt1(false) });

                ins->functionReturnReg->value = result;

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::StringChar: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *str = m_utils.castValue(arg1.second, arg1.first);
                llvm::Value *index = m_builder.CreateFPToSI(m_utils.castValue(arg2.second, arg2.first), m_builder.getInt64Ty());
                llvm::PointerType *charPointerType = m_builder.getInt16Ty()->getPointerTo();

                // Get data ptr and size
                llvm::Value *dataField = m_builder.CreateStructGEP(m_stringPtrType, str, 0);
                llvm::Value *data = m_builder.CreateLoad(charPointerType, dataField);
                llvm::Value *sizeField = m_builder.CreateStructGEP(m_stringPtrType, str, 1);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField);

                // Check range, get character ptr
                llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateICmpSGE(index, m_builder.getInt64(0)), m_builder.CreateICmpSLT(index, size));
                llvm::Value *charPtr = m_builder.CreateGEP(m_builder.getInt16Ty(), data, index);

                // Allocate string
                llvm::Value *result = m_builder.CreateCall(m_utils.functions().resolve_string_pool_new(), m_builder.getInt1(true));
                m_utils.freeStringLater(result);
                m_builder.CreateCall(m_utils.functions().resolve_string_alloc(), { result, m_builder.getInt64(1) }); // size 1 to avoid branching

                // Get result data ptr
                dataField = m_builder.CreateStructGEP(m_stringPtrType, result, 0);
                data = m_builder.CreateLoad(charPointerType, dataField);

                // Write result
                llvm::Value *char1 = m_builder.CreateGEP(m_builder.getInt16Ty(), data, m_builder.getInt64(0));
                llvm::Value *char2 = m_builder.CreateGEP(m_builder.getInt16Ty(), data, m_builder.getInt64(1));
                sizeField = m_builder.CreateStructGEP(m_stringPtrType, result, 1);
                m_builder.CreateStore(m_builder.CreateSelect(inRange, m_builder.CreateLoad(m_builder.getInt16Ty(), charPtr), m_builder.getInt16(0)), char1);
                m_builder.CreateStore(m_builder.getInt16(0), char2);
                m_builder.CreateStore(m_builder.CreateSelect(inRange, m_builder.getInt64(1), m_builder.getInt64(0)), sizeField);

                ins->functionReturnReg->value = result;

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::StringLength: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                llvm::Value *str = m_utils.castValue(arg.second, arg.first);
                llvm::Value *sizeField = m_builder.CreateStructGEP(m_stringPtrType, str, 1);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), sizeField);
                ins->functionReturnReg->value = m_builder.CreateSIToFP(size, m_builder.getDoubleTy());

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Select: {
                assert(ins->args.size() == 3);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                const auto &arg3 = ins->args[2];
                auto type = arg2.first;
                llvm::Value *cond = m_utils.castValue(arg1.second, arg1.first);
                llvm::Value *trueValue;
                llvm::Value *falseValue;

                if (type == Compiler::StaticType::Unknown) {
                    trueValue = m_utils.createValue(arg2.second);
                    falseValue = m_utils.createValue(arg3.second);
                } else {
                    trueValue = m_utils.castValue(arg2.second, type);
                    falseValue = m_utils.castValue(arg3.second, type);
                }

                ins->functionReturnReg->value = m_builder.CreateSelect(cond, trueValue, falseValue);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::CreateLocalVariable: {
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

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::WriteLocalVariable: {
                assert(ins->args.size() == 2);
                const auto &arg1 = ins->args[0];
                const auto &arg2 = ins->args[1];
                llvm::Value *converted = m_utils.castValue(arg2.second, arg2.first);
                m_builder.CreateStore(converted, arg1.second->value);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ReadLocalVariable: {
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

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::WriteVariable: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                Compiler::StaticType argType = m_utils.optimizeRegisterType(arg.second);
                LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->workVariable);
                varPtr.changed = true; // TODO: Handle loops and if statements

                Compiler::StaticType varType = Compiler::StaticType::Unknown;

                if (m_warp)
                    varType = m_typeAnalyzer.variableType(ins->workVariable, ins, Compiler::StaticType::Unknown);

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

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ReadVariable: {
                assert(ins->args.size() == 0);
                LLVMVariablePtr &varPtr = m_utils.variablePtr(ins->workVariable);
                Compiler::StaticType type = Compiler::StaticType::Unknown;

                if (m_warp)
                    type = m_typeAnalyzer.variableType(ins->workVariable, ins, Compiler::StaticType::Unknown);

                ins->functionReturnReg->value = varPtr.onStack && !(ins->loopCondition && !m_warp) ? varPtr.stackPtr : varPtr.heapPtr;
                ins->functionReturnReg->setType(type);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ClearList: {
                assert(ins->args.size() == 0);
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
                m_builder.CreateCall(m_utils.functions().resolve_list_clear(), listPtr.ptr);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::RemoveListItem: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                // Range check
                llvm::Value *min = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
                llvm::Value *index = m_utils.castValue(arg.second, arg.first);
                llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));
                llvm::BasicBlock *removeBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.CreateCondBr(inRange, removeBlock, nextBlock);

                // Remove
                m_builder.SetInsertPoint(removeBlock);
                index = m_builder.CreateFPToUI(m_utils.castValue(arg.second, arg.first), m_builder.getInt64Ty());
                m_builder.CreateCall(m_utils.functions().resolve_list_remove(), { listPtr.ptr, index });
                m_builder.CreateBr(nextBlock);

                m_builder.SetInsertPoint(nextBlock);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::AppendToList: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                Compiler::StaticType type = m_utils.optimizeRegisterType(arg.second);
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                // Check if enough space is allocated
                llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                llvm::Value *isAllocated = m_builder.CreateICmpUGT(allocatedSize, size);
                llvm::BasicBlock *ifBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.CreateCondBr(isAllocated, ifBlock, elseBlock);

                // If there's enough space, use the allocated memory
                m_builder.SetInsertPoint(ifBlock);
                llvm::Value *itemPtr = m_utils.getListItem(listPtr, size);
                m_utils.createReusedValueStore(arg.second, itemPtr, type, listType);
                m_builder.CreateStore(m_builder.CreateAdd(size, m_builder.getInt64(1)), listPtr.sizePtr);
                m_builder.CreateBr(nextBlock);

                // Otherwise call appendEmpty()
                m_builder.SetInsertPoint(elseBlock);
                itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_append_empty(), listPtr.ptr);
                m_utils.createReusedValueStore(arg.second, itemPtr, type, listType);
                m_builder.CreateBr(nextBlock);

                m_builder.SetInsertPoint(nextBlock);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::InsertToList: {
                assert(ins->args.size() == 2);
                const auto &indexArg = ins->args[0];
                const auto &valueArg = ins->args[1];
                Compiler::StaticType type = m_utils.optimizeRegisterType(valueArg.second);
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                // Range check
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                llvm::Value *min = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
                llvm::Value *index = m_utils.castValue(indexArg.second, indexArg.first);
                llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLE(index, size));
                llvm::BasicBlock *insertBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.CreateCondBr(inRange, insertBlock, nextBlock);

                // Insert
                m_builder.SetInsertPoint(insertBlock);
                index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());
                llvm::Value *itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_insert_empty(), { listPtr.ptr, index });
                m_utils.createReusedValueStore(valueArg.second, itemPtr, type, listType);

                m_builder.CreateBr(nextBlock);

                m_builder.SetInsertPoint(nextBlock);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ListReplace: {
                assert(ins->args.size() == 2);
                const auto &indexArg = ins->args[0];
                const auto &valueArg = ins->args[1];
                Compiler::StaticType type = m_utils.optimizeRegisterType(valueArg.second);
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                // Range check
                llvm::Value *min = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
                llvm::Value *index = m_utils.castValue(indexArg.second, indexArg.first);
                llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));
                llvm::BasicBlock *replaceBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.CreateCondBr(inRange, replaceBlock, nextBlock);

                // Replace
                m_builder.SetInsertPoint(replaceBlock);
                index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());
                llvm::Value *itemPtr = m_utils.getListItem(listPtr, index);
                m_utils.createValueStore(valueArg.second, itemPtr, type, listType);
                m_builder.CreateBr(nextBlock);

                m_builder.SetInsertPoint(nextBlock);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::GetListContents: {
                assert(ins->args.size() == 0);
                const LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
                llvm::Value *ptr = m_builder.CreateCall(m_utils.functions().resolve_list_to_string(), listPtr.ptr);
                m_utils.freeStringLater(ptr);
                ins->functionReturnReg->value = ptr;

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::GetListItem: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                llvm::Value *min = llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0));
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
                llvm::Value *index = m_utils.castValue(arg.second, arg.first);
                llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));

                LLVMConstantRegister nullReg(listType == Compiler::StaticType::Unknown ? Compiler::StaticType::Number : listType, Value());
                llvm::Value *null = m_utils.createValue(static_cast<LLVMRegister *>(&nullReg));

                index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());
                ins->functionReturnReg->value = m_builder.CreateSelect(inRange, m_utils.getListItem(listPtr, index), null);
                ins->functionReturnReg->setType(listType);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::GetListSize: {
                assert(ins->args.size() == 0);
                const LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
                llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
                ins->functionReturnReg->value = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::GetListItemIndex: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                ins->functionReturnReg->value = m_builder.CreateSIToFP(m_utils.getListItemIndex(listPtr, listType, arg.second), m_builder.getDoubleTy());

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ListContainsItem: {
                assert(ins->args.size() == 1);
                const auto &arg = ins->args[0];
                LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

                Compiler::StaticType listType = Compiler::StaticType::Unknown;

                if (m_warp)
                    listType = m_typeAnalyzer.listType(ins->workList, ins, Compiler::StaticType::Unknown, false);

                llvm::Value *index = m_utils.getListItemIndex(listPtr, listType, arg.second);
                ins->functionReturnReg->value = m_builder.CreateICmpSGT(index, llvm::ConstantInt::get(m_builder.getInt64Ty(), -1, true));

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Yield:
                createSuspend(coro.get(), m_utils.warpArg(), m_utils.targetVariables());

                ins = ins->next;
                break;

            case LLVMInstruction::Type::BeginIf: {
                LLVMIfStatement statement;
                statement.beforeIf = m_builder.GetInsertBlock();
                statement.body = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                // Use last reg
                assert(ins->args.size() == 1);
                const auto &reg = ins->args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                statement.condition = m_utils.castValue(reg.second, reg.first);

                // Switch to body branch
                m_builder.SetInsertPoint(statement.body);

                ifStatements.push_back(statement);
                m_utils.pushScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::BeginElse: {
                assert(!ifStatements.empty());
                LLVMIfStatement &statement = ifStatements.back();

                // Jump to the branch after the if statement
                assert(!statement.afterIf);
                statement.afterIf = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_utils.freeScopeHeap();
                m_builder.CreateBr(statement.afterIf);

                // Create else branch
                assert(!statement.elseBranch);
                statement.elseBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                // Since there's an else branch, the conditional instruction should jump to it
                m_builder.SetInsertPoint(statement.beforeIf);
                m_builder.CreateCondBr(statement.condition, statement.body, statement.elseBranch);

                // Switch to the else branch
                m_builder.SetInsertPoint(statement.elseBranch);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::EndIf: {
                assert(!ifStatements.empty());
                LLVMIfStatement &statement = ifStatements.back();
                m_utils.freeScopeHeap();

                // Jump to the branch after the if statement
                if (!statement.afterIf)
                    statement.afterIf = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

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
                m_utils.popScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::BeginRepeatLoop: {
                LLVMLoop loop;
                loop.isRepeatLoop = true;

                // index = 0
                llvm::Constant *zero = llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true);
                loop.index = m_utils.addAlloca(m_builder.getInt64Ty());
                m_builder.CreateStore(zero, loop.index);

                // Create branches
                llvm::BasicBlock *roundBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                loop.conditionBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                loop.afterLoop = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                // Use last reg for count
                assert(ins->args.size() == 1);
                const auto &reg = ins->args[0];
                assert(reg.first == Compiler::StaticType::Number);
                llvm::Value *count = m_utils.castValue(reg.second, reg.first);
                llvm::Value *isInf = m_builder.CreateFCmpOEQ(count, llvm::ConstantFP::getInfinity(m_builder.getDoubleTy(), false));

                // Clamp count if <= 0 (we can skip the loop if count is not positive)
                llvm::Value *comparison = m_builder.CreateFCmpULE(count, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0)));
                m_builder.CreateCondBr(comparison, loop.afterLoop, roundBranch);

                // Round (Scratch-specific behavior)
                m_builder.SetInsertPoint(roundBranch);
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module, llvm::Intrinsic::round, { count->getType() });
                count = m_builder.CreateCall(roundFunc, { count });
                count = m_builder.CreateFPToUI(count, m_builder.getInt64Ty()); // cast to unsigned integer
                count = m_builder.CreateSelect(isInf, zero, count);

                // Jump to condition branch
                m_builder.CreateBr(loop.conditionBranch);

                // Check index
                m_builder.SetInsertPoint(loop.conditionBranch);

                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                if (!loop.afterLoop)
                    loop.afterLoop = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                comparison = m_builder.CreateOr(isInf, m_builder.CreateICmpULT(currentIndex, count));
                m_builder.CreateCondBr(comparison, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);

                loops.push_back(loop);
                m_utils.pushScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::LoopIndex: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();
                llvm::Value *index = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                ins->functionReturnReg->value = m_builder.CreateUIToFP(index, m_builder.getDoubleTy());

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::BeginWhileLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                loop.afterLoop = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                // Use last reg
                assert(ins->args.size() == 1);
                const auto &reg = ins->args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = m_utils.castValue(reg.second, reg.first);
                m_builder.CreateCondBr(condition, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                m_utils.pushScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::BeginRepeatUntilLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                loop.afterLoop = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

                // Use last reg
                assert(ins->args.size() == 1);
                const auto &reg = ins->args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = m_utils.castValue(reg.second, reg.first);
                m_builder.CreateCondBr(condition, loop.afterLoop, body);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                m_utils.pushScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::BeginLoopCondition: {
                LLVMLoop loop;
                loop.isRepeatLoop = false;
                loop.conditionBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.CreateBr(loop.conditionBranch);
                m_builder.SetInsertPoint(loop.conditionBranch);
                loops.push_back(loop);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::EndLoop: {
                assert(!loops.empty());
                LLVMLoop &loop = loops.back();

                if (loop.isRepeatLoop) {
                    // Increment index
                    llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                    llvm::Value *incremented = m_builder.CreateAdd(currentIndex, llvm::ConstantInt::get(m_builder.getInt64Ty(), 1, false));
                    m_builder.CreateStore(incremented, loop.index);
                }

                // Jump to the condition branch
                m_utils.freeScopeHeap();
                m_builder.CreateBr(loop.conditionBranch);

                // Switch to the branch after the loop
                m_builder.SetInsertPoint(loop.afterLoop);

                loops.pop_back();
                m_utils.popScopeLevel();

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::Stop: {
                m_utils.freeScopeHeap();
                m_builder.CreateBr(endBranch);
                llvm::BasicBlock *nextBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                m_builder.SetInsertPoint(nextBranch);

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::CallProcedure: {
                assert(ins->procedurePrototype);
                assert(ins->args.size() == ins->procedurePrototype->argumentTypes().size());
                m_utils.freeScopeHeap();
                m_utils.syncVariables(m_utils.targetVariables());

                std::string name = getMainFunctionName(ins->procedurePrototype);
                llvm::FunctionType *type = getMainFunctionType(ins->procedurePrototype);
                std::vector<llvm::Value *> args;

                for (size_t i = 0; i < m_defaultArgCount; i++)
                    args.push_back(m_function->getArg(i));

                // Add warp arg
                if (m_warp)
                    args.push_back(m_builder.getInt1(true));
                else
                    args.push_back(m_utils.procedurePrototype() ? m_utils.warpArg() : m_builder.getInt1(false));

                // Add procedure args
                for (const auto &arg : ins->args) {
                    if (arg.first == Compiler::StaticType::Unknown)
                        args.push_back(m_utils.createValue(arg.second));
                    else
                        args.push_back(m_utils.castValue(arg.second, arg.first));
                }

                llvm::Value *handle = m_builder.CreateCall(m_utils.functions().resolveFunction(name, type), args);

                if (!m_warp && !ins->procedurePrototype->warp()) {
                    llvm::BasicBlock *suspendBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                    llvm::BasicBlock *nextBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
                    m_builder.CreateCondBr(m_builder.CreateIsNull(handle), nextBranch, suspendBranch);

                    m_builder.SetInsertPoint(suspendBranch);
                    createSuspend(coro.get(), m_utils.warpArg(), m_utils.targetVariables());
                    llvm::Value *done = m_builder.CreateCall(m_ctx->coroutineResumeFunction(), { handle });
                    m_builder.CreateCondBr(done, nextBranch, suspendBranch);

                    m_builder.SetInsertPoint(nextBranch);
                }

                m_utils.reloadVariables(m_utils.targetVariables());

                ins = ins->next;
                break;
            }

            case LLVMInstruction::Type::ProcedureArg: {
                assert(m_procedurePrototype);
                llvm::Value *arg = m_function->getArg(m_defaultArgCount + 1 + ins->procedureArgIndex); // omit warp arg
                ins->functionReturnReg->value = arg;

                ins = ins->next;
                break;
            }
        }
    }

    m_utils.end();
    m_builder.CreateBr(endBranch);

    m_builder.SetInsertPoint(endBranch);
    m_utils.syncVariables(m_utils.targetVariables());

    // End and verify the function
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0);

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            if (m_warp)
                m_builder.CreateRet(llvm::ConstantPointerNull::get(pointerType));
            else
                coro->end();
            break;

        case Compiler::CodeType::Reporter: {
            // Use last instruction return value (or last constant) and create a ValueData instance
            assert(!m_instructions.empty() || m_lastConstValue);
            LLVMRegister *ret = m_instructions.empty() ? m_lastConstValue : m_instructions.last()->functionReturnReg;
            llvm::Value *copy = m_utils.createNewValue(ret);
            m_builder.CreateRet(m_builder.CreateLoad(m_valueDataType, copy));
            break;
        }

        case Compiler::CodeType::HatPredicate:
            // Use last instruction return value (or last constant)
            assert(!m_instructions.empty() || m_lastConstValue);

            if (m_instructions.empty())
                m_builder.CreateRet(m_utils.castValue(m_lastConstValue, Compiler::StaticType::Bool));
            else
                m_builder.CreateRet(m_utils.castValue(m_instructions.last()->functionReturnReg, Compiler::StaticType::Bool));
            break;
    }

    verifyFunction(m_function);

    return std::make_shared<LLVMExecutableCode>(m_ctx, m_function->getName().str(), m_ctx->coroutineResumeFunction()->getName().str(), m_codeType);
}

CompilerValue *LLVMCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    assert(argTypes.size() == args.size());

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, m_loopCondition);
    ins->functionName = functionName;

    for (size_t i = 0; i < args.size(); i++)
        ins->args.push_back({ argTypes[i], dynamic_cast<LLVMRegister *>(args[i]) });

    m_instructions.addInstruction(ins);

    if (returnType != Compiler::StaticType::Void) {
        auto reg = std::make_shared<LLVMRegister>(returnType);
        reg->isRawValue = true;
        ins->functionReturnReg = reg.get();
        return addReg(reg, ins);
    }

    return nullptr;
}

CompilerValue *LLVMCodeBuilder::addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    CompilerValue *ret = addFunctionCall(functionName, returnType, argTypes, args);
    m_instructions.last()->functionTargetArg = true;
    return ret;
}

CompilerValue *LLVMCodeBuilder::addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    CompilerValue *ret = addFunctionCall(functionName, returnType, argTypes, args);
    m_instructions.last()->functionCtxArg = true;
    return ret;
}

CompilerConstant *LLVMCodeBuilder::addConstValue(const Value &value)
{
    auto constReg = std::make_shared<LLVMConstantRegister>(m_utils.mapType(value.type()), value);
    auto reg = std::static_pointer_cast<LLVMRegister>(constReg);
    m_lastConstValue = reg.get();
    return static_cast<CompilerConstant *>(static_cast<LLVMConstantRegister *>(addReg(reg, nullptr)));
}

CompilerValue *LLVMCodeBuilder::addStringChar(CompilerValue *string, CompilerValue *index)
{
    return createOp(LLVMInstruction::Type::StringChar, Compiler::StaticType::String, { Compiler::StaticType::String, Compiler::StaticType::Number }, { string, index });
}

CompilerValue *LLVMCodeBuilder::addStringLength(CompilerValue *string)
{
    return createOp(LLVMInstruction::Type::StringLength, Compiler::StaticType::Number, Compiler::StaticType::String, { string });
}

CompilerValue *LLVMCodeBuilder::addLoopIndex()
{
    return createOp(LLVMInstruction::Type::LoopIndex, Compiler::StaticType::Number, {}, {});
}

CompilerValue *LLVMCodeBuilder::addLocalVariableValue(CompilerLocalVariable *variable)
{
    return createOp(LLVMInstruction::Type::ReadLocalVariable, variable->type(), variable->type(), { variable->ptr() });
}

CompilerValue *LLVMCodeBuilder::addVariableValue(Variable *variable)
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, m_loopCondition);
    ins->workVariable = variable;
    m_utils.createVariablePtr(variable);

    auto ret = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    ret->isRawValue = false;
    ins->functionReturnReg = ret.get();

    m_instructions.addInstruction(ins);
    return addReg(ret, ins);
}

CompilerValue *LLVMCodeBuilder::addListContents(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListContents, m_loopCondition);
    ins.workList = list;
    m_utils.createListPtr(list);

    return createOp(ins, Compiler::StaticType::String);
}

CompilerValue *LLVMCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, m_loopCondition);
    ins->workList = list;
    m_utils.createListPtr(list);

    ins->args.push_back({ Compiler::StaticType::Number, dynamic_cast<LLVMRegister *>(index) });

    auto ret = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    ret->isRawValue = false;
    ins->functionReturnReg = ret.get();

    m_instructions.addInstruction(ins);
    return addReg(ret, ins);
}

CompilerValue *LLVMCodeBuilder::addListItemIndex(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListItemIndex, m_loopCondition);
    ins.workList = list;
    m_utils.createListPtr(list);

    auto ret = createOp(ins, Compiler::StaticType::Number, Compiler::StaticType::Unknown, { item });
    return ret;
}

CompilerValue *LLVMCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListContainsItem, m_loopCondition);
    ins.workList = list;
    m_utils.createListPtr(list);

    auto ret = createOp(ins, Compiler::StaticType::Bool, Compiler::StaticType::Unknown, { item });
    return ret;
}

CompilerValue *LLVMCodeBuilder::addListSize(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListSize, m_loopCondition);
    ins.workList = list;
    m_utils.createListPtr(list);

    return createOp(ins, Compiler::StaticType::Number);
}

CompilerValue *LLVMCodeBuilder::addProcedureArgument(const std::string &name)
{
    if (!m_procedurePrototype)
        return addConstValue(Value());

    const auto &argNames = m_procedurePrototype->argumentNames();
    auto it = std::find(argNames.begin(), argNames.end(), name);

    if (it == argNames.end()) {
        std::cout << "warning: could not find argument '" << name << "' in custom block '" << m_procedurePrototype->procCode() << "'" << std::endl;
        return addConstValue(Value());
    }

    const auto index = it - argNames.begin();
    const Compiler::StaticType type = getProcedureArgType(m_procedurePrototype->argumentTypes()[index]);
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ProcedureArg, m_loopCondition);
    auto ret = std::make_shared<LLVMRegister>(type);
    ret->isRawValue = (type != Compiler::StaticType::Unknown);
    ins->functionReturnReg = ret.get();
    ins->procedureArgIndex = index;

    m_instructions.addInstruction(ins);
    return addReg(ret, ins);
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

CompilerValue *LLVMCodeBuilder::createRandom(CompilerValue *from, CompilerValue *to)
{
    return createOp(LLVMInstruction::Type::Random, Compiler::StaticType::Number, Compiler::StaticType::Unknown, { from, to });
}

CompilerValue *LLVMCodeBuilder::createRandomInt(CompilerValue *from, CompilerValue *to)
{
    return createOp(LLVMInstruction::Type::RandomInt, Compiler::StaticType::Number, Compiler::StaticType::Number, { from, to });
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

CompilerValue *LLVMCodeBuilder::createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive)
{
    return createOp(caseSensitive ? LLVMInstruction::Type::StrCmpEQCS : LLVMInstruction::Type::StrCmpEQCI, Compiler::StaticType::Bool, Compiler::StaticType::String, { string1, string2 });
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

CompilerValue *LLVMCodeBuilder::createStringConcat(CompilerValue *string1, CompilerValue *string2)
{
    return createOp(LLVMInstruction::Type::StringConcat, Compiler::StaticType::String, Compiler::StaticType::String, { string1, string2 });
}

CompilerValue *LLVMCodeBuilder::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType)
{
    LLVMRegister *ret = createOp(LLVMInstruction::Type::Select, valueType, { Compiler::StaticType::Bool, valueType, valueType }, { cond, trueValue, falseValue });

    if (valueType == Compiler::StaticType::Unknown)
        ret->isRawValue = false;

    return ret;
}

CompilerLocalVariable *LLVMCodeBuilder::createLocalVariable(Compiler::StaticType type)
{
    CompilerValue *ptr = createOp(LLVMInstruction::Type::CreateLocalVariable, type);
    auto var = std::make_shared<CompilerLocalVariable>(ptr);
    m_localVars.push_back(var);
    return var.get();
}

void LLVMCodeBuilder::createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value)
{
    createOp(LLVMInstruction::Type::WriteLocalVariable, Compiler::StaticType::Void, variable->type(), { variable->ptr(), value });
}

void LLVMCodeBuilder::createVariableWrite(Variable *variable, CompilerValue *value)
{
    LLVMInstruction ins(LLVMInstruction::Type::WriteVariable, m_loopCondition);
    ins.workVariable = variable;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { value });
    m_utils.createVariablePtr(variable);
}

void LLVMCodeBuilder::createListClear(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::ClearList, m_loopCondition);
    ins.workList = list;
    createOp(ins, Compiler::StaticType::Void);
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    LLVMInstruction ins(LLVMInstruction::Type::RemoveListItem, m_loopCondition);
    ins.workList = list;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Number, { index });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::AppendToList, m_loopCondition);
    ins.workList = list;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { item });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::InsertToList, m_loopCondition);
    ins.workList = list;
    createOp(ins, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Unknown }, { index, item });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListReplace, m_loopCondition);
    ins.workList = list;
    createOp(ins, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Unknown }, { index, item });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::beginIfStatement(CompilerValue *cond)
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<LLVMRegister *>(cond) });
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::beginElseBranch()
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::endIf()
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::beginRepeatLoop(CompilerValue *count)
{
    assert(!m_loopCondition);

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Number, dynamic_cast<LLVMRegister *>(count) });
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::beginWhileLoop(CompilerValue *cond)
{
    assert(m_loopCondition);
    m_loopCondition = false;

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<LLVMRegister *>(cond) });
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::beginRepeatUntilLoop(CompilerValue *cond)
{
    assert(m_loopCondition);
    m_loopCondition = false;

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, m_loopCondition);
    ins->args.push_back({ Compiler::StaticType::Bool, dynamic_cast<LLVMRegister *>(cond) });
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::beginLoopCondition()
{
    assert(!m_loopCondition);

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginLoopCondition, m_loopCondition);
    m_instructions.addInstruction(ins);
    m_loopCondition = true;
}

void LLVMCodeBuilder::endLoop()
{
    if (!m_warp) {
        auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Yield, m_loopCondition);
        m_instructions.addInstruction(ins);
    }

    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::yield()
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Yield, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::createStop()
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Stop, m_loopCondition);
    m_instructions.addInstruction(ins);
}

void LLVMCodeBuilder::createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args)
{
    assert(prototype);
    assert(prototype->argumentTypes().size() == args.size());
    const auto &procedureArgs = prototype->argumentTypes();
    Compiler::ArgTypes types;

    for (BlockPrototype::ArgType type : procedureArgs)
        types.push_back(getProcedureArgType(type));

    LLVMInstruction ins(LLVMInstruction::Type::CallProcedure, m_loopCondition);
    ins.procedurePrototype = prototype;
    createOp(ins, Compiler::StaticType::Void, types, args);
}

void LLVMCodeBuilder::initTypes()
{
    m_valueDataType = m_ctx->valueDataType();
    m_stringPtrType = m_ctx->stringPtrType();
}

std::string LLVMCodeBuilder::getMainFunctionName(BlockPrototype *procedurePrototype)
{
    std::string name;

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            name = "script";
            break;

        case Compiler::CodeType::Reporter:
            name = "reporter";
            break;

        case Compiler::CodeType::HatPredicate:
            name = "predicate";
            break;
    }

    return procedurePrototype ? "proc." + procedurePrototype->procCode() : name;
}

llvm::FunctionType *LLVMCodeBuilder::getMainFunctionType(BlockPrototype *procedurePrototype)
{
    // void *f(ExecutionContext *, Target *, ValueData **, List **, (warp arg), (procedure args...))
    // ValueData f(...) (reporters)
    // bool f(...) (hat predicates)
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0);
    std::vector<llvm::Type *> argTypes = { pointerType, pointerType, pointerType, pointerType };

    if (procedurePrototype) {
        argTypes.push_back(m_builder.getInt1Ty()); // warp arg (only in procedures)
        const auto &types = procedurePrototype->argumentTypes();

        for (BlockPrototype::ArgType type : types) {
            if (type == BlockPrototype::ArgType::Bool)
                argTypes.push_back(m_builder.getInt1Ty());
            else
                argTypes.push_back(m_valueDataType->getPointerTo());
        }
    }

    llvm::Type *retType = nullptr;

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            retType = pointerType;
            break;

        case Compiler::CodeType::Reporter:
            retType = m_valueDataType;
            break;

        case Compiler::CodeType::HatPredicate:
            retType = m_builder.getInt1Ty();
            break;
    }

    return llvm::FunctionType::get(retType, argTypes, false);
}

llvm::Function *LLVMCodeBuilder::getOrCreateFunction(const std::string &name, llvm::FunctionType *type)
{
    llvm::Function *func = m_module->getFunction(name);

    if (func)
        return func;
    else
        return llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, m_module);
}

void LLVMCodeBuilder::verifyFunction(llvm::Function *func)
{
    if (llvm::verifyFunction(*func, &llvm::errs())) {
        llvm::errs() << "error: LLVM function verficiation failed!\n";
        llvm::errs() << "module name: " << m_module->getName() << "\n";
    }
}

LLVMRegister *LLVMCodeBuilder::addReg(std::shared_ptr<LLVMRegister> reg, std::shared_ptr<LLVMInstruction> ins)
{
    reg->instruction = ins;
    m_regs.push_back(reg);
    return reg.get();
}

Compiler::StaticType LLVMCodeBuilder::getProcedureArgType(BlockPrototype::ArgType type)
{
    return type == BlockPrototype::ArgType::Bool ? Compiler::StaticType::Bool : Compiler::StaticType::Unknown;
}

LLVMRegister *LLVMCodeBuilder::createOp(LLVMInstruction::Type type, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args)
{
    return createOp({ type, m_loopCondition }, retType, argType, args);
}

LLVMRegister *LLVMCodeBuilder::createOp(LLVMInstruction::Type type, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    return createOp({ type, m_loopCondition }, retType, argTypes, args);
}

LLVMRegister *LLVMCodeBuilder::createOp(const LLVMInstruction &ins, Compiler::StaticType retType, Compiler::StaticType argType, const Compiler::Args &args)
{
    std::vector<Compiler::StaticType> types;
    types.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++)
        types.push_back(argType);

    return createOp(ins, retType, types, args);
}

LLVMRegister *LLVMCodeBuilder::createOp(const LLVMInstruction &ins, Compiler::StaticType retType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args)
{
    auto createdIns = std::make_shared<LLVMInstruction>(ins);
    m_instructions.addInstruction(createdIns);

    for (size_t i = 0; i < args.size(); i++)
        createdIns->args.push_back({ argTypes[i], dynamic_cast<LLVMRegister *>(args[i]) });

    if (retType != Compiler::StaticType::Void) {
        auto ret = std::make_shared<LLVMRegister>(retType);
        ret->isRawValue = true;
        createdIns->functionReturnReg = ret.get();
        return addReg(ret, createdIns);
    }

    return nullptr;
}

void LLVMCodeBuilder::createSuspend(LLVMCoroutine *coro, llvm::Value *warpArg, llvm::Value *targetVariables)
{
    if (!m_warp) {
        llvm::BasicBlock *suspendBranch, *nextBranch;

        if (warpArg) {
            suspendBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
            nextBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
            m_builder.CreateCondBr(warpArg, nextBranch, suspendBranch);
            m_builder.SetInsertPoint(suspendBranch);
        }

        m_utils.syncVariables(targetVariables);
        coro->createSuspend();
        m_utils.reloadVariables(targetVariables);

        if (warpArg) {
            m_builder.CreateBr(nextBranch);
            m_builder.SetInsertPoint(nextBranch);
        }
    }
}
