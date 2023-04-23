// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../scratch/target.h"
#include "block.h"
#include <map>

namespace libscratchcpp
{

/*! \brief The RunningScript class represents a script that is being executed. */
class LIBSCRATCHCPP_EXPORT RunningScript
{
    public:
        RunningScript(std::shared_ptr<Block> block, std::shared_ptr<Target> target, Engine *engine);

        std::shared_ptr<Block> currentBlock() const;
        void moveToNextBlock();

        std::shared_ptr<Block> cMouth();
        void moveToSubstack(std::shared_ptr<Block> substackBlock);
        void moveToSubstack(const BlockArgs &args, int inputId);
        std::shared_ptr<Block> getSubstack(const BlockArgs &args, int inputId) const;
        void skipSubstack();

        std::shared_ptr<Target> target() const;

        bool atCMouthEnd() const;

    private:
        bool substackEnd();
        std::shared_ptr<Block> m_currentBlock;
        // pair<C mouth block, first block of the substack>
        std::vector<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>> m_tree;
        std::shared_ptr<Target> m_target;
        Engine *m_engine = nullptr;
        bool m_atCMouthEnd = false;
};

} // namespace libscratchcpp
