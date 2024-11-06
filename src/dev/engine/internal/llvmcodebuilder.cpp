// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/PassBuilder.h>

#include "llvmcodebuilder.h"
#include "llvmexecutablecode.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType>
    TYPE_MAP = { { ValueType::Number, Compiler::StaticType::Number }, { ValueType::Bool, Compiler::StaticType::Bool }, { ValueType::String, Compiler::StaticType::String } };

LLVMCodeBuilder::LLVMCodeBuilder(const std::string &id, bool warp) :
    m_id(id),
    m_module(std::make_unique<llvm::Module>(id, m_ctx)),
    m_builder(m_ctx),
    m_defaultWarp(warp),
    m_warp(warp)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    m_constValues.push_back({});
    m_regs.push_back({});
    initTypes();
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::finalize()
{
    // Do not create coroutine if there are no yield instructions
    if (!m_warp) {
        auto it = std::find_if(m_steps.begin(), m_steps.end(), [](const Step &step) { return step.type == Step::Type::Yield; });

        if (it == m_steps.end())
            m_warp = true;
    }

    // Create function
    // void *f(Target *)
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    llvm::FunctionType *funcType = llvm::FunctionType::get(pointerType, pointerType, false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f", m_module.get());

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
    m_builder.SetInsertPoint(entry);

    // Init coroutine
    Coroutine coro;

    if (!m_warp)
        coro = initCoroutine(func);

    std::vector<IfStatement> ifStatements;
    std::vector<Loop> loops;
    m_heap.clear();

    // Execute recorded steps
    for (const Step &step : m_steps) {
        switch (step.type) {
            case Step::Type::FunctionCall: {
                std::vector<llvm::Type *> types;
                std::vector<llvm::Value *> args;

                // Add target pointer arg
                assert(func->arg_size() == 1);
                types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0));
                args.push_back(func->getArg(0));

                // Args
                for (auto &arg : step.args) {
                    types.push_back(getType(arg.first));
                    args.push_back(castValue(arg.second, arg.first));
                }

                llvm::Value *ret = m_builder.CreateCall(resolveFunction(step.functionName, llvm::FunctionType::get(getType(step.functionReturnType), types, false)), args);

                if (step.functionReturnReg) {
                    step.functionReturnReg->value = ret;

                    if (step.functionReturnType == Compiler::StaticType::String)
                        m_heap.push_back(step.functionReturnReg->value);
                }

                break;
            }

            case Step::Type::Add: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFAdd(num1, num2);
                break;
            }

            case Step::Type::Sub: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFSub(num1, num2);
                break;
            }

            case Step::Type::Mul: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFMul(num1, num2);
                break;
            }

            case Step::Type::Div: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *num1 = removeNaN(castValue(arg1.second, arg1.first));
                llvm::Value *num2 = removeNaN(castValue(arg2.second, arg2.first));
                step.functionReturnReg->value = m_builder.CreateFDiv(num1, num2);
                break;
            }

            case Step::Type::CmpEQ: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::EQ);
                break;
            }

            case Step::Type::CmpGT: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::GT);
                break;
            }

            case Step::Type::CmpLT: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0].second;
                const auto &arg2 = step.args[1].second;
                step.functionReturnReg->value = createComparison(arg1, arg2, Comparison::LT);
                break;
            }

            case Step::Type::And: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *bool1 = castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = castValue(arg2.second, arg2.first);
                step.functionReturnReg->value = m_builder.CreateAnd(bool1, bool2);
                break;
            }

            case Step::Type::Or: {
                assert(step.args.size() == 2);
                const auto &arg1 = step.args[0];
                const auto &arg2 = step.args[1];
                llvm::Value *bool1 = castValue(arg1.second, arg1.first);
                llvm::Value *bool2 = castValue(arg2.second, arg2.first);
                step.functionReturnReg->value = m_builder.CreateOr(bool1, bool2);
                break;
            }

            case Step::Type::Not: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Value *value = castValue(arg.second, arg.first);
                step.functionReturnReg->value = m_builder.CreateNot(value);
                break;
            }

            case Step::Type::Mod: {
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

            case Step::Type::Round: {
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

            case Step::Type::Abs: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *absFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::fabs, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(absFunc, num);
                break;
            }

            case Step::Type::Floor: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *floorFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::floor, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(floorFunc, num);
                break;
            }

            case Step::Type::Ceil: {
                assert(step.args.size() == 1);
                const auto &arg = step.args[0];
                llvm::Function *ceilFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::ceil, m_builder.getDoubleTy());
                llvm::Value *num = removeNaN(castValue(arg.second, arg.first));
                step.functionReturnReg->value = m_builder.CreateCall(ceilFunc, num);
                break;
            }

            case Step::Type::Sqrt: {
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

            case Step::Type::Sin: {
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

            case Step::Type::Cos: {
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

            case Step::Type::Tan: {
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

            case Step::Type::Yield:
                if (!m_warp) {
                    freeHeap();
                    m_builder.CreateStore(m_builder.getInt1(true), coro.didSuspend);
                    llvm::BasicBlock *resumeBranch = llvm::BasicBlock::Create(m_ctx, "", func);
                    llvm::Value *noneToken = llvm::ConstantTokenNone::get(m_ctx);
                    llvm::Value *suspendResult = m_builder.CreateCall(llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_suspend), { noneToken, m_builder.getInt1(false) });
                    llvm::SwitchInst *sw = m_builder.CreateSwitch(suspendResult, coro.suspend, 2);
                    sw->addCase(m_builder.getInt8(0), resumeBranch);
                    sw->addCase(m_builder.getInt8(1), coro.cleanup);
                    m_builder.SetInsertPoint(resumeBranch);
                }

                break;

            case Step::Type::BeginIf: {
                IfStatement statement;
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
                break;
            }

            case Step::Type::BeginElse: {
                assert(!ifStatements.empty());
                IfStatement &statement = ifStatements.back();

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

            case Step::Type::EndIf: {
                assert(!ifStatements.empty());
                IfStatement &statement = ifStatements.back();

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
                break;
            }

            case Step::Type::BeginRepeatLoop: {
                Loop loop;
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
                break;
            }

            case Step::Type::BeginWhileLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

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
                break;
            }

            case Step::Type::BeginRepeatUntilLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

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
                break;
            }

            case Step::Type::BeginLoopCondition: {
                Loop loop;
                loop.isRepeatLoop = false;
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", func);
                freeHeap();
                m_builder.CreateBr(loop.conditionBranch);
                m_builder.SetInsertPoint(loop.conditionBranch);
                loops.push_back(loop);
                break;
            }

            case Step::Type::EndLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

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
                break;
            }
        }
    }

    freeHeap();

    // Add final suspend point
    if (!m_warp) {
        llvm::BasicBlock *endBranch = llvm::BasicBlock::Create(m_ctx, "end", func);
        llvm::BasicBlock *finalSuspendBranch = llvm::BasicBlock::Create(m_ctx, "finalSuspend", func);
        m_builder.CreateCondBr(m_builder.CreateLoad(m_builder.getInt1Ty(), coro.didSuspend), finalSuspendBranch, endBranch);

        m_builder.SetInsertPoint(finalSuspendBranch);
        llvm::Value *suspendResult =
            m_builder.CreateCall(llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_suspend), { llvm::ConstantTokenNone::get(m_ctx), m_builder.getInt1(true) });
        llvm::SwitchInst *sw = m_builder.CreateSwitch(suspendResult, coro.suspend, 2);
        sw->addCase(m_builder.getInt8(0), endBranch); // unreachable
        sw->addCase(m_builder.getInt8(1), coro.cleanup);

        m_builder.SetInsertPoint(endBranch);
    }

    // End and verify the function
    if (!m_tmpRegs.empty()) {
        std::cout
            << "warning: " << m_tmpRegs.size() << " registers were leaked by script '" << m_module->getName().str() << "', function '" << func->getName().str()
            << "' (if you see this as a regular user, this is a bug and should be reported)" << std::endl;
    }

    if (m_warp)
        m_builder.CreateRet(llvm::ConstantPointerNull::get(pointerType));
    else
        m_builder.CreateBr(coro.freeMemRet);

    verifyFunction(func);

    // Create resume function
    // bool resume(void *)
    funcType = llvm::FunctionType::get(m_builder.getInt1Ty(), pointerType, false);
    func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "resume", m_module.get());

    entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
    m_builder.SetInsertPoint(entry);

    if (m_warp)
        m_builder.CreateRet(m_builder.getInt1(true));
    else {
        llvm::Value *coroHandle = func->getArg(0);
        m_builder.CreateCall(llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_resume), { coroHandle });
        llvm::Value *done = m_builder.CreateCall(llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_done), { coroHandle });
        m_builder.CreateRet(done);
    }

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

void LLVMCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const std::vector<Compiler::StaticType> &argTypes)
{
    Step step(Step::Type::FunctionCall);
    step.functionName = functionName;

    assert(m_tmpRegs.size() >= argTypes.size());
    size_t j = 0;

    for (size_t i = m_tmpRegs.size() - argTypes.size(); i < m_tmpRegs.size(); i++)
        step.args.push_back({ argTypes[j++], m_tmpRegs[i] });

    m_tmpRegs.erase(m_tmpRegs.end() - argTypes.size(), m_tmpRegs.end());

    step.functionReturnType = returnType;

    if (returnType != Compiler::StaticType::Void) {
        auto reg = std::make_shared<Register>(returnType);
        reg->isRawValue = true;
        step.functionReturnReg = reg;
        m_regs[m_currentFunction].push_back(reg);
        m_tmpRegs.push_back(reg);
    }

    m_steps.push_back(step);
}

void LLVMCodeBuilder::addConstValue(const Value &value)
{
    auto reg = std::make_shared<Register>(TYPE_MAP[value.type()]);
    reg->isConstValue = true;
    reg->constValue = value;
    m_regs[m_currentFunction].push_back(reg);
    m_tmpRegs.push_back(reg);
}

void LLVMCodeBuilder::addVariableValue(Variable *variable)
{
}

