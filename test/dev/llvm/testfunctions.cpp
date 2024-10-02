#include "testmock.h"

using namespace libscratchcpp;

extern "C" void test_function(TestMock *mock, Target *target)
{
    if (mock)
        mock->f(target);
}
