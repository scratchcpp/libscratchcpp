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
        ScriptPrivate(Target *target, Block *topBlock, IEngine *engine);
        ScriptPrivate(const ScriptPrivate &) = delete;

        std::shared_ptr<ExecutableCode> code;
        std::shared_ptr<ExecutableCode> hatPredicateCode;

        Target *target = nullptr;
        Block *topBlock = nullptr;
        IEngine *engine = nullptr;
};

} // namespace libscratchcpp
