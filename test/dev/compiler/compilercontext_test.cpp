#include <scratchcpp/dev/compilercontext.h>
#include <enginemock.h>
#include <targetmock.h>

using namespace libscratchcpp;

TEST(CompilerContextTest, Constructors)
{
    EngineMock engine;
    TargetMock target;
    CompilerContext ctx(&engine, &target);
    ASSERT_EQ(ctx.engine(), &engine);
    ASSERT_EQ(ctx.target(), &target);
}
