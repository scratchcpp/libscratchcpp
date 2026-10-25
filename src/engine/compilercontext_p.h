// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_set>
#include <string>

namespace libscratchcpp
{

class IEngine;
class Target;

struct CompilerContextPrivate
{
        CompilerContextPrivate(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants);

        IEngine *engine = nullptr;
        Target *target = nullptr;
        const std::unordered_set<std::string> unsafeConstants;
};

} // namespace libscratchcpp
