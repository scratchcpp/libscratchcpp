#include <scratchcpp/value_functions.h>
#include <iostream>
#include <targetmock.h>

#include "testfunctions.h"
#include "testmock.h"

using namespace libscratchcpp;

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

    void test_function_no_args_ret(Target *target, ValueData *ret)
    {
        target->isStage();
        std::cout << "no_args_ret" << std::endl;
        value_assign_cstring(ret, "no_args_output");
    }

    void test_function_1_arg(Target *target, const ValueData *arg1)
    {
        target->isStage();
        std::string s;
        value_toString(arg1, &s);
        std::cout << "1_arg " << s << std::endl;
    }

    void test_function_1_arg_ret(Target *target, ValueData *ret, const ValueData *arg1)
    {
        target->isStage();
        std::string s;
        value_toString(arg1, &s);
        std::cout << "1_arg_ret " << s << std::endl;
        value_assign_cstring(ret, "1_arg_output");
    }

    void test_function_3_args(Target *target, const ValueData *arg1, const ValueData *arg2, const ValueData *arg3)
    {
        target->isStage();
        std::string s1, s2, s3;
        value_toString(arg1, &s1);
        value_toString(arg2, &s2);
        value_toString(arg3, &s3);
        std::cout << "3_args " << s1 << " " << s2 << " " << s3 << std::endl;
    }

    void test_function_3_args_ret(Target *target, ValueData *ret, const ValueData *arg1, const ValueData *arg2, const ValueData *arg3)
    {
        target->isStage();
        std::string s1, s2, s3;
        value_toString(arg1, &s1);
        value_toString(arg2, &s2);
        value_toString(arg3, &s3);
        std::cout << "3_args " << s1 << " " << s2 << " " << s3 << std::endl;
        value_assign_cstring(ret, "3_args_output");
    }

    void test_equals(Target *target, ValueData *ret, ValueData *a, ValueData *b)
    {
        value_assign_bool(ret, value_equals(a, b));
    }
}