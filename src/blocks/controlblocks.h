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

        enum FieldValues
        {
            StopAll,
            StopThisScript,
            StopOtherScriptsInSprite
        };

        ControlBlocks();

        std::string name() const override;

        static Value repeatForever(const BlockArgs &args);
        static Value repeat(const BlockArgs &args);
        static Value repeatUntil(const BlockArgs &args);
        static Value repeatWhile(const BlockArgs &args);
        static Value ifStatement(const BlockArgs &args);
        static Value ifElseStatement(const BlockArgs &args);
        static Value stop(const BlockArgs &args);

    private:
        // pair<script, C mouth>, pair<current index, repeat count>
        static std::map<std::pair<RunningScript *, Block *>, std::pair<int, int>> repeatLoops;
};

} // namespace libscratchcpp
