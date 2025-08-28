#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmcodeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, FirstVariableWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.0);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    // First write should have Unknown targetType (no previous type)
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, SecondVariableWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    analyzer.analyzeScript(list);

    // First write has no previous type
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    // Second write has Number type from first write
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, MultipleWritesSameType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "first");
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "second");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "third");
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::String);
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, StringOptimization)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "3.14");
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    // String "3.14" optimized to Number, so second write sees Number type
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, StringOptimization_DifferentString)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "1.0");
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    // String "1.0" does NOT get optimized to Number because it would convert to "1"
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, LoopSingleWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // Loop convergence: first iteration Unknown, subsequent iterations Number
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WhileLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Establish bool type before loop
    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto loopCond = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginLoopCondition, false);
    list.addInstruction(loopCond);

    // Read the variable in loop condition
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, true);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister varValue(Compiler::StaticType::Unknown);
    varValue.isRawValue = false;
    varValue.instruction = readVar;
    readVar->functionReturnReg = &varValue;

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, false);
    list.addInstruction(loopStart);

    // Change the type in the loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(readVar->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, RepeatUntilLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Establish bool type before loop
    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto loopCond = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginLoopCondition, false);
    list.addInstruction(loopCond);

    // Read the variable in loop condition
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, true);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister varValue(Compiler::StaticType::Unknown);
    varValue.isRawValue = false;
    varValue.instruction = readVar;
    readVar->functionReturnReg = &varValue;

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(loopStart);

    // Change the type in the loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(readVar->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, ProcedureCallInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto procCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::CallProcedure, false);
    list.addInstruction(procCall);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);

    // Type unknown due to procedure call
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, LoopMultipleWrites_UnknownType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // First write: Unknown (unknown before loop)
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);

    // Second write: Number
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, LoopMultipleWrites_KnownType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Establish bool type before loop
    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // First write: Bool | String (from loop iterations)
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Bool | Compiler::StaticType::String);

    // Second write: Number
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, IfElseStatementSameTypes)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    // Both writes see Unknown before the if-else
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);

    // The type is always Number after the if statement
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, IfElseStatementDifferentTypes)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    // Both writes see Unknown before the if-else
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);

    // The type is Number or String after the if statement
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, IfElseStatementDifferentType_IfBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);

    // The type is Number or String after the if statement
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, IfElseStatementDifferentType_ElseBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);

    // The type is Number or String after the if statement
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteBeforeLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVarBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    setVarBefore->targetVariable = &var;
    setVarBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(setVarBefore);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVarInLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInLoop(Compiler::StaticType::Number, 42);
    setVarInLoop->targetVariable = &var;
    setVarInLoop->args.push_back({ Compiler::StaticType::Unknown, &valueInLoop });
    list.addInstruction(setVarInLoop);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    // The type might be String or Number because the loop might or might not run
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteBeforeIfStatement_IfBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVarBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    setVarBefore->targetVariable = &var;
    setVarBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(setVarBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVarInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    setVarInIfStatement->targetVariable = &var;
    setVarInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(setVarInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    // The type might be String or Number because the if statement might or might not run
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteBeforeIfStatement_ElseBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVarBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    setVarBefore->targetVariable = &var;
    setVarBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(setVarBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVarInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    setVarInIfStatement->targetVariable = &var;
    setVarInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(setVarInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    // The type might be String or Number because the else branch might or might not run
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteBeforeIfElse)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVarBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::Bool, true);
    setVarBefore->targetVariable = &var;
    setVarBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(setVarBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    // Write before if-else establishes Bool type
    ASSERT_EQ(setVarBefore->targetType, Compiler::StaticType::Unknown);

    // Both writes in branches see Bool type from before
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Bool);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Bool);

    // The type is Number or String after the if statement
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteInIfStatement_IfBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVarInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    setVarInIfStatement->targetVariable = &var;
    setVarInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(setVarInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVarInIfStatement->targetType, Compiler::StaticType::Unknown);

    // The type is Unknown because the if statement might not run at all
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteInIfStatement_ElseBranch)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVarInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    setVarInIfStatement->targetVariable = &var;
    setVarInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(setVarInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVarInIfStatement->targetType, Compiler::StaticType::Unknown);

    // The type is Unknown because the if statement might not run at all
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteInIfElse)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);

    // The type is Number | String because any of the branches may run
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, WriteInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto setVarInLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueInLoop(Compiler::StaticType::Number, 42);
    setVarInLoop->targetVariable = &var;
    setVarInLoop->args.push_back({ Compiler::StaticType::Unknown, &valueInLoop });
    list.addInstruction(setVarInLoop);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    analyzer.analyzeScript(list);

    ASSERT_EQ(setVarInLoop->targetType, Compiler::StaticType::Unknown);

    // The type is Unknown because the loop might not run at all
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, ComplexNestedControlFlow)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "nested");
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto innerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerLoopEnd);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto outerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerLoopEnd);

    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value4(Compiler::StaticType::Bool, true);
    setVar4->targetVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    analyzer.analyzeScript(list);

    // Complex analysis with multiple execution paths and loop convergence
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(setVar4->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, MultipleVariablesSeparateAnalysis)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->targetVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    analyzer.analyzeScript(list);

    // Both are first writes for their respective variables
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, CrossVariableDependency_SingleType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->targetVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var2;
    list.addInstruction(readVar2);

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1_1->targetVariable = &var1;
    setVar1_1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1_1);

    auto setVar1_2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar1_2->targetVariable = &var1;
    setVar1_2->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1_2);

    analyzer.analyzeScript(list);

    // var2 first write has no previous type
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Unknown);

    // var2 has Number type at read operation
    ASSERT_EQ(readVar2->targetType, Compiler::StaticType::Number);

    // var1 first write has no previous type
    ASSERT_EQ(setVar1_1->targetType, Compiler::StaticType::Unknown);

    // var1 second write has Number type
    ASSERT_EQ(setVar1_2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, CrossVariableDependency_MultipleTypes)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    // Establish Number | Bool types
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var2;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->targetVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var2;
    list.addInstruction(readVar2);

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar3->targetVariable = &var1;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar3);

    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "test");
    setVar4->targetVariable = &var1;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar4);

    analyzer.analyzeScript(list);

    // var2 has Number or Bool type at read operation
    ASSERT_EQ(readVar2->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);

    // var1 first write has no previous type
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Unknown);

    // var1 second write has Number or Bool type
    ASSERT_EQ(setVar4->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, ChainedAssignmentsInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable varA("a", ""), varB("b", ""), varC("c", "");

    // a = 5
    auto setA1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueA1(Compiler::StaticType::Number, 5);
    setA1->targetVariable = &varA;
    setA1->args.push_back({ Compiler::StaticType::Unknown, &valueA1 });
    list.addInstruction(setA1);

    // b = 3
    auto setB1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueB1(Compiler::StaticType::Number, 3);
    setB1->targetVariable = &varB;
    setB1->args.push_back({ Compiler::StaticType::Unknown, &valueB1 });
    list.addInstruction(setB1);

    // c = 8
    auto setC1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueC1(Compiler::StaticType::Number, 8);
    setC1->targetVariable = &varC;
    setC1->args.push_back({ Compiler::StaticType::Unknown, &valueC1 });
    list.addInstruction(setC1);

    // loop start
    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    // a = b
    auto readB = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readB->targetVariable = &varB;
    list.addInstruction(readB);

    LLVMRegister bValue(Compiler::StaticType::Unknown);
    bValue.isRawValue = false;
    bValue.instruction = readB;
    readB->functionReturnReg = &bValue;

    auto setA2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setA2->targetVariable = &varA;
    setA2->args.push_back({ Compiler::StaticType::Unknown, &bValue });
    list.addInstruction(setA2);

    // b = c
    auto readC1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readC1->targetVariable = &varC;
    list.addInstruction(readC1);

    LLVMRegister cValue1(Compiler::StaticType::Unknown);
    cValue1.isRawValue = false;
    cValue1.instruction = readC1;
    readC1->functionReturnReg = &cValue1;

    auto setB2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setB2->targetVariable = &varB;
    setB2->args.push_back({ Compiler::StaticType::Unknown, &cValue1 });
    list.addInstruction(setB2);

    // c = "test"
    auto setC2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister valueC2(Compiler::StaticType::String, "test");
    setC2->targetVariable = &varC;
    setC2->args.push_back({ Compiler::StaticType::Unknown, &valueC2 });
    list.addInstruction(setC2);

    // loop end
    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    // a = c (final assignment we want to test)
    auto readC2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readC2->targetVariable = &varC;
    list.addInstruction(readC2);

    LLVMRegister cValue2(Compiler::StaticType::Unknown);
    cValue2.isRawValue = false;
    cValue2.instruction = readC2;
    readC2->functionReturnReg = &cValue2;

    auto setA3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setA3->targetVariable = &varA;
    setA3->args.push_back({ Compiler::StaticType::Unknown, &cValue2 });
    list.addInstruction(setA3);

    analyzer.analyzeScript(list);

    // Check the type of 'a' before the final a = c assignment
    // 'a' could be Number (from initial assignment) or String (from loop iterations where a=b, b=c, c="test")
    auto expectedAType = Compiler::StaticType::Number | Compiler::StaticType::String;
    ASSERT_EQ(setA3->targetType, expectedAType);

    // Check the type of 'c' before the final a = c assignment
    // 'c' could be Number (from initial assignment) or String (from loop assignment c="test")
    auto expectedCType = Compiler::StaticType::Number | Compiler::StaticType::String;
    // We need to check what type 'c' has at the point of the final read
    // This would be determined by the loop convergence analysis
    ASSERT_EQ(readC2->targetType, expectedCType);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, SelfAssignment)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // First establish a type for the variable
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->targetVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Self-assignment: var = var
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister varValue(Compiler::StaticType::Unknown);
    varValue.isRawValue = false;
    varValue.instruction = readVar;
    readVar->functionReturnReg = &varValue;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->targetVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &varValue });
    list.addInstruction(setVar2);

    // Should still be Number
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    setVar3->targetVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    analyzer.analyzeScript(list);

    // First write should have Unknown targetType (no previous type)
    ASSERT_EQ(setVar1->targetType, Compiler::StaticType::Unknown);

    // Self-assignment should see the Number type from the previous write
    ASSERT_EQ(setVar2->targetType, Compiler::StaticType::Number);

    // The variable should still have the Number type from the first write
    ASSERT_EQ(setVar3->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, SelfAssignmentInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    // Self-assignment inside loop: var = var
    auto readVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar->targetVariable = &var;
    list.addInstruction(readVar);

    LLVMRegister varValue(Compiler::StaticType::Unknown);
    varValue.isRawValue = false;
    varValue.instruction = readVar;
    readVar->functionReturnReg = &varValue;

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar->targetVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &varValue });
    list.addInstruction(setVar);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // Self-assignment in loop should maintain Unknown type since it's a no-op
    // and doesn't change the variable's type across iterations
    ASSERT_EQ(setVar->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, VariableReadReturnRegType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->targetVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var2;
    list.addInstruction(readVar2);

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1_1->targetVariable = &var1;
    setVar1_1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1_1);

    analyzer.analyzeScript(list);

    // var2 read return register has Number type
    ASSERT_EQ(var2Value.type(), Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_VariableTypeAnalysis, CrossVariableWriteArgType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->targetVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    readVar2->targetVariable = &var2;
    list.addInstruction(readVar2);

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1_1->targetVariable = &var1;
    setVar1_1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1_1);

    analyzer.analyzeScript(list);

    // last write argument has Number type
    ASSERT_EQ(setVar1_1->args.front().first, Compiler::StaticType::Number);
}
