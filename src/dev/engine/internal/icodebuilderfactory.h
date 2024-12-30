// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class ICodeBuilder;
class CompilerContext;
class Target;
class IEngine;

class ICodeBuilderFactory
{
    public:
        virtual ~ICodeBuilderFactory() { }

        virtual std::shared_ptr<ICodeBuilder> create(CompilerContext *ctx, bool warp) const = 0;
        virtual std::shared_ptr<CompilerContext> createCtx(IEngine *engine, Target *target) const = 0;
};

} // namespace libscratchcpp
