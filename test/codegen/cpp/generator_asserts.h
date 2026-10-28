// Included in all codegen_test_generator sources.
// The generator only collects the code built by the tests, so fatal assertions are
// turned into non-fatal ones to make sure all code in the tests is built.

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#undef ASSERT_TRUE
#undef ASSERT_FALSE
#undef ASSERT_EQ
#undef ASSERT_NE
#undef ASSERT_LT
#undef ASSERT_LE
#undef ASSERT_GT
#undef ASSERT_GE
#undef ASSERT_STREQ
#undef ASSERT_STRNE
#undef ASSERT_FLOAT_EQ
#undef ASSERT_DOUBLE_EQ
#undef ASSERT_NEAR
#undef ASSERT_THAT

#define ASSERT_TRUE EXPECT_TRUE
#define ASSERT_FALSE EXPECT_FALSE
#define ASSERT_EQ EXPECT_EQ
#define ASSERT_NE EXPECT_NE
#define ASSERT_LT EXPECT_LT
#define ASSERT_LE EXPECT_LE
#define ASSERT_GT EXPECT_GT
#define ASSERT_GE EXPECT_GE
#define ASSERT_STREQ EXPECT_STREQ
#define ASSERT_STRNE EXPECT_STRNE
#define ASSERT_FLOAT_EQ EXPECT_FLOAT_EQ
#define ASSERT_DOUBLE_EQ EXPECT_DOUBLE_EQ
#define ASSERT_NEAR EXPECT_NEAR
#define ASSERT_THAT EXPECT_THAT
