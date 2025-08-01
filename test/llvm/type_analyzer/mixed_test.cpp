#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_MixedTest, VariableToList_SimpleTransfer_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable sourceVar("sourceVar", "");
    List targetList("targetList", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &targetList;
    instructionList.addInstruction(clearList);

    // Write to variable
    auto writeVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister varValue(Compiler::StaticType::String, "hello");
    writeVar->workVariable = &sourceVar;
    writeVar->args.push_back({ Compiler::StaticType::Unknown, &varValue });
    instructionList.addInstruction(writeVar);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVarReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar->functionReturnReg = readVarReg.get();
    readVarReg->instruction = readVar;
    readVar->workVariable = &sourceVar;
    instructionList.addInstruction(readVar);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList->workList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, readVarReg.get() });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // List type should match variable type
    ASSERT_EQ(analyzer.listType(&targetList, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_MixedTest, ListToVariable_SimpleTransfer_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List sourceList("sourceList", "");
    Variable targetVar("targetVar", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &sourceList;
    instructionList.addInstruction(clearList);

    // Write to list
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister listValue(Compiler::StaticType::Bool, true);
    appendList->workList = &sourceList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &listValue });
    instructionList.addInstruction(appendList);

    // Read from list and write to variable
    auto readList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, nullptr, false);
    auto readListReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readList->functionReturnReg = readListReg.get();
    readListReg->instruction = readList;
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    readList->workList = &sourceList;
    readList->args.push_back({ Compiler::StaticType::Number, &index });
    instructionList.addInstruction(readList);

    auto writeVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    writeVar->workVariable = &targetVar;
    writeVar->args.push_back({ Compiler::StaticType::Unknown, readListReg.get() });
    instructionList.addInstruction(writeVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Variable type should match list type
    ASSERT_EQ(analyzer.variableType(&targetVar, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_MixedTest, CircularVarListDependency_TypeSafety)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable var("var", "");
    List list("list", "");

    // Initialize variable with number
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    writeVar1->workVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(writeVar1);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVarReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar->functionReturnReg = readVarReg.get();
    readVarReg->instruction = readVar;
    readVar->workVariable = &var;
    instructionList.addInstruction(readVar);

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, readVarReg.get() });
    instructionList.addInstruction(appendList);

    // Read from list and write back to variable (circular dependency)
    auto readList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, nullptr, false);
    auto readListReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readList->functionReturnReg = readListReg.get();
    readListReg->instruction = readList;
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    readList->workList = &list;
    readList->args.push_back({ Compiler::StaticType::Number, &index });
    instructionList.addInstruction(readList);

    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    writeVar2->workVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, readListReg.get() });
    instructionList.addInstruction(writeVar2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should handle circular dependency gracefully
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_MixedTest, LoopWithVarListInteraction_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable var("var", "");
    List list("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // First iteration: write string to variable
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "text");
    writeVar1->workVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(writeVar1);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(loopStart);

    // Read from variable and write to list
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVarReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar->functionReturnReg = readVarReg.get();
    readVarReg->instruction = readVar;
    readVar->workVariable = &var;
    instructionList.addInstruction(readVar);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, readVarReg.get() });
    instructionList.addInstruction(appendList);

    // Second iteration: write number to variable (creates type conflict)
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    writeVar2->workVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(writeVar2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(loopEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown due to type conflict
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_MixedTest, ConditionalVarListTransfer_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable var("var", "");
    List list("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // If branch: write number to variable, transfer to list
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 123);
    writeVar1->workVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(writeVar1);

    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVar1Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar1->functionReturnReg = readVar1Reg.get();
    readVar1Reg->instruction = readVar1;
    readVar1->workVariable = &var;
    instructionList.addInstruction(readVar1);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, readVar1Reg.get() });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Else branch: write string to variable, transfer to list
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    writeVar2->workVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(writeVar2);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVar2Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar2->functionReturnReg = readVar2Reg.get();
    readVar2Reg->instruction = readVar2;
    readVar2->workVariable = &var;
    instructionList.addInstruction(readVar2);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, readVar2Reg.get() });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown due to conditional type conflicts
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_MixedTest, MultipleVarsToSingleList_TypePropagation)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable var1("var1", "");
    Variable var2("var2", "");
    List list("list", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Initialize first variable with boolean
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    writeVar1->workVariable = &var1;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(writeVar1);

    // Initialize second variable with same type
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, false);
    writeVar2->workVariable = &var2;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(writeVar2);

    // Read from first variable and write to list
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVar1Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar1->functionReturnReg = readVar1Reg.get();
    readVar1Reg->instruction = readVar1;
    readVar1->workVariable = &var1;
    instructionList.addInstruction(readVar1);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, readVar1Reg.get() });
    instructionList.addInstruction(appendList1);

    // Read from second variable and write to list
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVar2Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar2->functionReturnReg = readVar2Reg.get();
    readVar2Reg->instruction = readVar2;
    readVar2->workVariable = &var2;
    instructionList.addInstruction(readVar2);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, readVar2Reg.get() });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // List should have Bool type from both variables
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_MixedTest, SingleListToMultipleVars_TypePropagation)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List sourceList("sourceList", "");
    Variable var1("var1", "");
    Variable var2("var2", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &sourceList;
    instructionList.addInstruction(clearList);

    // Initialize list with string values
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister listValue1(Compiler::StaticType::String, "first");
    appendList1->workList = &sourceList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &listValue1 });
    instructionList.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister listValue2(Compiler::StaticType::String, "second");
    appendList2->workList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &listValue2 });
    instructionList.addInstruction(appendList2);

    // Read first item and write to first variable
    auto readList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, nullptr, false);
    auto readList1Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readList1->functionReturnReg = readList1Reg.get();
    readList1Reg->instruction = readList1;
    LLVMConstantRegister index1(Compiler::StaticType::Number, 0);
    readList1->workList = &sourceList;
    readList1->args.push_back({ Compiler::StaticType::Number, &index1 });
    instructionList.addInstruction(readList1);

    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    writeVar1->workVariable = &var1;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, readList1Reg.get() });
    instructionList.addInstruction(writeVar1);

    // Read second item and write to second variable
    auto readList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, nullptr, false);
    auto readList2Reg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readList2->functionReturnReg = readList2Reg.get();
    readList2Reg->instruction = readList2;
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    readList2->workList = &sourceList;
    readList2->args.push_back({ Compiler::StaticType::Number, &index2 });
    instructionList.addInstruction(readList2);

    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    writeVar2->workVariable = &var2;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, readList2Reg.get() });
    instructionList.addInstruction(writeVar2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Both variables should have String type from the list
    ASSERT_EQ(analyzer.variableType(&var1, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
    ASSERT_EQ(analyzer.variableType(&var2, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_MixedTest, ComplexChain_VarToListToVar_TypePropagation)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable sourceVar("sourceVar", "");
    List intermediateList("intermediateList", "");
    Variable targetVar("targetVar", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &intermediateList;
    instructionList.addInstruction(clearList);

    // Initialize source variable
    auto writeSourceVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister sourceValue(Compiler::StaticType::Number, 3.14159);
    writeSourceVar->workVariable = &sourceVar;
    writeSourceVar->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    instructionList.addInstruction(writeSourceVar);

    // Read from source variable and write to intermediate list
    auto readSourceVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readSourceVarReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readSourceVar->functionReturnReg = readSourceVarReg.get();
    readSourceVarReg->instruction = readSourceVar;
    readSourceVar->workVariable = &sourceVar;
    instructionList.addInstruction(readSourceVar);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList->workList = &intermediateList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, readSourceVarReg.get() });
    instructionList.addInstruction(appendList);

    // Read from intermediate list and write to target variable
    auto readList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, nullptr, false);
    auto readListReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readList->functionReturnReg = readListReg.get();
    readListReg->instruction = readList;
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    readList->workList = &intermediateList;
    readList->args.push_back({ Compiler::StaticType::Number, &index });
    instructionList.addInstruction(readList);

    auto writeTargetVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    writeTargetVar->workVariable = &targetVar;
    writeTargetVar->args.push_back({ Compiler::StaticType::Unknown, readListReg.get() });
    instructionList.addInstruction(writeTargetVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Type should propagate through the chain: sourceVar -> intermediateList -> targetVar
    ASSERT_EQ(analyzer.variableType(&targetVar, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
    ASSERT_EQ(analyzer.listType(&intermediateList, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_MixedTest, NestedLoopWithMixedDependencies_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    Variable var("var", "");
    List list("list", "");

    auto outerLoopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoopStart);

    // Write to variable in outer loop
    auto writeVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    writeVar1->workVariable = &var;
    writeVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(writeVar1);

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto innerLoopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoopStart);

    // Read from variable and write to list in inner loop
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    auto readVarReg = std::make_shared<LLVMRegister>(Compiler::StaticType::Unknown);
    readVar->functionReturnReg = readVarReg.get();
    readVarReg->instruction = readVar;
    readVar->workVariable = &var;
    instructionList.addInstruction(readVar);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, readVarReg.get() });
    instructionList.addInstruction(appendList);

    // Write different type to variable in inner loop (creates conflict)
    auto writeVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "conflict");
    writeVar2->workVariable = &var;
    writeVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(writeVar2);

    auto innerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerLoopEnd);

    auto outerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerLoopEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown due to type conflicts in nested loops
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Unknown);
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}
