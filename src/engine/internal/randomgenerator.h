// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/irandomgenerator.h>
#include <memory>
#include <random>

namespace libscratchcpp
{

class RandomGenerator : public IRandomGenerator
{
    public:
        RandomGenerator();
        RandomGenerator(const RandomGenerator &) = delete;

        static std::shared_ptr<RandomGenerator> instance();

        long randint(long start, long end) const override;
        double randintDouble(double start, double end) const override;
        long randintExcept(long start, long end, long except) const override;

    private:
        static std::shared_ptr<RandomGenerator> m_instance;
        std::random_device m_device;
        std::unique_ptr<std::mt19937> m_generator;
};

} // namespace libscratchcpp
