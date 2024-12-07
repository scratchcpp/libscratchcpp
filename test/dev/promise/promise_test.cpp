#include <scratchcpp/dev/promise.h>

#include "../../common.h"

using namespace libscratchcpp;

TEST(PromiseTest, Resolve)
{
    Promise promise;
    ASSERT_FALSE(promise.isResolved());

    promise.resolve();
    ASSERT_TRUE(promise.isResolved());
}
