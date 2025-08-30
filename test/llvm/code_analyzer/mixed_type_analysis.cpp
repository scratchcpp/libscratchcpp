#include <scratchcpp/project.h>
#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmcodeanalyzer.h>
#include <engine/internal/llvm/llvmcompilercontext.h>
#include <engine/internal/llvm/llvmbuildutils.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <llvm/IR/IRBuilder.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

class LLVMCodeAnalyzer_MixedTypeAnalysis : public testing::Test
{
    public:
        void SetUp() override
        {
            auto engine = m_project.engine();
            m_ctx = std::make_unique<LLVMCompilerContext>(engine.get(), &m_target);
            m_builder = std::make_unique<llvm::IRBuilder<>>(*m_ctx->llvmCtx());
            m_utils = std::make_unique<LLVMBuildUtils>(m_ctx.get(), *m_builder, Compiler::CodeType::Script);
            m_analyzer = std::make_unique<LLVMCodeAnalyzer>(*m_utils);
        }

        std::unique_ptr<LLVMCodeAnalyzer> m_analyzer;

    private:
        Project m_project;
        Target m_target;
        std::unique_ptr<LLVMCompilerContext> m_ctx;
        std::unique_ptr<llvm::IRBuilder<>> m_builder;
        std::unique_ptr<LLVMBuildUtils> m_utils;
};

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, EmptyScript)
{
    LLVMInstructionList list;

    m_analyzer->analyzeScript(list);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, NoOperations)
{
    LLVMInstructionList list;

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, false);
    list.addInstruction(funcCall);

    m_analyzer->analyzeScript(list);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, VariableToList_SimpleTransfer)
{
    LLVMInstructionList list;
    Variable sourceVar("sourceVar", "");
    List targetList("targetList", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    // Write to variable
    auto writeVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister varValue(Compiler::StaticType::String, "hello");
    writeVar->targetVariable = &sourceVar;
    writeVar->args.push_back({ Compiler::StaticType::Unknown, &varValue });
    list.addInstruction(writeVar);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar->targetVariable = &sourceVar;
    list.addInstruction(readVar);

    LLVMRegister readVarReg(Compiler::StaticType::Unknown);
    readVarReg.isRawValue = false;
    readVarReg.instruction = readVar;
    readVar->functionReturnReg = &readVarReg;

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &readVarReg });
    list.addInstruction(appendList);

    m_analyzer->analyzeScript(list);

    // Variable write should see Unknown (first write)
    ASSERT_EQ(writeVar->targetType, Compiler::StaticType::Unknown);

    // Variable read should see String type
    ASSERT_EQ(readVar->targetType, Compiler::StaticType::String);

    // List append should see Void (empty list after clear)
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);

    // Return register should have String type
    ASSERT_EQ(readVarReg.type(), Compiler::StaticType::String);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, ListToVariable_SimpleTransfer)
{
    LLVMInstructionList list;
    List sourceList("sourceList", "");
    Variable targetVar("targetVar", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &sourceList;
    list.addInstruction(clearList);

    // Write to list
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister listValue(Compiler::StaticType::Bool, true);
    appendList->targetList = &sourceList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &listValue });
    list.addInstruction(appendList);

    // Read from list and write to variable
    auto getListItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getListItem->targetList = &sourceList;
    getListItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getListItem);

    LLVMRegister readListReg(Compiler::StaticType::Unknown);
    readListReg.isRawValue = false;
    readListReg.instruction = getListItem;
    getListItem->functionReturnReg = &readListReg;

    auto writeVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    writeVar->targetVariable = &targetVar;
    writeVar->args.push_back({ Compiler::StaticType::Unknown, &readListReg });
    list.addInstruction(writeVar);

    m_analyzer->analyzeScript(list);

    // List append should see Void (empty list after clear)
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);

    // List read should see Bool type
    ASSERT_EQ(getListItem->targetType, Compiler::StaticType::Bool);

    // Variable write should see Unknown (first write)
    ASSERT_EQ(writeVar->targetType, Compiler::StaticType::Unknown);

    // Return register should have Bool | String type (get list item can always return string)
    ASSERT_EQ(readListReg.type(), Compiler::StaticType::Bool | Compiler::StaticType::String);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, CircularVarListDependency)
{
    LLVMInstructionList list;
    Variable var("var", "");
    List targetList("list", "");

    // Initialize variable with number
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    writeVar1->targetVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(writeVar1);

    // Clear list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister readVarReg(Compiler::StaticType::Unknown);
    readVarReg.isRawValue = false;
    readVarReg.instruction = readVar;
    readVar->functionReturnReg = &readVarReg;

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &readVarReg });
    list.addInstruction(appendList);

    // Read from list and write back to variable (circular dependency)
    auto getListItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getListItem->targetList = &targetList;
    getListItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getListItem);

    LLVMRegister readListReg(Compiler::StaticType::Unknown);
    readListReg.isRawValue = false;
    readListReg.instruction = getListItem;
    getListItem->functionReturnReg = &readListReg;

    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    writeVar2->targetVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &readListReg });
    list.addInstruction(writeVar2);

    m_analyzer->analyzeScript(list);

    // Should handle circular dependency gracefully
    ASSERT_EQ(writeVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(readVar->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(getListItem->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(writeVar2->targetType, Compiler::StaticType::Number);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, LoopWithVarListInteraction_TypeConflict)
{
    LLVMInstructionList list;
    Variable var("var", "");
    List targetList("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    // First iteration: write string to variable
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "text");
    writeVar1->targetVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(writeVar1);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister readVarReg(Compiler::StaticType::Unknown);
    readVarReg.isRawValue = false;
    readVarReg.instruction = readVar;
    readVar->functionReturnReg = &readVarReg;

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &readVarReg });
    list.addInstruction(appendList);

    // Second iteration: write number to variable (creates type conflict)
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    writeVar2->targetVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(writeVar2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    m_analyzer->analyzeScript(list);

    // Should show type accumulation through loop iterations
    ASSERT_EQ(writeVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(readVar->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
    ASSERT_EQ(writeVar2->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, ConditionalVarListTransfer_TypeConflict)
{
    LLVMInstructionList list;
    Variable var("var", "");
    List targetList("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // If branch: write number to variable, transfer to list
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 123);
    writeVar1->targetVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(writeVar1);

    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar1->targetVariable = &var;
    list.addInstruction(readVar1);

    LLVMRegister readVar1Reg(Compiler::StaticType::Unknown);
    readVar1Reg.isRawValue = false;
    readVar1Reg.instruction = readVar1;
    readVar1->functionReturnReg = &readVar1Reg;

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &readVar1Reg });
    list.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Else branch: write string to variable, transfer to list
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    writeVar2->targetVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(writeVar2);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var;
    list.addInstruction(readVar2);

    LLVMRegister readVar2Reg(Compiler::StaticType::Unknown);
    readVar2Reg.isRawValue = false;
    readVar2Reg.instruction = readVar2;
    readVar2->functionReturnReg = &readVar2Reg;

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &readVar2Reg });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    m_analyzer->analyzeScript(list);

    // Should show type conflicts from conditional branches
    ASSERT_EQ(writeVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(writeVar2->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Void);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, MultipleVarsToSingleList_TypePropagation)
{
    LLVMInstructionList list;
    Variable var1("var1", "");
    Variable var2("var2", "");
    List targetList("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    // Initialize first variable with boolean
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    writeVar1->targetVariable = &var1;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(writeVar1);

    // Initialize second variable with same type
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, false);
    writeVar2->targetVariable = &var2;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(writeVar2);

    // Read from first variable and write to list
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar1->targetVariable = &var1;
    list.addInstruction(readVar1);

    LLVMRegister readVar1Reg(Compiler::StaticType::Unknown);
    readVar1Reg.isRawValue = false;
    readVar1Reg.instruction = readVar1;
    readVar1->functionReturnReg = &readVar1Reg;

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &readVar1Reg });
    list.addInstruction(appendList1);

    // Read from second variable and write to list
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var2;
    list.addInstruction(readVar2);

    LLVMRegister readVar2Reg(Compiler::StaticType::Unknown);
    readVar2Reg.isRawValue = false;
    readVar2Reg.instruction = readVar2;
    readVar2->functionReturnReg = &readVar2Reg;

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &readVar2Reg });
    list.addInstruction(appendList2);

    m_analyzer->analyzeScript(list);

    // List should have Bool type from both variables
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Bool);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, SingleListToMultipleVars_TypePropagation)
{
    LLVMInstructionList list;
    List sourceList("sourceList", "");
    Variable var1("var1", "");
    Variable var2("var2", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &sourceList;
    list.addInstruction(clearList);

    // Initialize list with string values
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister listValue1(Compiler::StaticType::String, "first");
    appendList1->targetList = &sourceList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &listValue1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister listValue2(Compiler::StaticType::String, "second");
    appendList2->targetList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &listValue2 });
    list.addInstruction(appendList2);

    // Read first item and write to first variable
    auto readList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 0);
    readList1->targetList = &sourceList;
    readList1->args.push_back({ Compiler::StaticType::Number, &index1 });
    list.addInstruction(readList1);

    LLVMRegister readList1Reg(Compiler::StaticType::Unknown);
    readList1Reg.isRawValue = false;
    readList1Reg.instruction = readList1;
    readList1->functionReturnReg = &readList1Reg;

    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    writeVar1->targetVariable = &var1;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &readList1Reg });
    list.addInstruction(writeVar1);

    // Read second item and write to second variable
    auto readList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    readList2->targetList = &sourceList;
    readList2->args.push_back({ Compiler::StaticType::Number, &index2 });
    list.addInstruction(readList2);

    LLVMRegister readList2Reg(Compiler::StaticType::Unknown);
    readList2Reg.isRawValue = false;
    readList2Reg.instruction = readList2;
    readList2->functionReturnReg = &readList2Reg;

    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    writeVar2->targetVariable = &var2;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &readList2Reg });
    list.addInstruction(writeVar2);

    m_analyzer->analyzeScript(list);

    // Both variables should have String type from the list
    ASSERT_EQ(writeVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(writeVar2->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(readList1Reg.type(), Compiler::StaticType::String);
    ASSERT_EQ(readList2Reg.type(), Compiler::StaticType::String);
}

TEST_F(LLVMCodeAnalyzer_MixedTypeAnalysis, ComplexChain_VarToListToVar_TypePropagation)
{
    LLVMInstructionList list;
    Variable sourceVar("sourceVar", "");
    List intermediateList("intermediateList", "");
    Variable targetVar("targetVar", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &intermediateList;
    list.addInstruction(clearList);

    // Initialize source variable
    auto writeSourceVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister sourceValue(Compiler::StaticType::Number, 3.14159);
    writeSourceVar->targetVariable = &sourceVar;
    writeSourceVar->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(writeSourceVar);

    // Read from source variable and write to intermediate list
    auto readSourceVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readSourceVar->targetVariable = &sourceVar;
    list.addInstruction(readSourceVar);

    LLVMRegister readSourceVarReg(Compiler::StaticType::Unknown);
    readSourceVarReg.isRawValue = false;
    readSourceVarReg.instruction = readSourceVar;
    readSourceVar->functionReturnReg = &readSourceVarReg;

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList->targetList = &intermediateList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &readSourceVarReg });
    list.addInstruction(appendList);

    // Read from intermediate list and write to target variable
    auto readList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    readList->targetList = &intermediateList;
    readList->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(readList);

    LLVMRegister readListReg(Compiler::StaticType::Unknown);
    readListReg.isRawValue = false;
    readListReg.instruction = readList;
    readList->functionReturnReg = &readListReg;

    auto writeTargetVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    writeTargetVar->targetVariable = &targetVar;
    writeTargetVar->args.push_back({ Compiler::StaticType::Unknown, &readListReg });
    list.addInstruction(writeTargetVar);

    m_analyzer->analyzeScript(list);

    // Type should propagate through the chain: sourceVar -> intermediateList -> targetVar
    ASSERT_EQ(writeSourceVar->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(readSourceVar->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(readList->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(writeTargetVar->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(readListReg.type(), Compiler::StaticType::Number | Compiler::StaticType::String); // get list item can always return string
}
