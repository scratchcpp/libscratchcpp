// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The ControlBlocks class contains the implementation of control blocks. */
class LIBSCRATCHCPP_EXPORT ControlBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            SUBSTACK,
            SUBSTACK2,
            TIMES,
            CONDITION
        };

        enum Fields
        {
            STOP_OPTION
        };

        ControlBlocks();

        std::string name() const override;

        Value repeatForever(const BlockArgs &args);
        Value repeat(const BlockArgs &args);
        Value repeatUntil(const BlockArgs &args);
        Value repeatWhile(const BlockArgs &args);
        Value ifStatement(const BlockArgs &args);
        Value ifElseStatement(const BlockArgs &args);
        Value stop(const BlockArgs &args);

    private:
        // pair<script, C mouth>, pair<current index, repeat count>
        std::map<std::pair<RunningScript *, Block *>, std::pair<int, int>> repeatLoops;
};

} // namespace libscratchcpp
