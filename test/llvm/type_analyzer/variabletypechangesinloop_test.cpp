#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvmvariableptr.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NullParams)
{
    LLVMTypeAnalyzer analyzer;
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(nullptr, nullptr, Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NullVariable)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(nullptr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NullLoopBody)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, nullptr, Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, EmptyLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, EmptyLoopUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Always returns true for unknown type
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NoWriteOperations)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteSameTypeNumber)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteSameTypeBool)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteSameTypeString)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteStringOptimization)
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
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteMultipleVariables)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteFromUnknownType)
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

    // Always returns true for unknown type
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteToUnknownType)
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

    // Always returns true for unknown type
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteUnknownToUnknownType)
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

    // Always returns true for unknown type
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeNumberToString)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeStringToNumber)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeNumberToBool)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeBoolToNumber)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeBoolToString)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, SingleWriteDifferentTypeStringToBool)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesSameTypeNumber)
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
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesSameTypeString)
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
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesSameTypeUnknown)
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
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesWithStringOptimization)
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
    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesNumberToString)
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
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "hello");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleWritesBoolToStringNumberOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;
    Variable var("", "");
    varPtr.var = &var;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "3.14");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // String "3.14" gets optimized to Number, which is still different from Bool
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, TypeChangeInIfBranch)
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

    // Returns true because the type-changing branch might run
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, TypeChangeInElseBranch)
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

    // Returns true because the type-changing branch might run
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, IfElseWithoutTypeChange)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NestedLoopWithTypeChange)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NestedLoopWithoutTypeChange)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NestedLoopTypeChangesButResets)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NestedLoopTypeChangesAndDoesNotReset)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, NestedLoopWithTypeChangeBeforeLoop)
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

    // Returns true because loops can be skipped
    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleNestedLoopsWithTypeChange)
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

    ASSERT_TRUE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMTypeAnalyzer_VariableTypeChangesInLoop, MultipleNestedLoopsWithTypeChangeButTypeReset)
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

    ASSERT_FALSE(analyzer.variableTypeChangesInLoop(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

// TODO: Handle cross-variable dependencies
