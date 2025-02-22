#pragma once

namespace libscratchcpp
{

class TestMock;
class Target;
class ValueData;
class List;
class ExecutionContext;
struct StringPtr;

extern "C"
{
    void test_function(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData);
    ValueData test_reporter(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData);
    bool test_predicate(TestMock *mock, ExecutionContext *ctx, Target *target, ValueData **varData, List **listData);
    void test_print_function(ValueData *arg1, ValueData *arg2);

    void test_empty_function();
    void test_ctx_function(ExecutionContext *ctx, const StringPtr *arg);

    void test_function_no_args(Target *target);
    StringPtr *test_function_no_args_ret(Target *target);
    void test_function_1_arg(Target *target, const StringPtr *arg1);
    StringPtr *test_function_1_arg_ret(Target *target, const StringPtr *arg1);
    void test_function_3_args(Target *target, const StringPtr *arg1, const StringPtr *arg2, const StringPtr *arg3);
    StringPtr *test_function_3_args_ret(Target *target, const StringPtr *arg1, const StringPtr *arg2, const StringPtr *arg3);

    bool test_equals(const StringPtr *a, const StringPtr *b);
    bool test_lower_than(double a, double b);
    bool test_not(bool arg);
    double test_const_number(double v);
    bool test_const_bool(bool v);
    StringPtr *test_const_string(const StringPtr *v);

    void test_unreachable();

    void test_reset_counter();
    void test_increment_counter();
    double test_get_counter();

    void test_print_number(double v);
    void test_print_bool(bool v);
    void test_print_string(const StringPtr *v);
}

} // namespace libscratchcpp
