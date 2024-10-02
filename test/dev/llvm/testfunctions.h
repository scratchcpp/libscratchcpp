#pragma once

namespace libscratchcpp
{

class TestMock;
class Target;

extern "C" void test_function(TestMock *mock, Target *target);

} // namespace libscratchcpp
