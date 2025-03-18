#include <scratchcpp/value.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_pool.h>
#include <utf8.h>
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
            mock->script(ctx, target, varData, listData);
    }

    ValueData test_reporter(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData)
    {
        if (mock)
            return mock->reporter(ctx, target, varData, listData);

        return ValueData();
    }

    bool test_predicate(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData)
    {
        if (mock)
            return mock->predicate(ctx, target, varData, listData);

        return false;
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

    void test_ctx_function(ExecutionContext *ctx, const StringPtr *arg)
    {
        std::cout << ctx << std::endl;
        std::cout << utf8::utf16to8(std::u16string(arg->data)) << std::endl;
    }

    void test_function_no_args(Target *target)
    {
        target->isStage();
        std::cout << "no_args" << std::endl;
    }

    StringPtr *test_function_no_args_ret(Target *target)
    {
        target->isStage();
        std::cout << "no_args_ret" << std::endl;
        Value v("no_args_output");
        return value_toStringPtr(&v.data());
    }

    void test_function_1_arg(Target *target, const StringPtr *arg1)
    {
        target->isStage();
        std::cout << "1_arg " << utf8::utf16to8(std::u16string(arg1->data)) << std::endl;
    }

    StringPtr *test_function_1_arg_ret(Target *target, const StringPtr *arg1)
    {
        target->isStage();
        std::cout << "1_arg_ret " << utf8::utf16to8(std::u16string(arg1->data)) << std::endl;
        Value v("1_arg_output");
        return value_toStringPtr(&v.data());
    }

    void test_function_3_args(Target *target, const StringPtr *arg1, const StringPtr *arg2, const StringPtr *arg3)
    {
        target->isStage();
        std::cout << "3_args " << utf8::utf16to8(std::u16string(arg1->data)) << " " << utf8::utf16to8(std::u16string(arg2->data)) << " " << utf8::utf16to8(std::u16string(arg3->data)) << std::endl;
    }

    StringPtr *test_function_3_args_ret(Target *target, const StringPtr *arg1, const StringPtr *arg2, const StringPtr *arg3)
    {
        target->isStage();
        std::cout << "3_args " << utf8::utf16to8(std::u16string(arg1->data)) << " " << utf8::utf16to8(std::u16string(arg2->data)) << " " << utf8::utf16to8(std::u16string(arg3->data)) << std::endl;
        Value v("3_args_output");
        return value_toStringPtr(&v.data());
    }

    const void *test_function_1_ptr_arg_ret(Target *target, const int *arg1)
    {
        target->isStage();
        std::cout << "1_arg_ret " << *arg1 << std::endl;
        return target;
    }

    bool test_equals(const StringPtr *a, const StringPtr *b)
    {
        return string_compare_case_sensitive(a, b) == 0;
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

    StringPtr *test_const_string(const StringPtr *v)
    {
        StringPtr *ret = string_pool_new();
        string_assign(ret, v);
        return ret;
    }

    const void *test_const_pointer(const void *v)
    {
        return v;
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

    void test_print_string(const StringPtr *v)
    {
        std::cout << utf8::utf16to8(std::u16string(v->data)) << std::endl;
    }

    void test_print_pointer(const void *v)
    {
        std::cout << v << std::endl;
    }

    void test_print_unknown(const ValueData *v)
    {
        std::string str;
        value_toString(v, &str);
        std::cout << str << std::endl;
    }
}
