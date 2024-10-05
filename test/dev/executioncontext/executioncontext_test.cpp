#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/target.h>

#include "../../common.h"

using namespace libscratchcpp;

TEST(ExecutionContextTest, Constructor)
{
    Target target;
    ExecutionContext ctx(&target);
    ASSERT_EQ(ctx.target(), &target);
}
