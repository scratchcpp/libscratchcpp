#include <scratchcpp/value.h>
#include <iostream>
#include <targetmock.h>

#include "testfunctions.h"
#include "testmock.h"

using namespace libscratchcpp;

static int counter = 0;

extern "C"
{
    void test_function(TestMock *mock, Target *target)
    {
        if (mock)
            mock->f(target);
    }

    void test_print_function(ValueData *arg1, ValueData *arg2)
    {
        std::string s1, s2;
        value_toString(arg1, &s1);
        value_toString(arg2, &s2);
        std::cout << s1 << " " << s2 << std::endl;
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

    bool test_equals(Target *target, const char *a, const char *b)
    {
        return strcmp(a, b) == 0;
    }

    void test_unreachable(Target *target)
    {
        std::cout << "error: unreachable reached" << std::endl;
        exit(1);
    }

    bool test_lower_than(Target *target, double a, double b)
    {
        return a < b;
    }

    double test_const_number(Target *target, double v)
    {
        return v;
    }

    bool test_const_bool(Target *target, bool v)
    {
        return v;
    }

    char *test_const_string(Target *target, const char *v)
    {
        Value value(v);
        return value_toCString(&value.data());
    }

    bool test_not(Target *target, bool arg)
    {
        return !arg;
    }

    void test_reset_counter(Target *target)
    {
        counter = 0;
    }

    void test_increment_counter(Target *target)
    {
        counter++;
    }

    double test_get_counter(Target *target)
    {
        return counter;
    }

    void test_print_number(Target *target, double v)
    {
        std::cout << v << std::endl;
    }

    void test_print_bool(Target *target, bool v)
    {
        std::cout << v << std::endl;
    }

    void test_print_string(Target *target, const char *v)
    {
        std::cout << v << std::endl;
    }
}
