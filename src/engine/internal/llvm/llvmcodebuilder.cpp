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
    m_utils(ctx, m_builder, codeType),
    m_procedurePrototype(procedurePrototype),
    m_defaultWarp(procedurePrototype ? procedurePrototype->warp() : false),
    m_warp(m_defaultWarp),
    m_codeType(codeType),
    m_instructionBuilder(m_utils)
{
    initTypes();
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::build()
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

    if (m_warp) {
#ifdef ENABLE_CODE_ANALYZER
        // Analyze the script (type analysis, optimizations, etc.)
        // NOTE: Do this only for warp scripts
        m_codeAnalyzer.analyzeScript(m_instructions);
#endif
    }

    // Set fast math flags
    llvm::FastMathFlags fmf;
    fmf.setFast(true);
    fmf.setNoInfs(false);
    fmf.setNoNaNs(false);
    fmf.setNoSignedZeros(false);
    m_builder.setFastMathFlags(fmf);

    // Create function
    std::string funcName = m_utils.scriptFunctionName(m_procedurePrototype);
    llvm::FunctionType *funcType = m_utils.scriptFunctionType(m_procedurePrototype);

    if (m_procedurePrototype)
        m_function = getOrCreateFunction(funcName, funcType);
    else
        m_function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcName, m_module);

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_llvmCtx, "entry", m_function);
    m_builder.SetInsertPoint(entry);

    m_utils.init(m_function, m_procedurePrototype, m_warp);

    // Build recorded instructions
    LLVMInstruction *ins = m_instructions.first();

    while (ins)
        ins = m_instructionBuilder.process(ins);

    m_utils.end(m_instructions.empty() ? nullptr : m_instructions.last(), m_lastConstValue);
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

        if (m_utils.isSingleType(returnType))
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
    ins->targetVariable = variable;
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
    ins.targetList = list;
    m_utils.createListPtr(list);

    return createOp(ins, Compiler::StaticType::String);
}

CompilerValue *LLVMCodeBuilder::addListItem(List *list, CompilerValue *index)
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, m_loopCondition);
    ins->targetList = list;
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
    ins.targetList = list;
    m_utils.createListPtr(list);

    auto ret = createOp(ins, Compiler::StaticType::Number, Compiler::StaticType::Unknown, { item });
    return ret;
}

CompilerValue *LLVMCodeBuilder::addListContains(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListContainsItem, m_loopCondition);
    ins.targetList = list;
    m_utils.createListPtr(list);

    auto ret = createOp(ins, Compiler::StaticType::Bool, Compiler::StaticType::Unknown, { item });
    return ret;
}

CompilerValue *LLVMCodeBuilder::addListSize(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::GetListSize, m_loopCondition);
    ins.targetList = list;
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
    ret->isRawValue = m_utils.isSingleType(valueType);
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
    ins.targetVariable = variable;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { value });
    m_utils.createVariablePtr(variable);
}

void LLVMCodeBuilder::createListClear(List *list)
{
    LLVMInstruction ins(LLVMInstruction::Type::ClearList, m_loopCondition);
    ins.targetList = list;
    createOp(ins, Compiler::StaticType::Void);
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListRemove(List *list, CompilerValue *index)
{
    LLVMInstruction ins(LLVMInstruction::Type::RemoveListItem, m_loopCondition);
    ins.targetList = list;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Number, { index });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListAppend(List *list, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::AppendToList, m_loopCondition);
    ins.targetList = list;
    createOp(ins, Compiler::StaticType::Void, Compiler::StaticType::Unknown, { item });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::InsertToList, m_loopCondition);
    ins.targetList = list;
    createOp(ins, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Unknown }, { index, item });
    m_utils.createListPtr(list);
}

void LLVMCodeBuilder::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    LLVMInstruction ins(LLVMInstruction::Type::ListReplace, m_loopCondition);
    ins.targetList = list;
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

void LLVMCodeBuilder::createStopWithoutSync()
{
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::StopWithoutSync, m_loopCondition);
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
