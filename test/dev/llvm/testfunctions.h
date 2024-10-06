#pragma once

namespace libscratchcpp
{

class TestMock;
class Target;
class ValueData;

extern "C"
{
    void test_function(TestMock *mock, Target *target);
    void test_print_function(ValueData *arg1, ValueData *arg2);

    void test_function_no_args(Target *target);
    void test_function_no_args_ret(Target *target, ValueData *ret);
    void test_function_1_arg(Target *target, const ValueData *arg1);
    void test_function_1_arg_ret(Target *target, ValueData *ret, const ValueData *arg1);
    void test_function_3_args(Target *target, const ValueData *arg1, const ValueData *arg2, const ValueData *arg3);
    void test_function_3_args_ret(Target *target, ValueData *ret, const ValueData *arg1, const ValueData *arg2, const ValueData *arg3);

    void test_equals(Target *target, ValueData *ret, ValueData *a, ValueData *b);
}

} // namespace libscratchcpp
