// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class Target;

struct ExecutionContextPrivate
{
        ExecutionContextPrivate(Target *target);

        Target *target = nullptr;
};

} // namespace libscratchcpp
