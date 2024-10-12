// SPDX-License-Identifier: Apache-2.0

#include <random>

#include "randomgenerator.h"

using namespace libscratchcpp;

std::shared_ptr<RandomGenerator> RandomGenerator::m_instance = std::make_shared<RandomGenerator>();

RandomGenerator::RandomGenerator() :
    m_generator(std::make_unique<std::mt19937>(m_device()))
{
}

std::shared_ptr<RandomGenerator> RandomGenerator::instance()
{
    return m_instance;
}

long RandomGenerator::randint(long start, long end) const
{
    if (start > end) {
        long tmp = start;
        start = end;
        end = tmp;
    }

    std::uniform_int_distribution<long> distribution(start, end);
    return distribution(*m_generator);
}

double RandomGenerator::randintDouble(double start, double end) const
{
    if (start > end) {
        double tmp = start;
        start = end;
        end = tmp;
    }

    std::uniform_real_distribution<double> distribution(start, end);
    return distribution(*m_generator);
}

long RandomGenerator::randintExcept(long start, long end, long except) const
{
    if (start > end) {
        std::swap(start, end);
    }

    if (except < start || except > end) {
        return randint(start, end);
    } else if (start == end) {
        return start;
    } else if (end - start == 1) {
        if (except == start)
            return end;
        else
            return start;
    }

    if (randint(0, 1) == 0) {
        std::uniform_int_distribution<long> distribution(start, except - 1);
        return distribution(*m_generator);
    } else {
        std::uniform_int_distribution<long> distribution(except + 1, end);
        return distribution(*m_generator);
    }
}
