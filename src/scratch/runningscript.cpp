// SPDX-License-Identifier: Apache-2.0

#include "runningscript.h"
#include "../engine/engine.h"
#include <cassert>
#include <iostream>

using namespace libscratchcpp;

/*! Constructs RunningScript. */
RunningScript::RunningScript(std::shared_ptr<Block> block, std::shared_ptr<Target> target, Engine *engine) :
    m_currentBlock(block),
    m_target(target),
    m_engine(engine)
{
    assert(engine);
    if (!block) {
        std::cout << "warning: creating a script with a null block" << std::endl;
        return;
    }

    if (!block->topLevel())
        std::cout << "warning: " << block->id() << ": initial script block should be top-level" << std::endl;

    if (!target) {
        std::cerr << "error: " << block->id() << ": creating script of a null target" << std::endl;
        assert(false);
    }
}

/*! Returns the current block. */
std::shared_ptr<Block> RunningScript::currentBlock() const
{
    return m_currentBlock;
}

/*!
 * Continues to the next block, cMouth() or to the first block of a C mouth.
 * \note Use moveToSubstack() to move to the substack of e. g. a loop, or
 * skipSubstack(), if it doesn't have one.
 */
void RunningScript::moveToNextBlock()
{
    if (m_currentBlock) {
        if (!m_currentBlock->next() && !m_tree.empty()) {
            bool ret = substackEnd();
            if (!ret)
                return;
            moveToNextBlock();
            return;
        }
        m_currentBlock = m_currentBlock->next();
    }
}

/*! Returns the C mouth block or nullptr if the block isn't in a C mouth. */
std::shared_ptr<Block> RunningScript::cMouth()
{
    if (m_tree.empty())
        return nullptr;
    return m_tree[m_tree.size() - 1].first;
}

/*!
 * Moves to a substack, e. g. in a loop.
 * \note Please use the overloaded function with the list of inputs
 * and the index of the substack input because it'll automatically
 * check if there's a substack defined.
 */
void RunningScript::moveToSubstack(std::shared_ptr<Block> substackBlock)
{
    if (!substackBlock) {
        std::cout << "warning: moving to null substack block (nothing will happen)" << std::endl;
        return;
    }

    if (!m_currentBlock)
        std::cout << "warning: " << substackBlock->id() << ": moving to substack from a null block" << std::endl;

    m_tree.push_back({ m_currentBlock, substackBlock });
    m_currentBlock = substackBlock;
    m_engine->stayOnCurrentBlock();
}

/*!
 * Moves to a substack, e. g. in a loop.
 * \param[in] args The arguments passed to the block.
 * \param[in] inputId The ID of the substack input (registered by the block section).
 */
void RunningScript::moveToSubstack(const BlockArgs &args, int inputId)
{
    auto substack = getSubstack(args, inputId);
    if (substack)
        moveToSubstack(substack);
    else
        skipSubstack();
}

/*!
 * Finds the substack.
 * \param[in] args The arguments passed to the block.
 * \param[in] inputId The ID of the substack input (registered by the block section).
 */
std::shared_ptr<Block> RunningScript::getSubstack(const BlockArgs &args, int inputId) const
{
    auto block = args.block();
    int index = block->findInputById(inputId);
    if (index != -1) {
        auto ret = block->inputAt(index)->valueBlock();
        if (ret)
            return ret;
    }
    return nullptr;
}

/*! Use this to avoid passing a null block to moveToSubstack(). */
void RunningScript::skipSubstack()
{
    m_tree.push_back({ m_currentBlock, nullptr });
    m_currentBlock = nullptr;
    bool ret = substackEnd();
    if (!ret) {
        m_tree.pop_back();
        m_engine->stayOnCurrentBlock();
    }
}

/*! Returns the Target which started the script. */
std::shared_ptr<Target> RunningScript::target() const
{
    return m_target;
}

/*!
 * Returns true if a block implementation has been
 * called to check whether to break from a C mouth.
 */
bool RunningScript::atCMouthEnd() const
{
    return m_atCMouthEnd;
}

bool RunningScript::substackEnd()
{
    /*
     * Call the C mouth block implementation
     * to check whether to break from the C
     * mouth or to jump to the beginning of it.
     */
    auto cMouthBlock = cMouth();
    m_atCMouthEnd = true;
    bool ret = cMouthBlock->run(this, true).toBool();
    m_atCMouthEnd = false;
    if (!m_engine->isAtomic())
        m_engine->breakFrame();
    if (ret) {
        m_currentBlock = cMouthBlock;
        m_tree.pop_back();
    } else {
        m_currentBlock = m_tree[m_tree.size() - 1].second;
        if (!m_currentBlock)
            m_currentBlock = cMouthBlock;
    }
    return ret;
}
