#include <scratchcpp/variable.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_VariableType, NullParams)
{
    LLVMTypeAnalyzer analyzer;
    ASSERT_EQ(analyzer.variableType(nullptr, nullptr, Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, NullVariable)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(nullptr, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, NullPos)
{
    LLVMTypeAnalyzer analyzer;
    Variable var("", "");
    ASSERT_EQ(analyzer.variableType(&var, nullptr, Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, NoWriteOperationsKnownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, NoWriteOperationsUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeNumber_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeNumber_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeBool_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeBool_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeString_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteSameTypeString_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "1.25");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // Although string is assigned, it's constant and represents a number
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteMultipleVariables)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");
    Variable var2("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var1, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteFromUnknownType_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteFromUnknownType_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteToUnknownType_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // The type is known because a number is assigned before the point
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteToUnknownType_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    // The type is not known because a number is assigned after the point
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteUnknownToUnknownType_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteUnknownToUnknownType_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeNumberToString_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeNumberToString_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeStringToNumber_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeNumberToBool_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeBoolToNumber_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeBoolToNumber_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeBoolToString_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "abc");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeStringToBool_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentTypeStringToBool_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, Loop_SingleWriteDifferentType_BeforeAndAfter)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, IfStatement_SingleWriteDifferentType_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, IfStatement_SingleWriteDifferentType_After_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Since this isn't a loop, the write after the point doesn't affect the type
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, IfStatement_SingleWriteDifferentType_After_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Since this isn't a loop, the write after the point doesn't affect the type
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, WriteSameTypeInLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, WriteDifferentTypeInLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Type is not known because the loop might not run at all
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, OverrideDifferentTypeBeforeLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, OverrideUnknownTypeBeforeLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, WriteSameTypeInIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, WriteDifferentTypeInIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Type is not known because the if statement might not run at all
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, OverrideDifferentTypeBeforeIfElseStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.5);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, OverrideUnknownTypeBeforeIfElseStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.5);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Unknown), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return Number type because final assignment writes a number
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return String type because final assignment writes a string
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesSameTypeUnknown)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return String type because final assignment writes a string
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesWithStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return Number type because final assignment writes a string which is optimized to Number type
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesNumberToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return String type because final assignment writes a string
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesBoolToNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.5);
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return Number type because final assignment writes a number
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesBoolToString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "hello");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Should return String type because final assignment writes a string
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleWritesBoolToStringNumberOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto setVar = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "3.14");
    setVar->workVariable = &var;
    setVar->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(setVar);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // String "3.14" gets optimized to Number, which is still different from Bool
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Returns unknown type because both branches might run
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, TypeChangeInElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Returns unknown type because both branches might run
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, IfElseWithEqualTypes_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, IfElseWithEqualTypes_DifferentTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(ifStart);

    // First write - changes type to string
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "Lorem");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(elseStart);

    // Second write - changes type to string
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "ipsum");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopWithTypeChange_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

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

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopWithTypeChange_BeforeInner)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Variable write inside outer loop
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

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

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopWithTypeChange_AfterWriteInsideInner)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopWithTypeChange_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    // Type is not known because the inner loop might not run at all
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopTypeChangesButResets)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, NestedLoopTypeChangesAndDoesNotReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleNestedLoopsWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, MultipleNestedLoopsWithTypeChangeButTypeReset)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

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
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd2);

    // Variable write inside inner loop 1 - resets type
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.75);
    setVar3->workVariable = &var;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(setVar3);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(innerEnd1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, SameTypeIfElseInLoopWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(outerLoop);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(innerIf);

    // Variable write inside inner if statement if branch
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    setVar1->workVariable = &var;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // Inner if statement else branch
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    list.addInstruction(innerElse);

    // Variable write inside inner if statement else branch
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "def");
    setVar2->workVariable = &var;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(outerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    // The type always changes to string
    ASSERT_EQ(analyzer.variableType(&var, funcCall.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_SimpleAssignment)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    // First write: var2 = "test" (String type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Query position after the assignment
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // var1 should have String type due to cross-variable dependency
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_TypeMismatch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    // First write: var2 = "test" (String type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency with type mismatch)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Query position after the assignment
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // var1 should have String type (different from Number previousType)
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::Number), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_QueryBeforeAssignment)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    // Query position before any assignments
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // First write: var2 = "test" (String type) - after query position
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency) - after query position
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // var1 should have the previous type since assignments are after query position
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::Number), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_ChainedAssignments)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", ""), var3("", "");

    // First write: var3 = 42 (Number type)
    auto setVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    setVar3->workVariable = &var3;
    setVar3->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar3);

    // Second write: var2 = var3 (cross-variable dependency)
    auto readVar3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar3);
    readVar3->workVariable = &var3;

    LLVMRegister var3Value(Compiler::StaticType::Unknown);
    var3Value.isRawValue = false;
    var3Value.instruction = readVar3;
    readVar3->functionReturnReg = &var3Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var3Value });
    list.addInstruction(setVar2);

    // Third write: var1 = var2 (chained cross-variable dependency)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Query position after all assignments
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // var1 should have Number type through the chain var1 = var2 = var3 = 42
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::Bool), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_CircularReference)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    // First write: var1 = var2 (circular dependency setup)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Second write: var2 = var1 (completes circular dependency)
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    // Query position after circular assignments
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // Should return Unknown due to circular dependency
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::Number), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_InIfStatement_QueryInside)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: var2 = 3.14 (Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency in if statement)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    // Query position inside if statement after assignments
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // var1 should have Number type from cross-variable dependency
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, CrossVariableDependency_InIfStatement_QueryOutside)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", ""), var2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    list.addInstruction(start);

    // First write: var2 = 3.14 (Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar2->workVariable = &var2;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar2);

    // Second write: var1 = var2 (cross-variable dependency in if statement)
    auto readVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar2);
    readVar2->workVariable = &var2;

    LLVMRegister var2Value(Compiler::StaticType::Unknown);
    var2Value.isRawValue = false;
    var2Value.instruction = readVar2;
    readVar2->functionReturnReg = &var2Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var2Value });
    list.addInstruction(setVar1);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    list.addInstruction(end);

    // Query position outside if statement after it ends
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // var1 should return Unknown because the write is inside an if statement that may not execute
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::String), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_VariableType, SelfAssignment)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar1);

    // Query position after self-assignment
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // Should return the previous type because the write is a no-op
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::String), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_VariableType, SelfAssignmentWithTypeChange_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    // First write: var1 = 3.14 (Number type)
    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(setVar1);

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar2->workVariable = &var1;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar2);

    // Query position after self-assignment
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // Should return the previous type (number) because the write is a no-op
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_VariableType, SelfAssignmentWithTypeChange_After)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;
    Variable var1("", "");

    // No-op: var1 = var1
    auto readVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ReadVariable, nullptr, false);
    list.addInstruction(readVar1);
    readVar1->workVariable = &var1;

    LLVMRegister var1Value(Compiler::StaticType::Unknown);
    var1Value.isRawValue = false;
    var1Value.instruction = readVar1;
    readVar1->functionReturnReg = &var1Value;

    auto setVar1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    setVar1->workVariable = &var1;
    setVar1->args.push_back({ Compiler::StaticType::Unknown, &var1Value });
    list.addInstruction(setVar1);

    // var1 = 3.14 (Number type)
    auto setVar2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::WriteVariable, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 3.14);
    setVar2->workVariable = &var1;
    setVar2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(setVar2);

    // Query position after real assignment
    auto queryPos = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(queryPos);

    // Should return number because it's assigned after the no-op
    ASSERT_EQ(analyzer.variableType(&var1, queryPos.get(), Compiler::StaticType::String), Compiler::StaticType::Number);
}
