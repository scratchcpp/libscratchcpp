// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/global.h>
#include <string>
#include <cstdint>

namespace libscratchcpp
{

class ExecutionContext;

void *cppLookupFunction(uint64_t moduleKey, const std::string &name);

extern "C"
{
    LIBSCRATCHCPP_EXPORT void cpp_register_function(uint64_t moduleKey, const char *name, void *function);
    LIBSCRATCHCPP_EXPORT void cpp_mark_thread_as_finished(ExecutionContext *ctx);
    LIBSCRATCHCPP_EXPORT bool cpp_is_thread_finished(ExecutionContext *ctx);

    // Makes the compiler use the C++ backend (used by generated codebases)
    LIBSCRATCHCPP_EXPORT void cpp_use_cpp_backend();

    // Generates a standalone C++ codebase from a Scratch project
    LIBSCRATCHCPP_EXPORT bool cpp_generate_codebase(const char *projectFile, const char *outputDir, const char *projectName);
}

} // namespace libscratchcpp
