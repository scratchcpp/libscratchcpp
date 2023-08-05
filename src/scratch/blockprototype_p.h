// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/blockprototype.h>

namespace libscratchcpp
{

struct BlockPrototypePrivate
{
        BlockPrototypePrivate();
        BlockPrototypePrivate(const std::string &procCode);

        void setProcCode(const std::string &newProcCode);

        std::string procCode;
        std::vector<std::string> argumentIds;
        std::vector<std::string> argumentNames;
        std::vector<Value> argumentDefaults;
        std::vector<BlockPrototype::ArgType> argumentTypes;
        bool warp = false;
};

} // namespace libscratchcpp
