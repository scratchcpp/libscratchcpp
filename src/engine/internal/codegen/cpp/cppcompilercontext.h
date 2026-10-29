// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilercontext.h>
#include <scratchcpp/global.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace libscratchcpp
{

class CppExecutableCode;

class LIBSCRATCHCPP_EXPORT CppCompilerContext : public CompilerContext
{
    public:
        using CodeFactory = std::shared_ptr<CppExecutableCode> (*)(CppCompilerContext *ctx, const std::vector<void *> &pointers);
        using SourceCallback = std::function<void(uint64_t hash, const std::string &source)>;

        CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants = {});

        void addProcedure(const std::string &procCode, std::shared_ptr<CppExecutableCode> code);
        CppExecutableCode *procedure(const std::string &procCode) const;

        static std::string sourceHeader();
        static uint64_t hash(const std::string &source);
        static std::string className(uint64_t hash);

        static void registerCode(uint64_t hash, CodeFactory factory);
        static CodeFactory findCode(uint64_t hash);

        static void setSourceCallback(const SourceCallback &callback);
        static void reportSource(uint64_t hash, const std::string &source);

    private:
        std::unordered_map<std::string, std::shared_ptr<CppExecutableCode>> m_procedures;
};

} // namespace libscratchcpp
