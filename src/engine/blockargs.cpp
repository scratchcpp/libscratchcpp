// SPDX-License-Identifier: Apache-2.0

#include "blockargs.h"
#include "../scratch/block.h"
#include <cassert>

namespace libscratchcpp
{

/*! Constructs BlockArgs. */
BlockArgs::BlockArgs(Target *target, Engine *engine, RunningScript *script, Block *block) :
    m_target(target),
    m_engine(engine),
    m_script(script),
    m_block(block)
{
    assert(target);
    assert(engine);
    assert(block);
}

/*! Returns the Target (a sprite or the stage) that called the block. */
Target *BlockArgs::target() const
{
    return m_target;
}

/*! Returns the Engine of the project. */
Engine *BlockArgs::engine() const
{
    return m_engine;
}

/*! Returns the script which contains the block. */
RunningScript *BlockArgs::script() const
{
    return m_script;
}

/*! Returns the block that was called. */
Block *BlockArgs::block() const
{
    return m_block;
}

/*! Returns the input with the given name. */
std::shared_ptr<Input> BlockArgs::input(const std::string &name) const
{
    return m_block->inputAt(m_block->findInput(name));
}

/*! \copydoc input() */
std::shared_ptr<Input> BlockArgs::input(const char *name) const
{
    return input(std::string(name));
}

/*! Returns the input with the given ID. */
std::shared_ptr<Input> BlockArgs::input(int id) const
{
    return m_block->inputAt(m_block->findInputById(id));
}

/*! Returns the field with the given name. */
std::shared_ptr<Field> BlockArgs::field(const std::string &name) const
{
    return m_block->fieldAt(m_block->findField(name));
}

/*! \copydoc field() */
std::shared_ptr<Field> BlockArgs::field(const char *name) const
{
    return field(std::string(name));
}

/*! Returns the field with the given ID. */
std::shared_ptr<Field> BlockArgs::field(int id) const
{
    return m_block->fieldAt(m_block->findFieldById(id));
}

} // namespace libscratchcpp
