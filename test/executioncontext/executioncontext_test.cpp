#include <scratchcpp/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>

#include <enginemock.h>
#include <stacktimermock.h>
#include <randomgeneratormock.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ExecutionContextTest, Constructor)
{
    EngineMock engine;
    Thread thread(nullptr, &engine, nullptr);
    ExecutionContext ctx(&thread);
    ASSERT_EQ(ctx.thread(), &thread);
    ASSERT_EQ(ctx.engine(), &engine);
}

TEST(ExecutionContextTest, Promise)
{
    ExecutionContext ctx(nullptr);
    ASSERT_EQ(ctx.promise(), nullptr);

    auto promise = std::make_shared<Promise>();
    ctx.setPromise(promise);
    ASSERT_EQ(ctx.promise(), promise);

    ctx.setPromise(nullptr);
    ASSERT_EQ(ctx.promise(), nullptr);
}

TEST(ExecutionContextTest, StackTimer)
{
    ExecutionContext ctx(nullptr);
    ASSERT_TRUE(ctx.stackTimer());

    StackTimerMock timer;
    ctx.setStackTimer(&timer);
    ASSERT_EQ(ctx.stackTimer(), &timer);
}

TEST(ExecutionContextTest, Rng)
{
    ExecutionContext ctx(nullptr);
    ASSERT_TRUE(ctx.rng());

    RandomGeneratorMock rng;
    ctx.setRng(&rng);
    ASSERT_EQ(ctx.rng(), &rng);
}

TEST(ExecutionContextTest, StringStack)
{
    EngineMock engine;
    ExecutionContext ctx(nullptr);

    StringPtr **frame1 = ctx.pushStringStack(3);

    for (int i = 0; i < 3; i++) {
        string_assign_cstring(frame1[i], "test"); // address sanitizer will detect potential buffer overflow
    }

    StringPtr **frame2 = ctx.pushStringStack(5);

    for (int i = 0; i < 5; i++) {
        string_assign_cstring(frame2[i], "foo bar");
    }

    ctx.popStringStack();

    StringPtr testString("test");
    ASSERT_EQ(string_compare_case_sensitive(frame1[2], &testString), 0);

    ctx.popStringStack();
}
