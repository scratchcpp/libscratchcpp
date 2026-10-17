// SPDX-License-Identifier: Apache-2.0

#include <cassert>

#include "cppcodebuilderfactory.h"
#include "cppcodebuilder.h"
#include "cppcompilercontext.h"

using namespace libscratchcpp;

std::shared_ptr<CppCodeBuilderFactory> CppCodeBuilderFactory::m_instance = std::make_shared<CppCodeBuilderFactory>();

std::shared_ptr<CppCodeBuilderFactory> CppCodeBuilderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<ICodeBuilder> CppCodeBuilderFactory::create(CompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) const
{
    assert(dynamic_cast<CppCompilerContext *>(ctx));
    return std::make_shared<CppCodeBuilder>(static_cast<CppCompilerContext *>(ctx), procedurePrototype, codeType);
}

std::shared_ptr<CompilerContext> CppCodeBuilderFactory::createCtx(IEngine *engine, Target *target) const
{
    return std::make_shared<CppCompilerContext>(engine, target);
}
