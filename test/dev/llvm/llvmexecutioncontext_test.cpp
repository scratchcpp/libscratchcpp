#include <scratchcpp/target.h>
#include <dev/engine/internal/llvmexecutioncontext.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMExecutionContextTest, Constructor)
{
    Target target;
    LLVMExecutionContext ctx(&target);
    ASSERT_EQ(ctx.target(), &target);
}
