// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

namespace libscratchcpp
{

struct LLVMLoopScope
{
        bool containsYield = false;
        LLVMLoopScope *parentScope = nullptr;
        std::vector<LLVMLoopScope *> childScopes;
};

} // namespace libscratchcpp
