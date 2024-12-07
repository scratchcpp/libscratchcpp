#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/target.h>
#include <scratchcpp/dev/promise.h>

#include "../../common.h"

using namespace libscratchcpp;

TEST(ExecutionContextTest, Constructor)
{
    Target target;
    ExecutionContext ctx(&target);
    ASSERT_EQ(ctx.target(), &target);
}

TEST(ExecutionContextTest, Promise)
{
    Target target;
    ExecutionContext ctx(&target);
    ASSERT_EQ(ctx.promise(), nullptr);

    auto promise = std::make_shared<Promise>();
    ctx.setPromise(promise);
    ASSERT_EQ(ctx.promise(), promise);

    ctx.setPromise(nullptr);
    ASSERT_EQ(ctx.promise(), nullptr);
}
