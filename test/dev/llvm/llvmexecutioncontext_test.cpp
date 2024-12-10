#include <scratchcpp/thread.h>
#include <dev/engine/internal/llvm/llvmexecutioncontext.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMExecutionContextTest, Constructor)
{
    Thread thread(nullptr, nullptr, nullptr);
    LLVMExecutionContext ctx(&thread);
    ASSERT_EQ(ctx.thread(), &thread);
}
