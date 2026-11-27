// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <engine/internal/codegen/cpp/cppownedvalue.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <memory>
#include <stdexcept>

using namespace libscratchcpp;

using PoolString = std::unique_ptr<StringPtr, decltype(&string_pool_free)>;

static ValueData nativeStringResult()
{
    ValueData result{};
    value_assign_cstring(&result, "ready");
    return result;
}

TEST(CppOwnedValueTest, ReleasesReplacedResultsAndCleansUpAtScopeExit)
{
    PoolString reclaimed(nullptr, string_pool_free);
    StringPtr *lastResult = nullptr;
    {
        CppOwnedValue result;
        result = nativeStringResult();
        for (int i = 0; i < 20; ++i) {
            reclaimed.reset();
            StringPtr *previous = result.stringValue;
            result = nativeStringResult();

            // These results occupy the pool's two highest-capacity slots. Keep
            // the current result checked out while reclaiming the previous one.
            reclaimed.reset(string_pool_new());
            EXPECT_EQ(reclaimed.get(), previous);
            EXPECT_EQ(std::u16string(result.stringValue->data), u"ready");
        }
        lastResult = result.stringValue;
    }

    // The other slot stays leased, so scope exit makes the final result available.
    PoolString finalResult(string_pool_new(), string_pool_free);
    EXPECT_EQ(finalResult.get(), lastResult);
}

TEST(CppOwnedValueTest, ReleasesResultDuringExceptionUnwinding)
{
    StringPtr *allocated = nullptr;
    const auto run = [&] {
        CppOwnedValue result;
        result = nativeStringResult();
        allocated = result.stringValue;
        throw std::runtime_error("native call failed");
    };
    EXPECT_THROW(run(), std::runtime_error);

    PoolString reclaimed(string_pool_new(), string_pool_free);
    EXPECT_EQ(reclaimed.get(), allocated);
}
