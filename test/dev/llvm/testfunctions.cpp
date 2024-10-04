#include <scratchcpp/value_functions.h>
#include <iostream>

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
}
