// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/virtualmachine.h>

namespace libscratchcpp
{

#ifdef USE_LLVM
class ExecutableCode;
class ExecutionContext;
#endif

struct ThreadPrivate
{
        ThreadPrivate(Target *target, IEngine *engine, Script *script);

        std::unique_ptr<VirtualMachine> vm;
        Target *target = nullptr;
        IEngine *engine = nullptr;
        Script *script = nullptr;
#ifdef USE_LLVM
        ExecutableCode *code = nullptr;
        std::shared_ptr<ExecutionContext> executionContext;
#endif
};

} // namespace libscratchcpp
