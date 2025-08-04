#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NullParams)
{
    LLVMTypeAnalyzer analyzer;
    ASSERT_EQ(analyzer.variableTypeAfterBranch(nullptr, nullptr, Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NullVariable)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(nullptr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NullStartInstruction)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, nullptr, Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyLoop)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfElseStatement_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfElseStatement_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, elseStart.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyLoopUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NoWriteOperations)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "1.25");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Although string is assigned, it's constant and represents a number
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteMultipleVariables)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");
    Variable var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Set var1
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 6);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Set var2
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteFromUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteUnknownToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeNumberToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeStringToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeNumberToBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "abc");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeStringToBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: number 42 (incompatible with String)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (compatible with String pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop String type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeUnknown)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: unknown type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMRegister value1(Compiler::StaticType::Unknown);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (compatible with String pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop String type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesWithStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "3.14" (optimized to Number, compatible with pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "3.14");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return false because final assignment is optimized to Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesNumberToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: number 42 (compatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Number pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return true because final assignment is incompatible with pre-loop Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 4.25 (incompatible with Bool pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 4.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Else branch write: string "test" (incompatible with Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Else branch first write: string "abc" (incompatible with Number)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Else branch second write: number 5.25 (compatible with Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 5.25);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, elseStart.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Else branch write: compatible with Bool
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // First else branch write: compatible with Bool
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Second else branch write: string "hello" (incompatible with Bool type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "hello");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, elseStart.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // First write - changes type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Second write - does not change type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_TypeChangeInElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // First write - does not change type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Second write - changes type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_IfElseWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // First write - does not change type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    // Second write - does not change type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopTypeChangesButResets)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd);

    // Variable write after inner loop - resets type
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopTypeChangesAndDoesNotReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd);

    // Variable write after inner loop - keeps the different type
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithTypeChangeBeforeLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    // Returns unknown type because loops can be skipped
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoopWithTypeChangeBeforeLoop_TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf);

    // Variable write inside inner if statement if branch
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement else branch begin
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    // Returns unknown type because if statements can be skipped
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoopWithTypeChangeBeforeLoop_TypeChangeInElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf);

    // Inner if statement else branch begin
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse);

    // Variable write inside inner if statement else branch
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    // Returns unknown type because if statements can be skipped
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoopWithTypeChangeBeforeLoop_TypeChangeInBothBranches)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    // Inner if statement else branch begin
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse);

    // Variable write inside inner if statement else branch
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    // Returns number type because the type always changes to number
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedLoopsWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop1);

    // Variable write inside inner loop 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop2);

    // Variable write inside inner loop 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd2);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd1);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedLoopsWithTypeChangeButTypeReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop1);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop2);

    // Variable write inside inner loop 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd2);

    // Variable write inside inner loop 1 - resets type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerEnd1);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 4);
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "abc");
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 0);
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_SimpleAssignment)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var2 = "test" (String type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have String type due to cross-variable dependency
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_TypeMismatch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var2 = "test" (String type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency with type mismatch)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have String type (incompatible with Number pre-loop type)
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_SimpleAssignmentBeforeTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // var1 = var2
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // var2 = 5.2 (type resets to number)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.2);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Although the type of var2 is changed to number, it's unknown before the first iteration
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_ChainedAssignments)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", ""), var3("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var3 = 42 (Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar3->workVariable = &var3;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar3);

    // Second write: var2 = var3 (cross-variable dependency)
    auto readVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar3);
    readVar3->workVariable = &var3;

    LLVMRegister var3Value(Compiler::StaticType::Unknown);
    var3Value.isRawValue = false;
    var3Value.instruction = readVar3;
    readVar3->functionReturnReg = &var3Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var3Value });
    list.addInstruction(setVar2);

    // Third write: var1 = var2 (chained cross-variable dependency)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have Number type through the chain var1 = var2 = var3 = 42
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_CircularReference)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return Unknown due to circular dependency
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_CircularReference_KnownTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // var1 is a number
    auto setupVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setupVar1->workVariable = &var1;
    setupVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setupVar1);

    // var2 is a number
    auto setupVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    setupVar2->workVariable = &var2;
    setupVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setupVar2);

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Although there is a circular dependency, the types of both variables are known
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_CircularReference_FirstAssignedTypeKnown)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // var1: unknown type

    // var2 is a number
    auto setupVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    setupVar2->workVariable = &var2;
    setupVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setupVar2);

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Although there is a circular dependency, the type of var2 is known and this variable is assigned to var1
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_CircularReference_AssignedTypeUnknown)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // var1 is a number
    auto setupVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setupVar1->workVariable = &var1;
    setupVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setupVar1);

    // var2: unknown type

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // The type of var2 is not known and this variable is assigned to var1
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_ChainedCircularReference)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", ""), var3("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var3
    auto readVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar3);
    readVar3->workVariable = &var3;

    LLVMRegister var3Value(Compiler::StaticType::Unknown);
    var3Value.isRawValue = false;
    var3Value.instruction = readVar3;
    readVar3->functionReturnReg = &var3Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var3Value });
    list.addInstruction(setVar2);

    // Third write: var3 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar3->workVariable = &var3;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return Unknown due to circular dependency
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, CrossVariableDependency_InIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(start);

    // First write: var2 = 3.14 (Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency in if statement)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(end);

    // var1 should have Number type from cross-variable dependency
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SelfAssignment)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Start if statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar1);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    // End loop
    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return the previous type because the write is a no-op
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SelfAssignmentWithTypeChange_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First write: var1 = 3.14 (Number type)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Start if statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar2->workVariable = &var1;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    // End loop
    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return the previous type (number) because the write is a no-op
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SelfAssignmentWithTypeChange_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Start if statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar1);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    // var1 = 3.14 (Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 3.14);
    setVar2->workVariable = &var1;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // End loop
    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // Should return number because it's assigned later in the loop
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_AddNumbers)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Create add instruction: 5 + 3
    auto addInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Add, false);
    LLVMConstantRegister operand1(Compiler::StaticType::Number, 5);
    LLVMConstantRegister operand2(Compiler::StaticType::Number, 3);
    addInstruction->args.push_back({ Compiler::StaticType::Number, &operand1 });
    addInstruction->args.push_back({ Compiler::StaticType::Number, &operand2 });

    // Set up return register for add instruction
    LLVMRegister addResult(Compiler::StaticType::Number);
    addResult.isRawValue = true;
    addResult.instruction = addInstruction;
    addInstruction->functionReturnReg = &addResult;
    list.addInstruction(addInstruction);

    // Assign add result to variable: var1 = (5 + 3)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &addResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have Number type from add instruction return
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_StringConcat)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Create string concatenation instruction: "hello" + "world"
    auto concatInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::StringConcat, false);
    LLVMConstantRegister str1(Compiler::StaticType::String, "hello");
    LLVMConstantRegister str2(Compiler::StaticType::String, "world");
    concatInstruction->args.push_back({ Compiler::StaticType::String, &str1 });
    concatInstruction->args.push_back({ Compiler::StaticType::String, &str2 });

    // Set up return register for concat instruction
    LLVMRegister concatResult(Compiler::StaticType::String);
    concatResult.isRawValue = true;
    concatResult.instruction = concatInstruction;
    concatInstruction->functionReturnReg = &concatResult;
    list.addInstruction(concatInstruction);

    // Assign concat result to variable: var1 = ("hello" + "world")
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &concatResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have String type from concat instruction return
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_MathFunction)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Create sqrt instruction: sqrt(16)
    auto sqrtInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Sqrt, false);
    LLVMConstantRegister operand(Compiler::StaticType::Number, 16);
    sqrtInstruction->args.push_back({ Compiler::StaticType::Number, &operand });

    // Set up return register for sqrt instruction
    LLVMRegister sqrtResult(Compiler::StaticType::Number);
    sqrtResult.isRawValue = true;
    sqrtResult.instruction = sqrtInstruction;
    sqrtInstruction->functionReturnReg = &sqrtResult;
    list.addInstruction(sqrtInstruction);

    // Assign sqrt result to variable: var1 = sqrt(16)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &sqrtResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have Number type from sqrt instruction return
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_Comparison)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Create comparison instruction: 5 > 3
    auto cmpInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::CmpGT, false);
    LLVMConstantRegister operand1(Compiler::StaticType::Number, 5);
    LLVMConstantRegister operand2(Compiler::StaticType::Number, 3);
    cmpInstruction->args.push_back({ Compiler::StaticType::Number, &operand1 });
    cmpInstruction->args.push_back({ Compiler::StaticType::Number, &operand2 });

    // Set up return register for comparison instruction
    LLVMRegister cmpResult(Compiler::StaticType::Bool);
    cmpResult.isRawValue = true;
    cmpResult.instruction = cmpInstruction;
    cmpInstruction->functionReturnReg = &cmpResult;
    list.addInstruction(cmpInstruction);

    // Assign comparison result to variable: var1 = (5 > 3)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &cmpResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have Bool type from comparison instruction return
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_FunctionCall)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // Create function call instruction
    auto funcInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, false);
    funcInstruction->functionName = "some_reporter_function";
    LLVMConstantRegister arg(Compiler::StaticType::Number, 42);
    funcInstruction->args.push_back({ Compiler::StaticType::Number, &arg });

    // Set up return register for function call
    LLVMRegister funcResult(Compiler::StaticType::String);
    funcResult.isRawValue = true;
    funcResult.instruction = funcInstruction;
    funcInstruction->functionReturnReg = &funcResult;
    list.addInstruction(funcInstruction);

    // Assign function result to variable: var1 = some_reporter_function(42)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &funcResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have String type from function call return
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, InstructionReturnType_ChainedOperations)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(start);

    // First operation: 5 + 3
    auto addInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Add, false);
    LLVMConstantRegister operand1(Compiler::StaticType::Number, 5);
    LLVMConstantRegister operand2(Compiler::StaticType::Number, 3);
    addInstruction->args.push_back({ Compiler::StaticType::Number, &operand1 });
    addInstruction->args.push_back({ Compiler::StaticType::Number, &operand2 });

    LLVMRegister addResult(Compiler::StaticType::Number);
    addResult.isRawValue = true;
    addResult.instruction = addInstruction;
    addInstruction->functionReturnReg = &addResult;
    list.addInstruction(addInstruction);

    // Second operation: (5 + 3) * 2
    auto mulInstruction = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::Mul, false);
    LLVMConstantRegister multiplier(Compiler::StaticType::Number, 2);
    mulInstruction->args.push_back({ Compiler::StaticType::Unknown, &addResult });
    mulInstruction->args.push_back({ Compiler::StaticType::Number, &multiplier });

    LLVMRegister mulResult(Compiler::StaticType::Number);
    mulResult.isRawValue = true;
    mulResult.instruction = mulInstruction;
    mulInstruction->functionReturnReg = &mulResult;
    list.addInstruction(mulInstruction);

    // Assign final result to variable: var1 = ((5 + 3) * 2)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &mulResult });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(end);

    // var1 should have Number type from chained operations
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&var1, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}
