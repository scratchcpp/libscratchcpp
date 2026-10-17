// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilercontext.h>
#include <scratchcpp/compiler.h>
#include <map>
#include <vector>

#include "../../../test_export.h"

namespace libscratchcpp
{

class BlockPrototype;

/*!
 * \brief The CppCompilerContext class collects the C++ code of all functions compiled for a target.
 *
 * The C++ code can't be executed until it's compiled and linked into the application.
 * Compiled modules register their functions on startup, see moduleKey().
 */
class LIBSCRATCHCPP_TEST_EXPORT CppCompilerContext : public CompilerContext
{
    public:
        CppCompilerContext(IEngine *engine, Target *target);

        std::string createFunctionName(BlockPrototype *procedurePrototype);
        void declareExternFunction(const std::string &name, const std::string &declaration);
        void declareProcedure(const std::string &name, const std::string &declaration);
        void addFunction(const std::string &name, const std::string &code, const std::string &entry, bool registered);

        std::string sourceCode() const;
        std::string moduleSource() const;
        uint64_t moduleKey() const;

        static std::string prelude();
        static std::string procedureFunctionName(BlockPrototype *procedurePrototype);
        static uint64_t hash(const std::string &str);

    private:
        struct Function
        {
                std::string name;
                std::string code;
                std::string entry;
                bool registered = false;
        };

        int m_functionCount = 0;
        std::map<std::string, std::string> m_externDeclarations;
        std::map<std::string, std::string> m_procedureDeclarations;
        std::vector<Function> m_functions;
};

} // namespace libscratchcpp
