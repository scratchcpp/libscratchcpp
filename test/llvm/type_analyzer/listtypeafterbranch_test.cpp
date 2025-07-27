#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NullParams)
{
    LLVMTypeAnalyzer analyzer;
    ASSERT_EQ(analyzer.listTypeAfterBranch(nullptr, nullptr, Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NullList)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList list;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    list.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    list.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(nullptr, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NullStartInstruction)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, nullptr, Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, EmptyLoop)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    LLVMInstructionList instructionList;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, EmptyIfStatement)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    LLVMInstructionList instructionList;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, EmptyIfElseStatement_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    LLVMInstructionList instructionList;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, EmptyIfElseStatement_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    LLVMInstructionList instructionList;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, elseStart.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, EmptyLoopUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    LLVMInstructionList instructionList;

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Unknown, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NoWriteOperations)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendSameTypeNumber)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendSameTypeBool)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendSameTypeString)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "1.25");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Although string is appended, it's constant and represents a number
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendMultipleLists)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list1("", "");
    List list2("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Append to list1
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 6);
    appendList1->workList = &list1;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Append to list2
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list2;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list1, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendFromUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Unknown, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendUnknownToUnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMRegister value(Compiler::StaticType::Unknown);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Unknown, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleInsertSameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleReplaceSameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeNumberToString_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeNumberToString_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeStringToNumber_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeStringToNumber_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeNumberToBool_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeNumberToBool_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeBoolToNumber_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeBoolToNumber_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeBoolToString_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "abc");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeBoolToString_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "abc");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeStringToBool_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, true), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentTypeStringToBool_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendSameType_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentType_IfStatement_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleAppendDifferentType_IfStatement_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 5.8);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleInsertDifferentType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Insert might be a no-op depending on the index
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleInsertDifferentType_Empty_UnknownType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Insert might be a no-op depending on the index
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Unknown, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleInsertDifferentType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleReplaceDifferentType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Since the list is empty, it's safe to return any type
    // Let's return string in this case as the replace instruction indicates that string is used later
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, SingleReplaceDifferentType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Number, &index });
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsDifferentTypes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First append: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append: number 5.25 (compatible with Number pre-loop type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because first append creates type conflict with pre-loop Number type
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsDifferentTypes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First append: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append: number 5.25 (compatible with Number pre-loop type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because first append creates type conflict with pre-loop Number type
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsWithStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First append: number 2 (compatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 2);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append: string "3.14" (optimized to Number, compatible with pre-loop type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "3.14");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Number because final append is optimized to Number type
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsDifferentTypes_IfStatement)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    // First append: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append: number 5.25 (compatible with Number type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because first append creates type conflict with Number type
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsDifferentTypes_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    // First append: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append: number 5.25 (compatible with Number type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Else branch append: string "test" (incompatible with Number type)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because first append in if branch creates type conflict
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleAppendsSameType_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    // First append: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Else branch first append: string "abc" (incompatible with Number)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Else branch second append: number 5.25 (compatible with Number type)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 5.25);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because first append in else branch creates type conflict
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, elseStart.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, IfStatementInLoop_TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First append - changes type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second append - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, IfStatementInLoop_TypeChangeInElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First append - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second append - changes type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Returns unknown type because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, IfStatementInLoop_IfElseWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First append - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second append - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleInsertsDifferentTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First insert: number
    auto insertList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    insertList1->workList = &list;
    insertList1->args.push_back({ Compiler::StaticType::Number, &index1 });
    insertList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(insertList1);

    // Second insert: string (creates type conflict)
    auto insertList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    insertList2->workList = &list;
    insertList2->args.push_back({ Compiler::StaticType::Number, &index2 });
    insertList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleReplacesDifferentTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First replace: compatible with Bool
    auto replaceList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    replaceList1->workList = &list;
    replaceList1->args.push_back({ Compiler::StaticType::Number, &index1 });
    replaceList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(replaceList1);

    // Second replace: incompatible with Bool
    auto replaceList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 2);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    replaceList2->workList = &list;
    replaceList2->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(replaceList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, CombinedAppendInsertReplace_SameTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First: append number (compatible with Number)
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister appendValue(Compiler::StaticType::Number, 42);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &appendValue });
    instructionList.addInstruction(appendList);

    // Second: insert number (compatible with Number)
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister insertIndex(Compiler::StaticType::Number, 1);
    LLVMConstantRegister insertValue(Compiler::StaticType::Number, 5);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &insertIndex });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &insertValue });
    instructionList.addInstruction(insertList);

    // Third: replace with number (compatible with Number)
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister replaceIndex(Compiler::StaticType::Number, 2);
    LLVMConstantRegister replaceValue(Compiler::StaticType::Number, 3.14);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &replaceIndex });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &replaceValue });
    instructionList.addInstruction(replaceList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Number because only numbers are used
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, CombinedAppendInsertReplace_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First: append number (compatible with Number)
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister appendValue(Compiler::StaticType::Number, 42);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &appendValue });
    instructionList.addInstruction(appendList);

    // Second: insert string (creates type conflict)
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister insertIndex(Compiler::StaticType::Number, 0);
    LLVMConstantRegister insertValue(Compiler::StaticType::String, "hello");
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &insertIndex });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &insertValue });
    instructionList.addInstruction(insertList);

    // Third: replace with number (still conflicted)
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister replaceIndex(Compiler::StaticType::Number, 1);
    LLVMConstantRegister replaceValue(Compiler::StaticType::Number, 3.14);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &replaceIndex });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &replaceValue });
    instructionList.addInstruction(replaceList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // Should return Unknown because insert created type conflict
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, start.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NestedLoopWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List append inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List append inside inner loop (creates type conflict)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerLoop.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NestedLoopWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List append inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List append inside inner loop (same type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.5);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerLoop.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, NestedLoopWithTypeChangeBeforeLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List append inside outer loop (creates type conflict)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List append inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Returns unknown type because first append creates type conflict
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerLoop.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, IfStatementInLoopWithTypeChangeBeforeLoop_TypeChangeInIfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List append inside outer loop (creates type conflict)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf);

    // List append inside inner if statement if branch
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner if statement else branch begin
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(innerElse);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Returns unknown type because first append already created type conflict
    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerLoop.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleNestedLoopsWithTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List append inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop1);

    // List append inside inner loop 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop2);

    // List append inside inner loop 2 (creates type conflict)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerLoop.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(outerIf);

    // List append inside outer if statement
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf1);

    // List append inside inner if statement 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf2);

    // List append inside inner if statement 2 (creates type conflict)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "abc");
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(innerElse2);

    // List append inside inner if statement 2 else branch
    auto appendList4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 4);
    appendList4->workList = &list;
    appendList4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    instructionList.addInstruction(appendList4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerIf.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleNestedIfStatementsWithTypeChange_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(outerIf);

    // List append inside outer if statement
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf1);

    // List append inside inner if statement 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf2);

    // List append inside inner if statement 2
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(innerElse2);

    // List append inside inner if statement 2 else branch (creates type conflict)
    auto appendList4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "abc");
    appendList4->workList = &list;
    appendList4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    instructionList.addInstruction(appendList4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerIf.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListTypeAfterBranch, MultipleNestedIfStatementsWithoutTypeChange)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer if statement begin
    auto outerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(outerIf);

    // List append inside outer if statement
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner if statement 1 begin
    auto innerIf1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf1);

    // List append inside inner if statement 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner if statement 2 begin
    auto innerIf2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf2);

    // List append inside inner if statement 2
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 4);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner if statement 2 else branch
    auto innerElse2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(innerElse2);

    // List append inside inner if statement 2 else branch (same type)
    auto appendList4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value4(Compiler::StaticType::Number, 0);
    appendList4->workList = &list;
    appendList4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    instructionList.addInstruction(appendList4);

    // Inner if statement 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // Inner if statement 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    // Outer if statement end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listTypeAfterBranch(&list, outerIf.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}