void LLVMCodeBuilder::addListContents(List *list)
{
}

void LLVMCodeBuilder::createAdd()
{
    createOp(Step::Type::Add, Compiler::StaticType::Number, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createSub()
{
    createOp(Step::Type::Sub, Compiler::StaticType::Number, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createMul()
{
    createOp(Step::Type::Mul, Compiler::StaticType::Number, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createDiv()
{
    createOp(Step::Type::Div, Compiler::StaticType::Number, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createCmpEQ()
{
    createOp(Step::Type::CmpEQ, Compiler::StaticType::Bool, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createCmpGT()
{
    createOp(Step::Type::CmpGT, Compiler::StaticType::Bool, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createCmpLT()
{
    createOp(Step::Type::CmpLT, Compiler::StaticType::Bool, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createAnd()
{
    createOp(Step::Type::And, Compiler::StaticType::Bool, Compiler::StaticType::Bool, 2);
}

void LLVMCodeBuilder::createOr()
{
    createOp(Step::Type::Or, Compiler::StaticType::Bool, Compiler::StaticType::Bool, 2);
}

void LLVMCodeBuilder::createNot()
{
    createOp(Step::Type::Not, Compiler::StaticType::Bool, Compiler::StaticType::Bool, 1);
}

void LLVMCodeBuilder::createMod()
{
    createOp(Step::Type::Mod, Compiler::StaticType::Number, Compiler::StaticType::Number, 2);
}

void LLVMCodeBuilder::createRound()
{
    createOp(Step::Type::Round, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createAbs()
{
    createOp(Step::Type::Abs, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createFloor()
{
    createOp(Step::Type::Floor, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createCeil()
{
    createOp(Step::Type::Ceil, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createSqrt()
{
    createOp(Step::Type::Sqrt, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createSin()
{
    createOp(Step::Type::Sin, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createCos()
{
    createOp(Step::Type::Cos, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::createTan()
{
    createOp(Step::Type::Tan, Compiler::StaticType::Number, Compiler::StaticType::Number, 1);
}

void LLVMCodeBuilder::beginIfStatement()
{
    Step step(Step::Type::BeginIf);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginElseBranch()
{
    m_steps.push_back(Step(Step::Type::BeginElse));
}

void LLVMCodeBuilder::endIf()
{
    m_steps.push_back(Step(Step::Type::EndIf));
}

void LLVMCodeBuilder::beginRepeatLoop()
{
    Step step(Step::Type::BeginRepeatLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Number, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginWhileLoop()
{
    Step step(Step::Type::BeginWhileLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginRepeatUntilLoop()
{
    Step step(Step::Type::BeginRepeatUntilLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginLoopCondition()
{
    m_steps.push_back(Step(Step::Type::BeginLoopCondition));
}

void LLVMCodeBuilder::endLoop()
{
    if (!m_warp)
        m_steps.push_back(Step(Step::Type::Yield));

    m_steps.push_back(Step(Step::Type::EndLoop));
}

void LLVMCodeBuilder::yield()
{
    m_steps.push_back({ Step::Type::Yield });
    m_currentFunction++;

    assert(m_currentFunction == m_constValues.size());
    m_constValues.push_back({});

    assert(m_currentFunction == m_regs.size());
    m_regs.push_back({});
}

void LLVMCodeBuilder::initTypes()
{
    // Create the ValueData struct
    llvm::Type *unionType = m_builder.getInt64Ty(); // 64 bits is the largest size

    llvm::Type *valueType = llvm::Type::getInt32Ty(m_ctx); // Assuming ValueType is a 32-bit enum
    llvm::Type *sizeType = llvm::Type::getInt64Ty(m_ctx);  // size_t

    m_valueDataType = llvm::StructType::create(m_ctx, "ValueData");
    m_valueDataType->setBody({ unionType, valueType, sizeType });
}

LLVMCodeBuilder::Coroutine LLVMCodeBuilder::initCoroutine(llvm::Function *func)
{
    // Set presplitcoroutine attribute
    func->setPresplitCoroutine();

    // Coroutine intrinsics
    llvm::Function *coroId = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_id);
    llvm::Function *coroSize = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_size, m_builder.getInt64Ty());
    llvm::Function *coroBegin = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_begin);
    llvm::Function *coroEnd = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_end);
    llvm::Function *coroFree = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::coro_free);

    // Init coroutine
    Coroutine coro;
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    llvm::Constant *nullPointer = llvm::ConstantPointerNull::get(pointerType);
    llvm::Value *coroIdRet = m_builder.CreateCall(coroId, { m_builder.getInt32(8), nullPointer, nullPointer, nullPointer });

    // Allocate memory
    llvm::Value *coroSizeRet = m_builder.CreateCall(coroSize, std::nullopt, "size");
    llvm::Function *mallocFunc = llvm::Function::Create(llvm::FunctionType::get(pointerType, { m_builder.getInt64Ty() }, false), llvm::Function::ExternalLinkage, "malloc", m_module.get());
    llvm::Value *alloc = m_builder.CreateCall(mallocFunc, coroSizeRet, "mem");

    // Begin
    coro.handle = m_builder.CreateCall(coroBegin, { coroIdRet, alloc });
    coro.didSuspend = m_builder.CreateAlloca(m_builder.getInt1Ty(), nullptr, "didSuspend");
    m_builder.CreateStore(m_builder.getInt1(false), coro.didSuspend);
    llvm::BasicBlock *entry = m_builder.GetInsertBlock();

    // Create suspend branch
    coro.suspend = llvm::BasicBlock::Create(m_ctx, "suspend", func);
    m_builder.SetInsertPoint(coro.suspend);
    m_builder.CreateCall(coroEnd, { coro.handle, m_builder.getInt1(false), llvm::ConstantTokenNone::get(m_ctx) });
    m_builder.CreateRet(coro.handle);

    // Create free branches
    coro.freeMemRet = llvm::BasicBlock::Create(m_ctx, "freeMemRet", func);
    m_builder.SetInsertPoint(coro.freeMemRet);
    m_builder.CreateFree(alloc);
    m_builder.CreateRet(llvm::ConstantPointerNull::get(pointerType));

    llvm::BasicBlock *freeBranch = llvm::BasicBlock::Create(m_ctx, "free", func);
    m_builder.SetInsertPoint(freeBranch);
    m_builder.CreateFree(alloc);
    m_builder.CreateBr(coro.suspend);

    // Create cleanup branch
    coro.cleanup = llvm::BasicBlock::Create(m_ctx, "cleanup", func);
    m_builder.SetInsertPoint(coro.cleanup);
    llvm::Value *mem = m_builder.CreateCall(coroFree, { coroIdRet, coro.handle });
    llvm::Value *needFree = m_builder.CreateIsNotNull(mem);
    m_builder.CreateCondBr(needFree, freeBranch, coro.suspend);

    m_builder.SetInsertPoint(entry);
    return coro;
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

void LLVMCodeBuilder::freeHeap()
{
    // Free dynamically allocated memory
    for (llvm::Value *ptr : m_heap)
        m_builder.CreateFree(ptr);

    m_heap.clear();
}

llvm::Value *LLVMCodeBuilder::castValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType)
{
    if (reg->isConstValue)
        return castConstValue(reg->constValue, targetType);

    if (reg->isRawValue)
        return castRawValue(reg, targetType);

    assert(reg->type != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type) {
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

                case Compiler::StaticType::String: {
                    // Convert string to double
                    return m_builder.CreateCall(resolve_value_toDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type) {
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
                    // Convert string to bool
                    return m_builder.CreateCall(resolve_value_toBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type) {
                case Compiler::StaticType::Number:
                case Compiler::StaticType::Bool: {
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

llvm::Value *LLVMCodeBuilder::castRawValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType)
{
    if (reg->type == targetType)
        return reg->value;

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type) {
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
            switch (reg->type) {
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
            switch (reg->type) {
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

void LLVMCodeBuilder::createOp(Step::Type type, Compiler::StaticType retType, Compiler::StaticType argType, size_t argCount)
{
    Step step(type);

    assert(m_tmpRegs.size() >= argCount);
    size_t j = 0;

    for (size_t i = m_tmpRegs.size() - argCount; i < m_tmpRegs.size(); i++)
        step.args.push_back({ argType, m_tmpRegs[i] });

    m_tmpRegs.erase(m_tmpRegs.end() - argCount, m_tmpRegs.end());

    auto ret = std::make_shared<Register>(retType);
    ret->isRawValue = true;
    step.functionReturnReg = ret;
    m_regs[m_currentFunction].push_back(ret);
    m_tmpRegs.push_back(ret);

    m_steps.push_back(step);
}

llvm::Value *LLVMCodeBuilder::createValue(std::shared_ptr<Register> reg)
{
    if (reg->isConstValue) {
        // Create a constant ValueData instance and store it
        llvm::Constant *value = castConstValue(reg->constValue, TYPE_MAP[reg->constValue.type()]);
        llvm::Value *ret = m_builder.CreateAlloca(m_valueDataType);

        if (reg->constValue.type() == ValueType::String)
            value = llvm::ConstantExpr::getPtrToInt(value, m_valueDataType->getElementType(0));
        else
            value = llvm::ConstantExpr::getBitCast(value, m_valueDataType->getElementType(0));

        llvm::Constant *type = m_builder.getInt32(static_cast<uint32_t>(reg->constValue.type()));
        llvm::Constant *constValue = llvm::ConstantStruct::get(m_valueDataType, { value, type, m_builder.getInt64(0) });
        m_builder.CreateStore(constValue, ret);

        return ret;
    } else if (reg->isRawValue) {
        llvm::Value *value = castRawValue(reg, reg->type);
        llvm::Value *ret = m_builder.CreateAlloca(m_valueDataType);

        // Store value
        llvm::Value *valueField = m_builder.CreateStructGEP(m_valueDataType, ret, 0);
        m_builder.CreateStore(value, valueField);

        auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [&reg](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == reg->type; });

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

llvm::Value *LLVMCodeBuilder::createComparison(std::shared_ptr<Register> arg1, std::shared_ptr<Register> arg2, Comparison type)
{
    auto type1 = arg1->type;
    auto type2 = arg2->type;

    if (arg1->isConstValue && arg2->isConstValue) {
        // If both operands are constant, perform the comparison at compile time
        bool result = false;

        switch (type) {
            case Comparison::EQ:
                result = arg1->constValue == arg2->constValue;
                break;

            case Comparison::GT:
                result = arg1->constValue > arg2->constValue;
                break;

            case Comparison::LT:
                result = arg1->constValue < arg2->constValue;
                break;

            default:
                assert(false);
                return nullptr;
        }

        return m_builder.getInt1(result);
    } else {
        // Optimize comparison of constant with number/bool
        if (arg1->isConstValue && arg1->constValue.isValidNumber() && (type2 == Compiler::StaticType::Number || type2 == Compiler::StaticType::Bool))
            type1 = Compiler::StaticType::Number;

        if (arg2->isConstValue && arg2->constValue.isValidNumber() && (type1 == Compiler::StaticType::Number || type1 == Compiler::StaticType::Bool))
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
    return resolveFunction("value_assign_double", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_cstring()
{
    return resolveFunction("value_assign_cstring", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0) }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_special()
{
    return resolveFunction("value_assign_special", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt32Ty() }, false));
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

llvm::FunctionCallee LLVMCodeBuilder::resolve_strcasecmp()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);
    return resolveFunction("strcasecmp", llvm::FunctionType::get(m_builder.getInt32Ty(), { pointerType, pointerType }, false));
}
