// SPDX-License-Identifier: Apache-2.0

#include "codebuilderfactory.h"
#include "llvm/llvmcodebuilder.h"
#include "llvm/llvmcompilercontext.h"

using namespace libscratchcpp;

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::m_instance = std::make_shared<CodeBuilderFactory>();

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<ICodeBuilder> CodeBuilderFactory::create(CompilerContext *ctx, bool warp) const
{
    assert(dynamic_cast<LLVMCompilerContext *>(ctx));
    return std::make_shared<LLVMCodeBuilder>(static_cast<LLVMCompilerContext *>(ctx), warp);
}

std::shared_ptr<CompilerContext> CodeBuilderFactory::createCtx(IEngine *engine, Target *target) const
{
    auto ptr = std::make_shared<LLVMCompilerContext>(engine, target);
    return ptr;
}
