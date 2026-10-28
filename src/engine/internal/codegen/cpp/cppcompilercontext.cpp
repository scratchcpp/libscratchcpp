// SPDX-License-Identifier: Apache-2.0

#include "cppcompilercontext.h"

using namespace libscratchcpp;

CppCompilerContext::CppCompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants) :
    CompilerContext(engine, target, unsafeConstants)
{
}
