#include <scratchcpp/compilervalue.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(CompilerValueTest, Constructors)
{
    {
        CompilerValue v(Compiler::StaticType::Number);
        ASSERT_EQ(v.type(), Compiler::StaticType::Number);
    }

    {
        CompilerValue v(Compiler::StaticType::String);
        ASSERT_EQ(v.type(), Compiler::StaticType::String);
    }

    {
        CompilerValue v(Compiler::StaticType::Unknown);
        ASSERT_EQ(v.type(), Compiler::StaticType::Unknown);
    }
}

TEST(CompilerValueTest, Type)
{
    CompilerValue v(Compiler::StaticType::Unknown);

    v.setType(Compiler::StaticType::String);
    ASSERT_EQ(v.type(), Compiler::StaticType::String);

    v.setType(Compiler::StaticType::Bool);
    ASSERT_EQ(v.type(), Compiler::StaticType::Bool);

    v.setType(Compiler::StaticType::Unknown);
    ASSERT_EQ(v.type(), Compiler::StaticType::Unknown);
}

TEST(CompilerValueTest, IsConst)
{
    CompilerValue v(Compiler::StaticType::Unknown);
    ASSERT_FALSE(v.isConst());
}
