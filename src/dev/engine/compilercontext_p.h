// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class IEngine;
class Target;

struct CompilerContextPrivate
{
        CompilerContextPrivate(IEngine *engine, Target *target);

        IEngine *engine = nullptr;
        Target *target = nullptr;
};

} // namespace libscratchcpp
