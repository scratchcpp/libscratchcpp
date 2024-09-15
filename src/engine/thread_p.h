// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/virtualmachine.h>

namespace libscratchcpp
{

struct ThreadPrivate
{
        ThreadPrivate(Target *target, IEngine *engine, Script *script);

        std::unique_ptr<VirtualMachine> vm;
        Target *target = nullptr;
        IEngine *engine = nullptr;
        Script *script = nullptr;
};

} // namespace libscratchcpp
