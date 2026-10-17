// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../icodebuilderfactory.h"
#include "../../../test_export.h"

namespace libscratchcpp
{

/*! \brief The CppCodeBuilderFactory class creates code builders which generate C++ code. */
class LIBSCRATCHCPP_TEST_EXPORT CppCodeBuilderFactory : public ICodeBuilderFactory
{
    public:
        static std::shared_ptr<CppCodeBuilderFactory> instance();

        std::shared_ptr<ICodeBuilder> create(CompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) const override;
        std::shared_ptr<CompilerContext> createCtx(IEngine *engine, Target *target) const override;

    private:
        static std::shared_ptr<CppCodeBuilderFactory> m_instance;
};

} // namespace libscratchcpp
