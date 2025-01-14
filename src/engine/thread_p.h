// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class Target;
class IEngine;
class Script;
class ExecutableCode;
class ExecutionContext;

struct ThreadPrivate
{
        ThreadPrivate(Target *target, IEngine *engine, Script *script);

        Target *target = nullptr;
        IEngine *engine = nullptr;
        Script *script = nullptr;
        ExecutableCode *code = nullptr;
        std::shared_ptr<ExecutionContext> executionContext;
};

} // namespace libscratchcpp
