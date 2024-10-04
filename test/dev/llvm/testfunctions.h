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
}

} // namespace libscratchcpp
