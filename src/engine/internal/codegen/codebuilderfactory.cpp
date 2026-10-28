// SPDX-License-Identifier: Apache-2.0

#include "codebuilderfactory.h"

#ifdef CODEGEN_BACKEND_LLVM
#include "llvm/llvmcodebuilder.h"
#include "llvm/llvmcompilercontext.h"
#elif defined(CODEGEN_BACKEND_CPP)
#endif

using namespace libscratchcpp;

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::m_instance = std::make_shared<CodeBuilderFactory>();

std::shared_ptr<CodeBuilderFactory> CodeBuilderFactory::instance()
{
    return m_instance;
}

std::shared_ptr<ICodeBuilder> CodeBuilderFactory::create(CompilerContext *ctx, BlockPrototype *procedurePrototype, Compiler::CodeType codeType) const
{
#ifdef CODEGEN_BACKEND_LLVM
    assert(dynamic_cast<LLVMCompilerContext *>(ctx));
    return std::make_shared<LLVMCodeBuilder>(static_cast<LLVMCompilerContext *>(ctx), procedurePrototype, codeType);
#elif defined(CODEGEN_BACKEND_CPP)
    return nullptr;
#endif
}

std::shared_ptr<CompilerContext> CodeBuilderFactory::createCtx(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants) const
{
#ifdef CODEGEN_BACKEND_LLVM
    auto ptr = std::make_shared<LLVMCompilerContext>(engine, target, unsafeConstants);
    return ptr;
#elif defined(CODEGEN_BACKEND_CPP)
    return nullptr;
#endif
}
