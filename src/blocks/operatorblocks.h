// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The OperatorBlocks class contains the implementation of operator blocks. */
class LIBSCRATCHCPP_EXPORT OperatorBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            NUM1,
            NUM2,
            FROM,
            TO,
            OPERAND1,
            OPERAND2,
            OPERAND
        };

        OperatorBlocks();

        std::string name() const override;

        static Value add(const BlockArgs &args);
        static Value subtract(const BlockArgs &args);
        static Value multiply(const BlockArgs &args);
        static Value divide(const BlockArgs &args);
        static Value pickRandom(const BlockArgs &args);
        static Value lessThan(const BlockArgs &args);
        static Value equals(const BlockArgs &args);
        static Value greaterThan(const BlockArgs &args);
        static Value andGate(const BlockArgs &args);
        static Value orGate(const BlockArgs &args);
        static Value notGate(const BlockArgs &args);
};

} // namespace libscratchcpp
