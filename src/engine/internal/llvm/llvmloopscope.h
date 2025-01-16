// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

namespace libscratchcpp
{

struct LLVMLoopScope
{
        bool containsYield = false;
        std::shared_ptr<LLVMLoopScope> parentScope;
        std::vector<std::shared_ptr<LLVMLoopScope>> childScopes;
};

} // namespace libscratchcpp
