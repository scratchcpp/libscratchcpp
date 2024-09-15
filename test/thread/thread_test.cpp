#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <targetmock.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ThreadTest, Constructor)
{
    TargetMock target;
    EngineMock engine;
    Script script(nullptr, nullptr, nullptr);
    Thread thread(&target, &engine, &script);
    ASSERT_EQ(thread.target(), &target);
    ASSERT_EQ(thread.engine(), &engine);
    ASSERT_EQ(thread.script(), &script);
    ASSERT_TRUE(thread.vm());
    ASSERT_EQ(thread.vm()->target(), &target);
    ASSERT_EQ(thread.vm()->engine(), &engine);
    ASSERT_EQ(thread.vm()->script(), &script);
}

// NOTE: Since VirtualMachine is going to be removed, tests for the other methods will be added later
