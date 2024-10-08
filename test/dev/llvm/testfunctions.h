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
    void test_lower_than(Target *target, ValueData *ret, ValueData *a, ValueData *b);
    void test_const(Target *target, ValueData *ret, ValueData *v);

    void test_unreachable(Target *target);

    void test_reset_counter(Target *target);
    void test_increment_counter(Target *target);
    void test_get_counter(Target *target, ValueData *ret);
}

} // namespace libscratchcpp
