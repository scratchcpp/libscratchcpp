// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilercontext.h>

#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT CppCompilerContext : public CompilerContext
{
    public:
        CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants = {});
};

} // namespace libscratchcpp
