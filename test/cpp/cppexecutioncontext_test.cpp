#include <scratchcpp/thread.h>
#include <engine/internal/codegen/cpp/cppexecutioncontext.h>
#include <engine/internal/codegen/cpp/cppcompilercontext.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(CppExecutionContextTest, Constructor)
{
    Thread thread(nullptr, nullptr, nullptr);
    CppCompilerContext compilerCtx(nullptr, nullptr);
    CppExecutionContext ctx(&compilerCtx, &thread);
    ASSERT_EQ(ctx.thread(), &thread);
}
