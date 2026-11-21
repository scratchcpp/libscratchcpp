// SPDX-License-Identifier: Apache-2.0

#include "cppcompilercontext.h"

#include <utility>

using namespace libscratchcpp;

namespace
{

CppCompilerContext::GeneratedCodeFactory defaultGeneratedCodeFactory;
CppCompilerContext::TestCodeIdProvider testCodeIdProvider;
std::string testGenerationDirectory;

} // namespace

CppCompilerContext::CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants) :
    CompilerContext(engine, target, unsafeConstants)
{
}

const CppCompilerContext::GeneratedCodeFactory &CppCompilerContext::generatedCodeFactory() const
{
    return m_generatedCodeFactory ? m_generatedCodeFactory : defaultGeneratedCodeFactory;
}

void CppCompilerContext::setDefaultGeneratedCodeFactory(GeneratedCodeFactory factory)
{
    defaultGeneratedCodeFactory = std::move(factory);
}

void CppCompilerContext::setTestCodeIdProvider(TestCodeIdProvider provider)
{
    testCodeIdProvider = std::move(provider);
}

std::string CppCompilerContext::nextTestCodeId()
{
    return testCodeIdProvider ? testCodeIdProvider() : std::string();
}

void CppCompilerContext::setTestGenerationDirectory(const std::string &directory)
{
    ::testGenerationDirectory = directory;
}

const std::string &CppCompilerContext::testGenerationDirectory()
{
    return ::testGenerationDirectory;
}
