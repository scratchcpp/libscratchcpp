#include <scratchcpp/target.h>
#include <dev/engine/internal/llvmexecutioncontext.h>

#include "../../common.h"

using namespace libscratchcpp;

TEST(LLVMExecutionContextTest, Constructor)
{
    Target target;
    LLVMExecutionContext ctx(&target);
    ASSERT_EQ(ctx.target(), &target);
}

TEST(LLVMExecutionContextTest, Pos)
{
    LLVMExecutionContext ctx(nullptr);
    ASSERT_EQ(ctx.pos(), 0);

    ctx.setPos(1);
    ASSERT_EQ(ctx.pos(), 1);

    ctx.setPos(356);
    ASSERT_EQ(ctx.pos(), 356);
}
