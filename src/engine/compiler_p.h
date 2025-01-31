// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>
#include <unordered_set>

namespace libscratchcpp
{

class CompilerContext;
class IEngine;
class Target;
class Block;
class CompilerValue;
class ICodeBuilderFactory;
class ICodeBuilder;

struct CompilerPrivate
{
        enum class SubstackType
        {
            Loop,
            IfStatement
        };

        CompilerPrivate(CompilerContext *ctx);
        CompilerPrivate(IEngine *engine, Target *target);

        static void initBuilderFactory();

        void substackEnd();

        std::shared_ptr<CompilerContext> ctxPtr; // for self-managed contexts
        CompilerContext *ctx = nullptr;

        std::shared_ptr<Block> block;
        int customIfStatementCount = 0;
        int customLoopCount = 0;
        std::vector<std::pair<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>, SubstackType>> substackTree;
        bool substackHit = false;
        bool emptySubstack = false;
        bool warp = false;

        static inline ICodeBuilderFactory *builderFactory = nullptr;
        std::shared_ptr<ICodeBuilder> builder;

        std::unordered_set<std::string> unsupportedBlocks;
};

} // namespace libscratchcpp
