#include <scratchcpp/thread.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMInstructionListTest, EmptyList_First)
{
    LLVMInstructionList list;
    ASSERT_EQ(list.first(), nullptr);
}

TEST(LLVMInstructionListTest, EmptyList_Last)
{
    LLVMInstructionList list;
    ASSERT_EQ(list.last(), nullptr);
}

TEST(LLVMInstructionListTest, AddSingleInstruction_First)
{
    LLVMInstructionList list;
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins);

    ASSERT_EQ(list.first(), ins.get());
}

TEST(LLVMInstructionListTest, AddSingleInstruction_Last)
{
    LLVMInstructionList list;
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins);

    ASSERT_EQ(list.last(), ins.get());
}

TEST(LLVMInstructionListTest, AddSingleInstruction_Previous)
{
    LLVMInstructionList list;
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins);

    ASSERT_EQ(ins->previous, nullptr);
}

TEST(LLVMInstructionListTest, AddSingleInstruction_Next)
{
    LLVMInstructionList list;
    auto ins = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins);

    ASSERT_EQ(ins->next, nullptr);
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_First)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(list.first(), ins1.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_Last)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(list.last(), ins3.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_PreviousOfFirst)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins1->previous, nullptr);
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_NextOfFirst)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins1->next, ins2.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_PreviousOfMiddle)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins2->previous, ins1.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_NextOfMiddle)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins2->next, ins3.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_PreviousOfLast)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins3->previous, ins2.get());
}

TEST(LLVMInstructionListTest, AddMultipleInstructions_NextOfLast)
{
    LLVMInstructionList list;

    auto ins1 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, nullptr, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins3->next, nullptr);
}
