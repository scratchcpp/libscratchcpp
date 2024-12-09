// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

namespace libscratchcpp
{

class IEngine;
class Target;
class Block;
class List;

} // namespace libscratchcpp

namespace libscratchcpp::test
{

class ScriptBuilderPrivate
{
    public:
        ScriptBuilderPrivate(IEngine *engine, std::shared_ptr<Target> target);
        ScriptBuilderPrivate(const ScriptBuilderPrivate &) = delete;

        IEngine *engine = nullptr;
        std::shared_ptr<Target> target;
        std::shared_ptr<Block> lastBlock;
        std::vector<std::shared_ptr<Block>> blocks;
        std::vector<std::shared_ptr<Block>> inputBlocks;
        unsigned int blockId = 0;
};

} // namespace libscratchcpp::test
