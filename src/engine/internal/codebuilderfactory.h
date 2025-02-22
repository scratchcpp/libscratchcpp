// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "icodebuilderfactory.h"

namespace libscratchcpp
{

class CodeBuilderFactory : public ICodeBuilderFactory
{
    public:
        static std::shared_ptr<CodeBuilderFactory> instance();
        std::shared_ptr<ICodeBuilder> create(CompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) const override;
        std::shared_ptr<CompilerContext> createCtx(IEngine *engine, Target *target) const override;

    private:
        static std::shared_ptr<CodeBuilderFactory> m_instance;
};

} // namespace libscratchcpp
