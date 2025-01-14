#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/value.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(CompilerConstantTest, Constructors)
{
    {
        CompilerConstant v(Compiler::StaticType::Number, 5);
        ASSERT_EQ(v.type(), Compiler::StaticType::Number);
        ASSERT_EQ(v.value(), 5);
    }

    {
        CompilerConstant v(Compiler::StaticType::String, "test");
        ASSERT_EQ(v.type(), Compiler::StaticType::String);
        ASSERT_EQ(v.value(), "test");
    }

    {
        CompilerConstant v(Compiler::StaticType::Unknown, true);
        ASSERT_EQ(v.type(), Compiler::StaticType::Unknown);
        ASSERT_EQ(v.value(), true);
    }
}

TEST(CompilerConstantTest, IsConst)
{
    CompilerConstant v(Compiler::StaticType::Unknown, Value());
    ASSERT_TRUE(v.isConst());
}
