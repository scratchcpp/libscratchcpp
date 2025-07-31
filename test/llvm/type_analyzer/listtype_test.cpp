#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmtypeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMTypeAnalyzer_ListType, NullParams)
{
    LLVMTypeAnalyzer analyzer;
    ASSERT_EQ(analyzer.listType(nullptr, nullptr, Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NullList)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(nullptr, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NullPos)
{
    LLVMTypeAnalyzer analyzer;
    List list("", "");
    ASSERT_EQ(analyzer.listType(&list, nullptr, Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NoWriteOperationsKnownType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, NoWriteOperationsKnownType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, NoWriteOperationsUnknownType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NoWriteOperationsUnknownType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteSameTypeNumber_Before_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteSameTypeNumber_Before_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteStringOptimization_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // String "1.25" gets optimized to Number type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteMultipleLists)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list1, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteFromUnknownType_Before_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteFromUnknownType_Before_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteToUnknownType_Before_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    // The type is known because a number is appended before the point
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteToUnknownType_Before_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteDifferentTypeNumberToString_Before_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteDifferentTypeNumberToString_Before_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteDifferentTypeBoolToNumber_Before_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, Loop_SingleWriteDifferentType_BeforeAndAfter)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatement_SingleWriteDifferentType_Before)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatement_SingleWriteDifferentType_After_IfBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    // Since this isn't a loop, the write after the point doesn't affect the type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatement_SingleWriteDifferentType_After_ElseBranch)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    // Since this isn't a loop, the write after the point doesn't affect the type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, WriteSameTypeInLoop_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, WriteSameTypeInLoop_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, WriteDifferentTypeInLoop_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Type is not known because the loop might not run at all
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, WriteDifferentTypeInLoop_Empty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Type is String because empty list can establish new type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ChangeTypeBeforeLoop_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, OverrideUnknownTypeBeforeLoop_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, WriteSameTypeInIfStatement_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, WriteDifferentTypeInIfStatement_NonEmpty)
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Type is not known because the if statement might not run at all
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, OverrideDifferentTypeBeforeIfElseStatement_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.5);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, OverrideUnknownTypeBeforeIfElseStatement_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1.25);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(start);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.5);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Unknown, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesSameTypeNumber_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: string "abc" (incompatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: number 5.25 (incompatible with previous String)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because of type conflict
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesSameTypeString_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: number 42 (establishes Number type for empty list)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: string "hello" (incompatible with Number)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because of type conflict
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesSameTypeUnknown_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: unknown type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMRegister value1(Compiler::StaticType::Unknown);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: string "hello"
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because first write is unknown
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesWithStringOptimization_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: number 5
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: string "3.14" (optimized to Number)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "3.14");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number type because the string gets optimized to number
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesNumberToString_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: number 42 (compatible with Number)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: string "hello" (incompatible with Number)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because of type conflict
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesNumberToString_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // First write: number 42 (establishes Number type for empty list)
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second write: string "hello" (incompatible with Number)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because of type conflict
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToNumber_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.5);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because Number conflicts with Bool
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToNumber_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.5);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number type because empty list establishes new type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToString_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "hello");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown type because String conflicts with Bool
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToString_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "hello");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String type because empty list establishes new type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToStringNumberOptimization_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "3.14");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // String "3.14" gets optimized to Number, which conflicts with Bool
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleWritesBoolToStringNumberOptimization_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "3.14");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // String "3.14" gets optimized to Number type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, TypeChangeInIfBranch_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - changes type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Returns unknown type because both branches might run
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, TypeChangeInIfBranch_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - changes type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "test");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - different type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Returns unknown type because different types in branches
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, TypeChangeInElseBranch_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - changes type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Returns unknown type because both branches might run
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, TypeChangeInElseBranch_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - establishes Number type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - establishes String type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Returns unknown type because different types in branches
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, IfElseWithEqualTypes_SameType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, IfElseWithEqualTypes_SameType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - establishes Number type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - establishes same Number type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 1.25);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, IfElseWithEqualTypes_DifferentTypes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - changes type to string
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "Lorem");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - changes type to string
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "ipsum");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, IfElseWithEqualTypes_DifferentTypes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - establishes String type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "Lorem");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - establishes same String type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "ipsum");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatementAfterLoop_SameTypes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(loopStart);

    // First write - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(loopEnd);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Second write - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatementAfterLoop_SameTypes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(loopStart);

    // First write - establishes type for empty list
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(loopEnd);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Second write - same type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatementAfterLoop_DifferentTypes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(loopStart);

    // First write - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(loopEnd);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Second write - changes type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, IfStatementAfterLoop_DifferentTypes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(loopStart);

    // First write - establishes Number type for empty list
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(loopEnd);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Second write - adds string
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, TwoIfStatements_DifferentTypes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - does not change type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - does not change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 4);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Third write - adds string (in else branch)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, TwoIfStatements_DifferentTypes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // First write - establishes Number type
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Second write - same Number type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 4);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Third write - adds String (in else branch)
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_Before_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
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

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_Before_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
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

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_BeforeInner_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
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

    // Returns Unknown because a string might be added
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_BeforeInner_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
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

    // Returns Unknown because a string might be added
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_AfterWriteInsideInner_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Returns Unknown because a string might be added
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_AfterWriteInsideInner_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Returns Unknown because a string might be added
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_After_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Type is not known because the inner loop might not run at all
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithTypeChange_After_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    // Type is not known because the inner loop might not run at all
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithoutTypeChange_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.5);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopWithoutTypeChange_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.5);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopTypeChangesMultipleTimes_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // List write after inner loop
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, NestedLoopTypeChangesMultipleTimes_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop begin
    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, nullptr, false);
    instructionList.addInstruction(innerLoop);

    // List write inside inner loop
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // List write after inner loop
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleNestedLoopsWithTypeChange_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop1);

    // List write inside inner loop 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop2);

    // List write inside inner loop 2
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleNestedLoopsWithTypeChange_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop1);

    // List write inside inner loop 1
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 2.75);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop2);

    // List write inside inner loop 2
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

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleNestedLoopsWithMultipleTypeChanges_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop1);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop2);

    // List write inside inner loop 2
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // List write inside inner loop 1
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.75);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MultipleNestedLoopsWithMultipleTypeChanges_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // List write inside outer loop
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner loop 1 begin
    auto innerLoop1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop1);

    // Inner loop 2 begin
    auto innerLoop2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(innerLoop2);

    // List write inside inner loop 2
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "abc");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner loop 2 end
    auto innerEnd2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd2);

    // List write inside inner loop 1
    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 2.75);
    appendList3->workList = &list;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(appendList3);

    // Inner loop 1 end
    auto innerEnd1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(innerEnd1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, SameTypeIfElseInLoopWithTypeChange_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Outer loop begin
    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(outerLoop);

    // Inner if statement begin
    auto innerIf = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(innerIf);

    // List write inside inner if statement if branch
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "abc");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Inner if statement else branch
    auto innerElse = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(innerElse);

    // List write inside inner if statement else branch
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "def");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    // Inner if statement end
    auto innerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(innerEnd);

    // Outer loop end
    auto outerEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(outerEnd);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // A type conflict always occurs
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, InsertToList_SameType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.5);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(insertList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, InsertToList_DifferentType_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value(Compiler::StaticType::String, "hello");
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(insertList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, InsertToList_InLoop_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto insertList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 123);
    insertList1->workList = &list;
    insertList1->args.push_back({ Compiler::StaticType::Number, &index1 });
    insertList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(insertList1);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    auto insertList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    insertList2->workList = &list;
    insertList2->args.push_back({ Compiler::StaticType::Number, &index2 });
    insertList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ListReplace_SameType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value(Compiler::StaticType::String, "replaced");
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ListReplace_DifferentType_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ListReplace_WithStringOptimization)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value(Compiler::StaticType::String, "42.75");
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // String "42.75" gets optimized to Number type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, MixedWriteOperations_TypeConflict)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Append a number
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList);

    // Insert a string
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList);

    // Replace with a boolean
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should be Unknown due to multiple type conflicts
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, MixedWriteOperations_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Append a string
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "hello");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList);

    // Insert another string
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::String, "world");
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList);

    // Replace with another string
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should be String type since all operations use strings
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ComplexMixedOperations)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Append a number
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 123);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList);

    // Insert another number
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 456);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index1 });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Replace with a string (creates type conflict)
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value3(Compiler::StaticType::String, "conflict");
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(replaceList);

    // Should be Number type at the function call point, before the conflicting replace
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, true), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, ComplexMixedOperations_InLoop)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Append a number
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 123);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList);

    // Insert another number
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index1(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 456);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index1 });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Replace with a string (creates type conflict)
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value3(Compiler::StaticType::String, "conflict");
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(replaceList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, true), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearAndRebuild_WithDifferentTypes)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    // Initial append
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "initial");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Rebuild with different operations and types
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(insertList);

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, false);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    instructionList.addInstruction(replaceList);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should be Bool type since clear resets and all subsequent operations use booleans
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_NewTypeAfterClear_Empty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list in the middle of the loop
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Append after clear - this should set the new type
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because the list was empty and append sets the type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_NewTypeAfterClear_NonEmpty)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list in the middle of the loop
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Append after clear - this should set the new type
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because clear makes the list empty, then append sets the type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_MultipleAppendsAfterClear)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // First append after clear
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append after clear (same type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 3.14);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number because all appends after clear are the same type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_TypeConflictAfterClear)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // First append after clear
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second append after clear (different type - creates conflict)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown because of type conflict after clear
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_WithInsertAndReplace)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Insert after clear
    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, nullptr, false);
    LLVMConstantRegister insertIndex(Compiler::StaticType::Number, 0);
    LLVMConstantRegister insertValue(Compiler::StaticType::Bool, true);
    insertList->workList = &list;
    insertList->args.push_back({ Compiler::StaticType::Number, &insertIndex });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &insertValue });
    instructionList.addInstruction(insertList);

    // Replace after insert
    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, nullptr, false);
    LLVMConstantRegister replaceIndex(Compiler::StaticType::Number, 0);
    LLVMConstantRegister replaceValue(Compiler::StaticType::Bool, false);
    replaceList->workList = &list;
    replaceList->args.push_back({ Compiler::StaticType::Number, &replaceIndex });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &replaceValue });
    instructionList.addInstruction(replaceList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Bool because all operations after clear use Bool type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_ConditionalClear)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // If statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Clear the list conditionally
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // Append after clear
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "cleared");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Else branch
    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Append without clearing
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown due to type conflict between branches
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendAfterClearingInIfBranch_DifferentType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // If statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown because the clearing branch might or might not run
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendAfterClearingInIfBranch_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // If statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number because there are no type conflicts
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendAfterClearingInElseBranch_DifferentType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // If statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Else branch
    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::String, "test");
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown because the clearing branch might or might not run
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendAfterClearingInElseBranch_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // If statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Else branch
    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Clear the list
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    // End if statement
    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append
    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number because there are no type conflicts
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_MultipleClearsAndWrites)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // First clear
    auto clearList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList1->workList = &list;
    instructionList.addInstruction(clearList1);

    // Append after first clear
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "first");
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Second clear
    auto clearList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList2->workList = &list;
    instructionList.addInstruction(clearList2);

    // Append after second clear (different type)
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 42);
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Number because the last clear+append sequence determines the final type
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Number);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_ClearWithoutSubsequentWrite)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Clear the list but don't write anything after
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return previous type since no writes occurred after clear
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Bool, false), Compiler::StaticType::Bool);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_ClearInNestedIf)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    // Nested if statement
    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Clear and write in nested if
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->workList = &list;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    instructionList.addInstruction(appendList);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return Unknown because the type-changing branch might or might not run
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::String, false), Compiler::StaticType::Unknown);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendToEmptyListAndClearInIfBranch_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Append in if branch
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Clear the list in if branch
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append after if statement - should change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because the list remains empty after the if statement
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendToEmptyListAndClearInIfBranch_DifferentType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    // Append in if branch
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Clear the list in if branch
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append after if statement - should change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because the list remains empty after the if statement
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendToEmptyListAndClearInElseBranch_SameType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Append in else branch
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Clear the list in else branch
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append after if statement - should change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because the list remains empty after the if statement
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}

TEST(LLVMTypeAnalyzer_ListType, ClearListInLoop_AppendToEmptyListAndClearInElseBranch_DifferentType)
{
    LLVMTypeAnalyzer analyzer;
    LLVMInstructionList instructionList;
    List list("", "");

    auto start = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, nullptr, false);
    instructionList.addInstruction(start);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, nullptr, false);
    instructionList.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, nullptr, false);
    instructionList.addInstruction(elseStart);

    // Append in else branch
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value1(Compiler::StaticType::Bool, true);
    appendList1->workList = &list;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    instructionList.addInstruction(appendList1);

    // Clear the list in else branch
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, nullptr, false);
    clearList->workList = &list;
    instructionList.addInstruction(clearList);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, nullptr, false);
    instructionList.addInstruction(ifEnd);

    // Append after if statement - should change type
    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, nullptr, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->workList = &list;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    instructionList.addInstruction(appendList2);

    auto end = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, nullptr, false);
    instructionList.addInstruction(end);

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    instructionList.addInstruction(funcCall);

    // Should return String because the list remains empty after the if statement
    ASSERT_EQ(analyzer.listType(&list, funcCall.get(), Compiler::StaticType::Number, true), Compiler::StaticType::String);
}
