// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <memory>

namespace libscratchcpp
{

class ICodeBuilder;

class ICodeBuilderFactory
{
    public:
        virtual ~ICodeBuilderFactory() { }

        virtual std::shared_ptr<ICodeBuilder> create(CompilerContext *ctx, BlockPrototype *procedurePrototype = nullptr, Compiler::CodeType codeType = Compiler::CodeType::Script) const = 0;
        virtual std::shared_ptr<CompilerContext> createCtx(IEngine *engine, Target *target) const = 0;
};

} // namespace libscratchcpp
