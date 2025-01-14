// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>
#include <scratchcpp/value.h>

namespace libscratchcpp
{

class Target;
class Block;
class IEngine;
class ExecutableCode;
class Variable;
class List;

struct ScriptPrivate
{
        ScriptPrivate(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine);
        ScriptPrivate(const ScriptPrivate &) = delete;

        std::shared_ptr<ExecutableCode> code;

        Target *target = nullptr;
        std::shared_ptr<Block> topBlock;
        IEngine *engine = nullptr;
};

} // namespace libscratchcpp
