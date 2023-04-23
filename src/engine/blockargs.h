// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include "../scratch/input.h"
#include <memory>
#include <vector>

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT Target;
class LIBSCRATCHCPP_EXPORT Engine;
class LIBSCRATCHCPP_EXPORT RunningScript;
class LIBSCRATCHCPP_EXPORT Field;

/*! \brief The BlockArgs class is a container for arguments passed to block implementations. */
class LIBSCRATCHCPP_EXPORT BlockArgs
{
    public:
        BlockArgs(Target *target, Engine *engine, RunningScript *script, Block *block);

        Target *target() const;
        Engine *engine() const;
        RunningScript *script() const;
        Block *block() const;

        std::shared_ptr<Input> input(const std::string &name) const;
        std::shared_ptr<Input> input(const char *name) const;
        std::shared_ptr<Input> input(int id) const;

        std::shared_ptr<Field> field(const std::string &name) const;
        std::shared_ptr<Field> field(const char *name) const;
        std::shared_ptr<Field> field(int id) const;

    private:
        Target *m_target = nullptr;
        Engine *m_engine = nullptr;
        RunningScript *m_script = nullptr;
        Block *m_block = nullptr;
};

} // namespace libscratchcpp
