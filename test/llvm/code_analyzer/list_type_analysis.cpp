#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmcodeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, FirstListWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 42.0);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    analyzer.analyzeScript(list);

    // First write should have Unknown targetType (no previous type)
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, SecondListWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    // First write has no previous type
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Unknown);

    // Second write has Unknown type because the list wasn't cleared
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, MultipleWritesSameType_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "first");
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "second");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::String, "third");
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::String);
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, MultipleWritesDifferentTypes_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Number);
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, StringOptimization_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "3.14");
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);

    // String "3.14" optimized to Number, so second write sees Number type
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ClearListOperation)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "hello");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Unknown);

    // After clear, list is empty so new type can be established
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Void);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ProcedureCall)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "hello");
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto procCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::CallProcedure, false);
    list.addInstruction(procCall);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::Number, 5.2);
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);

    // Type unknown due to procedure call
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, MixedWriteOperationsSameType_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "hello");
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList);

    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::String, "world");
    insertList->targetList = &targetList;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(insertList);

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value3(Compiler::StaticType::String, "test");
    replaceList->targetList = &targetList;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(replaceList);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(insertList->targetType, Compiler::StaticType::String);
    ASSERT_EQ(replaceList->targetType, Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, MixedWriteOperationsDifferentTypes_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::String, "hello");
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList);

    auto insertList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::InsertToList, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, false);
    insertList->targetList = &targetList;
    insertList->args.push_back({ Compiler::StaticType::Number, &index });
    insertList->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(insertList);

    auto replaceList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ListReplace, false);
    LLVMConstantRegister index2(Compiler::StaticType::Number, 1);
    LLVMConstantRegister value3(Compiler::StaticType::Number, 5.2);
    replaceList->targetList = &targetList;
    replaceList->args.push_back({ Compiler::StaticType::Number, &index2 });
    replaceList->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(replaceList);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "world");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(insertList->targetType, Compiler::StaticType::String);
    ASSERT_EQ(replaceList->targetType, Compiler::StaticType::Bool | Compiler::StaticType::String);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, LoopSingleWrite)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // Loop convergence: first iteration Unknown, subsequent iterations Number
    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, LoopSingleWrite_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Number, 1.25);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ClearAfterWriteInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    // Establish bool type before loop
    auto clearList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList1->targetList = &targetList;
    list.addInstruction(clearList1);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, false);
    list.addInstruction(loopStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto clearList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList2->targetList = &targetList;
    list.addInstruction(clearList2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, WhileLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    // Establish bool type before loop
    auto clearList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList1->targetList = &targetList;
    list.addInstruction(clearList1);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginWhileLoop, false);
    list.addInstruction(loopStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto clearList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList2->targetList = &targetList;
    list.addInstruction(clearList2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, RepeatUntilLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    // Establish bool type before loop
    auto clearList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList1->targetList = &targetList;
    list.addInstruction(clearList1);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, true);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatUntilLoop, false);
    list.addInstruction(loopStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto clearList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList2->targetList = &targetList;
    list.addInstruction(clearList2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Bool);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, LoopMultipleWrites_UnknownType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // First write: Unknown (unknown before loop)
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Unknown);

    // Second write: still Unknown
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, LoopMultipleWrites_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    // Establish bool type before loop
    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 5);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    analyzer.analyzeScript(list);

    // Number | String (from loop iterations)
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, IfElseStatementDifferentTypes)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    // Both writes see Unknown before the if-else
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);

    // The type is still Unknown even after the if statement
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, IfElseStatementDifferentTypes_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    // Both writes see Void (empty list) before the if-else
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Void);

    // The type is Number or String after the if statement
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, IfElseStatementDifferentType_IfBranch_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Number);

    // The type is Number or String after the if statement
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, IfElseStatementDifferentType_ElseBranch_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Number);

    // The type is Number or String after the if statement
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, WriteBeforeLoop_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendListBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    appendListBefore->targetList = &targetList;
    appendListBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(appendListBefore);

    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    auto appendListInLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueInLoop(Compiler::StaticType::Number, 42);
    appendListInLoop->targetList = &targetList;
    appendListInLoop->args.push_back({ Compiler::StaticType::Unknown, &valueInLoop });
    list.addInstruction(appendListInLoop);

    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, WriteBeforeIfStatement_IfBranch_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendListBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    appendListBefore->targetList = &targetList;
    appendListBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(appendListBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto appendListInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    appendListInIfStatement->targetList = &targetList;
    appendListInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(appendListInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, WriteBeforeIfStatement_ElseBranch_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendListBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::String, "initial");
    appendListBefore->targetList = &targetList;
    appendListBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(appendListBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendListInIfStatement = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueInIfStatement(Compiler::StaticType::Number, 42);
    appendListInIfStatement->targetList = &targetList;
    appendListInIfStatement->args.push_back({ Compiler::StaticType::Unknown, &valueInIfStatement });
    list.addInstruction(appendListInIfStatement);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value(Compiler::StaticType::Bool, false);
    appendList->targetList = &targetList;
    appendList->args.push_back({ Compiler::StaticType::Unknown, &value });
    list.addInstruction(appendList);

    analyzer.analyzeScript(list);

    ASSERT_EQ(appendList->targetType, Compiler::StaticType::String | Compiler::StaticType::Number);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, WriteBeforeIfElse_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto appendListBefore = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueBefore(Compiler::StaticType::Bool, true);
    appendListBefore->targetList = &targetList;
    appendListBefore->args.push_back({ Compiler::StaticType::Unknown, &valueBefore });
    list.addInstruction(appendListBefore);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    analyzer.analyzeScript(list);

    // Write before if-else establishes Bool type
    ASSERT_EQ(appendListBefore->targetType, Compiler::StaticType::Void);

    // Both writes in branches see Bool type from before
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Bool);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Bool);

    // The type is Unknown after the if statement because the first type is used as well
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ComplexNestedControlFlow_AfterClear)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList("", "");

    auto clearList = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList->targetList = &targetList;
    list.addInstruction(clearList);

    auto outerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(outerLoop);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 1);
    appendList1->targetList = &targetList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto ifStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginIf, false);
    list.addInstruction(ifStart);

    auto innerLoop = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(innerLoop);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "nested");
    appendList2->targetList = &targetList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto innerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(innerLoopEnd);

    auto elseStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginElse, false);
    list.addInstruction(elseStart);

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value3(Compiler::StaticType::Bool, true);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &value3 });
    list.addInstruction(appendList3);

    auto ifEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndIf, false);
    list.addInstruction(ifEnd);

    auto outerLoopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(outerLoopEnd);

    auto appendList4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value4(Compiler::StaticType::Bool, true);
    appendList4->targetList = &targetList;
    appendList4->args.push_back({ Compiler::StaticType::Unknown, &value4 });
    list.addInstruction(appendList4);

    analyzer.analyzeScript(list);

    // Complex analysis with multiple execution paths and loop convergence
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Unknown);
    ASSERT_EQ(appendList4->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, MultipleListsSeparateAnalysis)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List targetList1("", ""), targetList2("", "");

    auto clearList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearList1->targetList = &targetList1;
    list.addInstruction(clearList1);

    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &targetList1;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList2->targetList = &targetList2;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    analyzer.analyzeScript(list);

    // Both are first writes for their respective lists
    ASSERT_EQ(appendList1->targetType, Compiler::StaticType::Void);
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, CrossListDependency_SingleType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List sourceList("", ""), targetList("", "");

    auto clearSource = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearSource->targetList = &sourceList;
    list.addInstruction(clearSource);

    auto clearTarget = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearTarget->targetList = &targetList;
    list.addInstruction(clearTarget);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    appendList2->targetList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList2);

    auto getItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getItem->targetList = &sourceList;
    getItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getItem);

    LLVMRegister sourceValue(Compiler::StaticType::Unknown);
    sourceValue.isRawValue = false;
    sourceValue.instruction = getItem;
    getItem->functionReturnReg = &sourceValue;

    auto appendList1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList1_1->targetList = &targetList;
    appendList1_1->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList1_1);

    auto appendList1_2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::String, "test");
    appendList1_2->targetList = &targetList;
    appendList1_2->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList1_2);

    analyzer.analyzeScript(list);

    // sourceList first write has no previous type
    ASSERT_EQ(appendList2->targetType, Compiler::StaticType::Void);

    // sourceList has Number type at read operation
    ASSERT_EQ(getItem->targetType, Compiler::StaticType::Number);

    // targetList first write has no previous type
    ASSERT_EQ(appendList1_1->targetType, Compiler::StaticType::Void);

    // targetList second write has Number | String type (get list item can always return string)
    ASSERT_EQ(appendList1_2->targetType, Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, CrossListDependency_MultipleTypes)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List sourceList("", ""), targetList("", "");

    auto clearSource = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearSource->targetList = &sourceList;
    list.addInstruction(clearSource);

    auto clearTarget = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearTarget->targetList = &targetList;
    list.addInstruction(clearTarget);

    // Establish Number | Bool types
    auto appendList1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 42);
    appendList1->targetList = &sourceList;
    appendList1->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList1);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value2(Compiler::StaticType::Bool, true);
    appendList2->targetList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value2 });
    list.addInstruction(appendList2);

    auto getItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getItem->targetList = &sourceList;
    getItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getItem);

    LLVMRegister sourceValue(Compiler::StaticType::Unknown);
    sourceValue.isRawValue = false;
    sourceValue.instruction = getItem;
    getItem->functionReturnReg = &sourceValue;

    auto appendList3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList3->targetList = &targetList;
    appendList3->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList3);

    auto appendList4 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value4(Compiler::StaticType::String, "test");
    appendList4->targetList = &targetList;
    appendList4->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList4);

    analyzer.analyzeScript(list);

    // sourceList has Number or Bool type at read operation
    ASSERT_EQ(getItem->targetType, Compiler::StaticType::Number | Compiler::StaticType::Bool);

    // targetList first write has no previous type
    ASSERT_EQ(appendList3->targetType, Compiler::StaticType::Void);

    // targetList second write has Number or Bool type + String (get list item can always return string)
    ASSERT_EQ(appendList4->targetType, Compiler::StaticType::Unknown);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ChainedAssignmentsInLoop)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List listA("a", ""), listB("b", ""), listC("c", "");

    auto clearA = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearA->targetList = &listA;
    list.addInstruction(clearA);

    auto clearB = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearB->targetList = &listB;
    list.addInstruction(clearB);

    auto clearC = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearC->targetList = &listC;
    list.addInstruction(clearC);

    // a += 5
    auto appendA1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueA1(Compiler::StaticType::Number, 5);
    appendA1->targetList = &listA;
    appendA1->args.push_back({ Compiler::StaticType::Unknown, &valueA1 });
    list.addInstruction(appendA1);

    // b += 3
    auto appendB1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueB1(Compiler::StaticType::Number, 3);
    appendB1->targetList = &listB;
    appendB1->args.push_back({ Compiler::StaticType::Unknown, &valueB1 });
    list.addInstruction(appendB1);

    // c += 8
    auto appendC1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueC1(Compiler::StaticType::Number, 8);
    appendC1->targetList = &listC;
    appendC1->args.push_back({ Compiler::StaticType::Unknown, &valueC1 });
    list.addInstruction(appendC1);

    // loop start
    auto loopStart = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::BeginRepeatLoop, false);
    list.addInstruction(loopStart);

    // clear a
    clearA = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearA->targetList = &listA;
    list.addInstruction(clearA);

    // a += b[0]
    auto getB = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister indexB(Compiler::StaticType::Number, 0);
    getB->targetList = &listB;
    getB->args.push_back({ Compiler::StaticType::Number, &indexB });
    list.addInstruction(getB);

    LLVMRegister bValue(Compiler::StaticType::Unknown);
    bValue.isRawValue = false;
    bValue.instruction = getB;
    getB->functionReturnReg = &bValue;

    auto appendA2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendA2->targetList = &listA;
    appendA2->args.push_back({ Compiler::StaticType::Unknown, &bValue });
    list.addInstruction(appendA2);

    // clear b
    clearB = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearB->targetList = &listB;
    list.addInstruction(clearB);

    // b += c[0]
    auto getC1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister indexC1(Compiler::StaticType::Number, 0);
    getC1->targetList = &listC;
    getC1->args.push_back({ Compiler::StaticType::Number, &indexC1 });
    list.addInstruction(getC1);

    LLVMRegister cValue1(Compiler::StaticType::Unknown);
    cValue1.isRawValue = false;
    cValue1.instruction = getC1;
    getC1->functionReturnReg = &cValue1;

    auto appendB2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendB2->targetList = &listB;
    appendB2->args.push_back({ Compiler::StaticType::Unknown, &cValue1 });
    list.addInstruction(appendB2);

    // clear c
    clearC = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearC->targetList = &listC;
    list.addInstruction(clearC);

    // c += "test"
    auto appendC2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister valueC2(Compiler::StaticType::String, "test");
    appendC2->targetList = &listC;
    appendC2->args.push_back({ Compiler::StaticType::Unknown, &valueC2 });
    list.addInstruction(appendC2);

    // loop end
    auto loopEnd = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::EndLoop, false);
    list.addInstruction(loopEnd);

    // clear a (final assignment we want to test)
    auto clearAFinal = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearAFinal->targetList = &listA;
    list.addInstruction(clearAFinal);

    // a += c[0]
    auto getC2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister indexC2(Compiler::StaticType::Number, 0);
    getC2->targetList = &listC;
    getC2->args.push_back({ Compiler::StaticType::Number, &indexC2 });
    list.addInstruction(getC2);

    LLVMRegister cValue2(Compiler::StaticType::Unknown);
    cValue2.isRawValue = false;
    cValue2.instruction = getC2;
    getC2->functionReturnReg = &cValue2;

    auto appendA3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendA3->targetList = &listA;
    appendA3->args.push_back({ Compiler::StaticType::Unknown, &cValue2 });
    list.addInstruction(appendA3);

    analyzer.analyzeScript(list);

    // Check the type of 'a' before the final clear operation
    // 'a' could be Number (from initial assignment) or String (from loop iterations)
    auto expectedAType = Compiler::StaticType::Number | Compiler::StaticType::String;
    ASSERT_EQ(clearAFinal->targetType, expectedAType);

    // Check the type of 'c' before the final read
    // 'c' could be Number (from initial assignment) or String (from loop assignment)
    auto expectedCType = Compiler::StaticType::Number | Compiler::StaticType::String;
    ASSERT_EQ(getC2->targetType, expectedCType);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, ListReadReturnRegType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List sourceList("", ""), targetList("", "");

    auto clearSource = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearSource->targetList = &sourceList;
    list.addInstruction(clearSource);

    auto clearTarget = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearTarget->targetList = &targetList;
    list.addInstruction(clearTarget);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    appendList2->targetList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList2);

    auto getItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getItem->targetList = &sourceList;
    getItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getItem);

    LLVMRegister sourceValue(Compiler::StaticType::Unknown);
    sourceValue.isRawValue = false;
    sourceValue.instruction = getItem;
    getItem->functionReturnReg = &sourceValue;

    auto appendList1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList1_1->targetList = &targetList;
    appendList1_1->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList1_1);

    analyzer.analyzeScript(list);

    // sourceList read return register has Number | String type (get list item can always return string)
    ASSERT_EQ(sourceValue.type(), Compiler::StaticType::Number | Compiler::StaticType::String);
}

