#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmloopanalyzer.h>
#include <engine/internal/llvm/llvmvariableptr.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NullParams)
{
    LLVMLoopAnalyzer analyzer;
    ASSERT_FALSE(analyzer.variableTypeChanges(nullptr, nullptr, Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NullVariable)
{
    LLVMLoopAnalyzer analyzer;

    LLVMInstruction funcCall(LLVMInstruction::Type::FunctionCall, nullptr, false);
    LLVMInstruction end(LLVMInstruction::Type::EndLoop, nullptr, false);
    funcCall.next = &end;
    end.previous = &funcCall;

    ASSERT_FALSE(analyzer.variableTypeChanges(nullptr, &funcCall, Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NullLoopBody)
{
    LLVMLoopAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, nullptr, Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, EmptyLoop)
{
    LLVMLoopAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, EmptyLoopUnknownType)
{
    LLVMLoopAnalyzer analyzer;
    LLVMVariablePtr varPtr;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Always returns true for unknown type
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NoWriteOperations)
{
    LLVMLoopAnalyzer analyzer;
    LLVMInstructionList list;
    LLVMVariablePtr varPtr;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteSameTypeNumber)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteSameTypeBool)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteSameTypeString)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteStringOptimization)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteMultipleVariables)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteFromUnknownType)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteToUnknownType)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteUnknownToUnknownType)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Unknown));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeNumberToString)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeStringToNumber)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeNumberToBool)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeBoolToNumber)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeBoolToString)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, SingleWriteDifferentTypeStringToBool)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesSameTypeNumber)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesSameTypeString)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesSameTypeUnknown)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::String));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesWithStringOptimization)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesNumberToString)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesBoolToNumber)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesBoolToString)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleWritesBoolToStringNumberOptimization)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Bool));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, TypeChangeInIfBranch)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, TypeChangeInElseBranch)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, IfElseWithoutTypeChange)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, start.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NestedLoopWithTypeChange)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NestedLoopWithoutTypeChange)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NestedLoopTypeChangesButResets)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NestedLoopTypeChangesAndDoesNotReset)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, NestedLoopWithTypeChangeBeforeLoop)
{
    LLVMLoopAnalyzer analyzer;
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
    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleNestedLoopsWithTypeChange)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_TRUE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

TEST(LLVMLoopAnalyzer_VariableTypeChanges, MultipleNestedLoopsWithTypeChangeButTypeReset)
{
    LLVMLoopAnalyzer analyzer;
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

    ASSERT_FALSE(analyzer.variableTypeChanges(&varPtr, outerLoop.get(), Compiler::StaticType::Number));
}

// TODO: Handle cross-variable dependencies
