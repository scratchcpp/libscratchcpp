#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/dev/promise.h>

#include <enginemock.h>

#include "../../common.h"

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
