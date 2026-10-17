// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/blockprototype.h>
#include <sstream>
#include <iomanip>

#include "cppcompilercontext.h"
#include "cppprelude_source.h"

using namespace libscratchcpp;

CppCompilerContext::CppCompilerContext(IEngine *engine, Target *target) :
    CompilerContext(engine, target)
{
}

/*! Returns a unique name for a new function (or the name of the procedure function). */
std::string CppCompilerContext::createFunctionName(BlockPrototype *procedurePrototype)
{
    if (procedurePrototype)
        return procedureFunctionName(procedurePrototype);

    return "script" + std::to_string(m_functionCount++);
}

/*! Declares an external function (e.g. a block function implemented in an extension). */
void CppCompilerContext::declareExternFunction(const std::string &name, const std::string &declaration)
{
    m_externDeclarations[name] = declaration;
}

/*! Adds a forward declaration of a procedure function. */
void CppCompilerContext::declareProcedure(const std::string &name, const std::string &declaration)
{
    m_procedureDeclarations[name] = declaration;
}

/*!
 * Adds a function.
 * \param[in] code The frame and the entry function definition.
 * \param[in] entry The name of the entry function.
 * \param[in] registered Whether to register the function so that CppExecutableCode can find it.
 */
void CppCompilerContext::addFunction(const std::string &name, const std::string &code, const std::string &entry, bool registered)
{
    m_functions.push_back({ name, code, entry, registered });
}

/*! Returns the C++ code of all functions (without the prelude and registration code). */
std::string CppCompilerContext::sourceCode() const
{
    std::stringstream ss;

    for (const auto &[name, decl] : m_externDeclarations)
        ss << decl << "\n";

    ss << "\n";

    for (const auto &[name, decl] : m_procedureDeclarations)
        ss << decl << "\n";

    for (const Function &f : m_functions)
        ss << "\n" << f.code;

    return ss.str();
}

/*! Returns the C++ code of the module with the registration code. The module source can be compiled after prelude(). */
std::string CppCompilerContext::moduleSource() const
{
    std::stringstream ss;
    uint64_t key = moduleKey();
    std::stringstream keyStr;
    keyStr << "0x" << std::hex << std::setw(16) << std::setfill('0') << key;

    // Extern declarations must be outside of the module namespace
    ss << "using namespace libscratchcpp;\n\n";

    for (const auto &[name, decl] : m_externDeclarations)
        ss << decl << "\n";

    ss << "\nnamespace module_" << keyStr.str().substr(2) << "\n{\n\n";
    ss << "using namespace libscratchcpp;\n\n";

    for (const auto &[name, decl] : m_procedureDeclarations)
        ss << decl << "\n";

    for (const Function &f : m_functions)
        ss << "\n" << f.code;

    ss << "\nstruct Registrar\n{\n        Registrar()\n        {\n";

    for (const Function &f : m_functions) {
        if (f.registered)
            ss << "            cpp_register_function(" << keyStr.str() << "ULL, \"" << f.name << "\", reinterpret_cast<void *>(&" << f.entry << "));\n";
    }

    ss << "        }\n};\n\nstatic Registrar registrar;\n\n} // namespace module_" << keyStr.str().substr(2) << "\n";
    return ss.str();
}

/*! Returns the unique key of this module (computed from the source code). */
uint64_t CppCompilerContext::moduleKey() const
{
    return hash(sourceCode());
}

/*! Returns the C++ code of the runtime prelude. */
std::string CppCompilerContext::prelude()
{
    return CPP_PRELUDE_SOURCE;
}

std::string CppCompilerContext::procedureFunctionName(BlockPrototype *procedurePrototype)
{
    std::stringstream ss;
    ss << "proc_" << std::hex << std::setw(16) << std::setfill('0') << hash(procedurePrototype->procCode());
    return ss.str();
}

/*! 64-bit FNV-1a hash. */
uint64_t CppCompilerContext::hash(const std::string &str)
{
    uint64_t ret = 0xcbf29ce484222325ULL;

    for (unsigned char c : str) {
        ret ^= c;
        ret *= 0x100000001b3ULL;
    }

    return ret;
}
