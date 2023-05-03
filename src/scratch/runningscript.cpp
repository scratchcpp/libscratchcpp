// SPDX-License-Identifier: Apache-2.0

#include "runningscript.h"
#include <cassert>

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
