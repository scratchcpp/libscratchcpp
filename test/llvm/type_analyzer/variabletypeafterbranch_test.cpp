#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvmvariableptr.h>
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

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(nullptr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NullStartInstruction)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, nullptr, Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfElseStatement_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyIfElseStatement_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, elseStart.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, EmptyLoopUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NoWriteOperations)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "1.25");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Although string is assigned, it's constant and represents a number
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteMultipleVariables)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var1("", "");
    Variable var2("", "");
    varPtr.var = &var1;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // Set var1
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 6);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Set var2
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteFromUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteUnknownToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeNumberToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeStringToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeNumberToBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "abc");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentTypeStringToBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteSameType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, SingleWriteDifferentType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: number 42 (incompatible with String)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (compatible with String pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop String type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameTypeUnknown)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: unknown type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value1(Compiler::StaticType::Unknown);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (compatible with String pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with pre-loop String type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesWithStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "3.14" (optimized to Number, compatible with pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "3.14");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is optimized to Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesNumberToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: number 42 (compatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Number pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Should return true because final assignment is incompatible with pre-loop Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 4.25 (incompatible with Bool pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, 4.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool pre-loop type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: number 5.25 (compatible with Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Else branch write: string "test" (incompatible with Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesSameType_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: string "abc" (incompatible with Number)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Else branch first write: string "abc" (incompatible with Number)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Else branch second write: number 5.25 (compatible with Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 5.25);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Should return false because final assignment is compatible with Number type
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, elseStart.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Second write: string "hello" (incompatible with Bool type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Else branch write: compatible with Bool
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleWritesDifferentTypes_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: compatible with Bool
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // First else branch write: compatible with Bool
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Second else branch write: string "hello" (incompatible with Bool type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "hello");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, elseStart.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(ifStart);

    // First write - changes type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Second write - does not change type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_TypeChangeInElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(ifStart);

    // First write - does not change type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Second write - changes type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, IfStatementInLoop_IfElseWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(ifStart);

    // First write - does not change type
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Second write - does not change type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, start.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopTypeChangesButResets)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Variable write after inner loop - resets type
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopTypeChangesAndDoesNotReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, nullptr, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Variable write after inner loop - keeps the different type
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, NestedLoopWithTypeChangeBeforeLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop);

    // Variable write inside inner loop
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    // Returns unknown type because loops can be skipped
    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedLoopsWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop1);

    // Variable write inside inner loop 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop2);

    // Variable write inside inner loop 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd2);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd1);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedLoopsWithTypeChangeButTypeReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop1);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(innerLoop2);

    // Variable write inside inner loop 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd2);

    // Variable write inside inner loop 1 - resets type
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd1);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerLoop.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 4);
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "abc");
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableTypeAfterBranch, MultipleNestedIfStatementsWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(outerIf);

    // Variable write inside outer if statement
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf1);

    // Variable write inside inner if statement 1
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf2);

    // Variable write inside inner if statement 2
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(innerElse2);

    // Variable write inside inner if statement 2 else branch
    auto setVar4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 0);
    setVar4->workVariable = &var;
    setVar4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(setVar4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableTypeAfterBranch(&varPtr, outerIf.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

// TODO: Handle cross-variable dependencies
