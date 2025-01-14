#include <scratchcpp/value.h>
#include <iostream>
#include <targetmock.h>

#include "testfunctions.h"
#include "testmock.h"

using namespace libscratchcpp;

static int counter = 0;

extern "C"
{
    void test_function(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData)
    {
        if (mock)
            mock->f(ctx, target, varData, listData);
    }

    void test_print_function(ValueData *arg1, ValueData *arg2)
    {
        std::string s1, s2;
        value_toString(arg1, &s1);
        value_toString(arg2, &s2);
        std::cout << s1 << " " << s2 << std::endl;
    }

    void test_empty_function()
    {
        std::cout << "empty" << std::endl;
    }

    void test_ctx_function(ExecutionContext *ctx, const char *arg)
    {
        std::cout << ctx << std::endl;
        std::cout << arg << std::endl;
    }

    void test_function_no_args(Target *target)
    {
        target->isStage();
        std::cout << "no_args" << std::endl;
    }

    char *test_function_no_args_ret(Target *target)
    {
        target->isStage();
        std::cout << "no_args_ret" << std::endl;
        Value v("no_args_output");
        return value_toCString(&v.data());
    }

    void test_function_1_arg(Target *target, const char *arg1)
    {
        target->isStage();
        std::cout << "1_arg " << arg1 << std::endl;
    }

    char *test_function_1_arg_ret(Target *target, const char *arg1)
    {
        target->isStage();
        std::cout << "1_arg_ret " << arg1 << std::endl;
        Value v("1_arg_output");
        return value_toCString(&v.data());
    }

    void test_function_3_args(Target *target, const char *arg1, const char *arg2, const char *arg3)
    {
        target->isStage();
        std::cout << "3_args " << arg1 << " " << arg2 << " " << arg3 << std::endl;
    }

    char *test_function_3_args_ret(Target *target, const char *arg1, const char *arg2, const char *arg3)
    {
        target->isStage();
        std::cout << "3_args " << arg1 << " " << arg2 << " " << arg3 << std::endl;
        Value v("3_args_output");
        return value_toCString(&v.data());
    }

    bool test_equals(const char *a, const char *b)
    {
        return strcmp(a, b) == 0;
    }

    void test_unreachable()
    {
        std::cout << "error: unreachable reached" << std::endl;
        exit(1);
    }

    bool test_lower_than(double a, double b)
    {
        return a < b;
    }

    double test_const_number(double v)
    {
        return v;
    }

    bool test_const_bool(bool v)
    {
        return v;
    }

    char *test_const_string(const char *v)
    {
        Value value(v);
        return value_toCString(&value.data());
    }

    bool test_not(bool arg)
    {
        return !arg;
    }

    void test_reset_counter()
    {
        counter = 0;
    }

    void test_increment_counter()
    {
        counter++;
    }

    double test_get_counter()
    {
        return counter;
    }

    void test_print_number(double v)
    {
        std::cout << v << std::endl;
    }

    void test_print_bool(bool v)
    {
        std::cout << v << std::endl;
    }

    void test_print_string(const char *v)
    {
        std::cout << v << std::endl;
    }
}
