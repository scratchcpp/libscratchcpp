#include <scratchcpp/thread.h>
#include <engine/internal/llvm/llvmexecutioncontext.h>
#include <engine/internal/llvm/llvmcompilercontext.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMExecutionContextTest, Constructor)
{
    Thread thread(nullptr, nullptr, nullptr);
    LLVMCompilerContext compilerCtx(nullptr, nullptr);
    LLVMExecutionContext ctx(&compilerCtx, &thread);
    ASSERT_EQ(ctx.thread(), &thread);
}
