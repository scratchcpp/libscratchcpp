// SPDX-License-Identifier: Apache-2.0

#include <unordered_map>
#include <cassert>

#include "cppruntime.h"
#include "cppexecutioncontext.h"

using namespace libscratchcpp;

static std::unordered_map<std::string, void *> &functionRegistry()
{
    static std::unordered_map<std::string, void *> registry;
    return registry;
}

static std::string registryKey(uint64_t moduleKey, const std::string &name)
{
    return std::to_string(moduleKey) + ":" + name;
}

/*! Returns the function registered by a compiled C++ module, or nullptr if it doesn't exist. */
void *libscratchcpp::cppLookupFunction(uint64_t moduleKey, const std::string &name)
{
    auto &registry = functionRegistry();
    auto it = registry.find(registryKey(moduleKey, name));
    return it == registry.cend() ? nullptr : it->second;
}

extern "C"
{
    LIBSCRATCHCPP_EXPORT void cpp_register_function(uint64_t moduleKey, const char *name, void *function)
    {
        functionRegistry()[registryKey(moduleKey, name)] = function;
    }

    LIBSCRATCHCPP_EXPORT void cpp_mark_thread_as_finished(ExecutionContext *ctx)
    {
        assert(dynamic_cast<CppExecutionContext *>(ctx));
        static_cast<CppExecutionContext *>(ctx)->setFinished(true);
    }

    LIBSCRATCHCPP_EXPORT bool cpp_is_thread_finished(ExecutionContext *ctx)
    {
        assert(dynamic_cast<CppExecutionContext *>(ctx));
        return static_cast<CppExecutionContext *>(ctx)->finished();
    }
}
