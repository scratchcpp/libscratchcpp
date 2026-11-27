// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilercontext.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "test_export.h"

namespace libscratchcpp
{

class ExecutableCode;

class LIBSCRATCHCPP_TEST_EXPORT CppCompilerContext : public CompilerContext
{
    public:
        using GeneratedCodeFactory = std::function<std::shared_ptr<ExecutableCode>(const std::string &, CppCompilerContext *)>;
        using TestCodeIdProvider = std::function<std::string()>;

        CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants = {});

        void setTestCodeId(const std::string &id) { m_testCodeId = id; }
        const std::string &testCodeId() const { return m_testCodeId; }
        void setGeneratedCodeFactory(GeneratedCodeFactory factory) { m_generatedCodeFactory = std::move(factory); }
        const GeneratedCodeFactory &generatedCodeFactory() const;

        static void setDefaultGeneratedCodeFactory(GeneratedCodeFactory factory);
        static void setTestCodeIdProvider(TestCodeIdProvider provider);
        static std::string nextTestCodeId();
        static void setTestGenerationDirectory(const std::string &directory);
        static const std::string &testGenerationDirectory();

    private:
        std::string m_testCodeId;
        GeneratedCodeFactory m_generatedCodeFactory;
};

} // namespace libscratchcpp
