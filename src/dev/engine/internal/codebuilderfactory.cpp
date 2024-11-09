// SPDX-License-Identifier: Apache-2.0

#include "codebuilderfactory.h"
#include "llvmcodebuilder.h"

using namespace libscratchcpp;

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::m_instance = std::make_shared<CodeBuilderFactory>();

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<ICodeBuilder> CodeBuilderFactory::create(Target *target, const std::string &id, bool warp) const
{
    return std::make_shared<LLVMCodeBuilder>(target, id, warp);
}