TEST(LLVMCodeAnalyzer_ListTypeAnalysis, CrossListWriteArgType)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;
    List sourceList("", ""), targetList("", "");

    auto clearSource = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearSource->targetList = &sourceList;
    list.addInstruction(clearSource);

    auto clearTarget = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::ClearList, false);
    clearTarget->targetList = &targetList;
    list.addInstruction(clearTarget);

    auto appendList2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    LLVMConstantRegister value1(Compiler::StaticType::Number, 3.14);
    appendList2->targetList = &sourceList;
    appendList2->args.push_back({ Compiler::StaticType::Unknown, &value1 });
    list.addInstruction(appendList2);

    auto getItem = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::GetListItem, false);
    LLVMConstantRegister index(Compiler::StaticType::Number, 0);
    getItem->targetList = &sourceList;
    getItem->args.push_back({ Compiler::StaticType::Number, &index });
    list.addInstruction(getItem);

    LLVMRegister sourceValue(Compiler::StaticType::Unknown);
    sourceValue.isRawValue = false;
    sourceValue.instruction = getItem;
    getItem->functionReturnReg = &sourceValue;

    auto appendList1_1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::AppendToList, false);
    appendList1_1->targetList = &targetList;
    appendList1_1->args.push_back({ Compiler::StaticType::Unknown, &sourceValue });
    list.addInstruction(appendList1_1);

    analyzer.analyzeScript(list);

    // last write argument has Number | String type (get list item can always return string)
    ASSERT_EQ(appendList1_1->args.back().first, Compiler::StaticType::Number | Compiler::StaticType::String);
}
