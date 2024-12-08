#include <engine/internal/randomgenerator.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(RandomGeneratorTest, RandInt)
{
    auto rng = RandomGenerator::instance();
    ASSERT_TRUE(rng);

    long num;

    for (int i = 0; i < 25; i++) {
        num = rng->randint(-2, 3);
        ASSERT_GE(num, -2);
        ASSERT_LE(num, 3);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randint(5, -3);
        ASSERT_GE(num, -3);
        ASSERT_LE(num, 5);
    }
}

TEST(RandomGeneratorTest, RandIntDouble)
{
    auto rng = RandomGenerator::instance();
    ASSERT_TRUE(rng);

    long num;

    for (int i = 0; i < 1000; i++) {
        num = rng->randintDouble(-2.23, 3.875);
        ASSERT_GE(num, -2.23);
        ASSERT_LE(num, 3.875);
    }

    for (int i = 0; i < 1000; i++) {
        num = rng->randintDouble(5.081, -2.903);
        ASSERT_GE(num, -2.903);
        ASSERT_LE(num, 5.081);
    }
}

TEST(RandomGeneratorTest, RandIntExcept)
{
    auto rng = RandomGenerator::instance();
    ASSERT_TRUE(rng);

    long num;

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(-2, 3, 0);
        ASSERT_GE(num, -2);
        ASSERT_LE(num, 3);
        ASSERT_NE(num, 0);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(5, -3, 2);
        ASSERT_GE(num, -3);
        ASSERT_LE(num, 5);
        ASSERT_NE(num, 2);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(5, 8, 2);
        ASSERT_GE(num, 5);
        ASSERT_LE(num, 8);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(5, 8, 10);
        ASSERT_GE(num, 5);
        ASSERT_LE(num, 8);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(2, 2, 2);
        ASSERT_EQ(num, 2);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(2, 2, 5);
        ASSERT_EQ(num, 2);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(1, 2, 1);
        ASSERT_EQ(num, 2);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(1, 2, 2);
        ASSERT_EQ(num, 1);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(1, 5, 1);
        ASSERT_GE(num, 2);
        ASSERT_LE(num, 5);
    }

    for (int i = 0; i < 25; i++) {
        num = rng->randintExcept(1, 5, 5);
        ASSERT_GE(num, 1);
        ASSERT_LE(num, 4);
    }
}
