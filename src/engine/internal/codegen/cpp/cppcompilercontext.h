// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilercontext.h>

namespace libscratchcpp
{

class CppCompilerContext : public CompilerContext
{
    public:
        CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants = {});
};

} // namespace libscratchcpp
