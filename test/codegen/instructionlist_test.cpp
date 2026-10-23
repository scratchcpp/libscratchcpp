#include <scratchcpp/thread.h>
#include <engine/internal/codegen/instructionlist.h>
#include <engine/internal/codegen/instruction.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

struct TInstruction : public Instruction<TInstruction, CompilerValue>
{
        using Type = Instruction<TInstruction, CompilerValue>::Type;
        using Instruction<TInstruction, CompilerValue>::Instruction; // inherit constructor
};

TEST(InstructionListTest, EmptyList_First)
{
    InstructionList<TInstruction> list;
    ASSERT_EQ(list.first(), nullptr);
}

TEST(InstructionListTest, EmptyList_Last)
{
    InstructionList<TInstruction> list;
    ASSERT_EQ(list.last(), nullptr);
}

TEST(InstructionListTest, EmptyList_Empty)
{
    InstructionList<TInstruction> list;
    ASSERT_TRUE(list.empty());
}

TEST(InstructionListTest, EmptyList_ContainsNull)
{
    InstructionList<TInstruction> list;
    ASSERT_FALSE(list.containsInstruction(nullptr));
}

TEST(InstructionListTest, EmptyList_ContainsFunc)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    ASSERT_FALSE(list.containsInstruction([](const TInstruction *) { return true; }));
}

TEST(InstructionListTest, EmptyList_Contains)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    ASSERT_FALSE(list.containsInstruction(ins.get()));
}

TEST(InstructionListTest, SingleInstructionList_ContainsExistent)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    ASSERT_TRUE(list.containsInstruction(ins1.get()));
}

TEST(InstructionListTest, SingleInstructionList_ContainsNonExistent)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    ASSERT_FALSE(list.containsInstruction(ins.get()));
}

TEST(InstructionListTest, SingleInstructionList_ContainsNull)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    ASSERT_FALSE(list.containsInstruction(nullptr));
}

TEST(InstructionListTest, SingleInstructionList_ContainsFuncTrue)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    ASSERT_TRUE(list.containsInstruction([](const TInstruction *) { return true; }));
}

TEST(InstructionListTest, SingleInstructionList_ContainsFuncFalse)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    ASSERT_FALSE(list.containsInstruction([](const TInstruction *) { return false; }));
}

TEST(InstructionListTest, MultipleInstructionList_ContainsExistent)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_TRUE(list.containsInstruction(ins2.get()));
}

TEST(InstructionListTest, MultipleInstructionList_ContainsNonExistent)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    ASSERT_FALSE(list.containsInstruction(ins.get()));
}

TEST(InstructionListTest, MultipleInstructionList_ContainsNull)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_FALSE(list.containsInstruction(nullptr));
}

TEST(InstructionListTest, MultipleInstructionList_ContainsExistentFunc)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::Stop, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_TRUE(list.containsInstruction([](const TInstruction *ins) { return ins->type == TInstruction::Type::Stop; }));
}

TEST(InstructionListTest, MultipleInstructionList_ContainsNonExistentFunc)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_FALSE(list.containsInstruction([](const TInstruction *ins) { return ins->type == TInstruction::Type::Stop; }));
}

TEST(InstructionListTest, AddSingleInstruction_First)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins);

    ASSERT_EQ(list.first(), ins.get());
}

TEST(InstructionListTest, AddSingleInstruction_Last)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins);

    ASSERT_EQ(list.last(), ins.get());
}

TEST(InstructionListTest, AddSingleInstruction_Empty)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins);

    ASSERT_FALSE(list.empty());
}

TEST(InstructionListTest, AddSingleInstruction_Previous)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins);

    ASSERT_EQ(ins->previous, nullptr);
}

TEST(InstructionListTest, AddSingleInstruction_Next)
{
    InstructionList<TInstruction> list;
    auto ins = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins);

    ASSERT_EQ(ins->next, nullptr);
}

TEST(InstructionListTest, AddMultipleInstructions_First)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(list.first(), ins1.get());
}

TEST(InstructionListTest, AddMultipleInstructions_Last)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(list.last(), ins3.get());
}

TEST(InstructionListTest, AddMultipleInstructions_Empty)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_FALSE(list.empty());
}

TEST(InstructionListTest, AddMultipleInstructions_PreviousOfFirst)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins1->previous, nullptr);
}

TEST(InstructionListTest, AddMultipleInstructions_NextOfFirst)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins1->next, ins2.get());
}

TEST(InstructionListTest, AddMultipleInstructions_PreviousOfMiddle)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins2->previous, ins1.get());
}

TEST(InstructionListTest, AddMultipleInstructions_NextOfMiddle)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins2->next, ins3.get());
}

TEST(InstructionListTest, AddMultipleInstructions_PreviousOfLast)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins3->previous, ins2.get());
}

TEST(InstructionListTest, AddMultipleInstructions_NextOfLast)
{
    InstructionList<TInstruction> list;

    auto ins1 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins1);

    auto ins2 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins2);

    auto ins3 = std::make_shared<TInstruction>(TInstruction::Type::FunctionCall, false);
    list.addInstruction(ins3);

    ASSERT_EQ(ins3->next, nullptr);
}
