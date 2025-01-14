#include <scratchcpp/compilerlocalvariable.h>
#include <scratchcpp/compilervalue.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(CompilerLocalVariableTest, Constructors)
{
    CompilerValue ptr(Compiler::StaticType::Number);
    CompilerLocalVariable var(&ptr);
    ASSERT_EQ(var.ptr(), &ptr);
}

TEST(CompilerLocalVariableTest, Type)
{
    {
        CompilerValue ptr(Compiler::StaticType::Number);
        CompilerLocalVariable var(&ptr);
        ASSERT_EQ(var.type(), ptr.type());
    }

    {
        CompilerValue ptr(Compiler::StaticType::Bool);
        CompilerLocalVariable var(&ptr);
        ASSERT_EQ(var.type(), ptr.type());
    }

    {
        CompilerValue ptr(Compiler::StaticType::String);
        CompilerLocalVariable var(&ptr);
        ASSERT_EQ(var.type(), ptr.type());
    }
}
