#pragma once

namespace libscratchcpp
{

class TestMock;
class Target;
class ValueData;
class List;

extern "C"
{
    void test_function(TestMock *mock, Target *target, ValueData **varData, List **listData);
    void test_print_function(ValueData *arg1, ValueData *arg2);

    void test_function_no_args(Target *target);
    char *test_function_no_args_ret(Target *target);
    void test_function_1_arg(Target *target, const char *arg1);
    char *test_function_1_arg_ret(Target *target, const char *arg1);
    void test_function_3_args(Target *target, const char *arg1, const char *arg2, const char *arg3);
    char *test_function_3_args_ret(Target *target, const char *arg1, const char *arg2, const char *arg3);

    bool test_equals(Target *target, const char *a, const char *b);
    bool test_lower_than(Target *target, double a, double b);
    bool test_not(Target *target, bool arg);
    double test_const_number(Target *target, double v);
    bool test_const_bool(Target *target, bool v);
    char *test_const_string(Target *target, const char *v);

    void test_unreachable(Target *target);

    void test_reset_counter(Target *target);
    void test_increment_counter(Target *target);
    double test_get_counter(Target *target);

    void test_print_number(Target *target, double v);
    void test_print_bool(Target *target, bool v);
    void test_print_string(Target *target, const char *v);
}

} // namespace libscratchcpp
