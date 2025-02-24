// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Target;
class Block;
class IEngine;
class ExecutableCode;
class Thread;
class ScriptPrivate;

/*! \brief The Script class represents a compiled Scratch script. */
class LIBSCRATCHCPP_EXPORT Script
{
    public:
        Script(Target *target, Block *topBlock, IEngine *engine);
        Script(const Script &) = delete;

        Target *target() const;
        Block *topBlock() const;

        ExecutableCode *code() const;
        void setCode(std::shared_ptr<ExecutableCode> code);

        ExecutableCode *hatPredicateCode() const;
        void setHatPredicateCode(std::shared_ptr<ExecutableCode> code);

        bool runHatPredicate(Target *target);

        std::shared_ptr<Thread> start();
        std::shared_ptr<Thread> start(Target *target);

    private:
        spimpl::unique_impl_ptr<ScriptPrivate> impl;
};

} // namespace libscratchcpp
