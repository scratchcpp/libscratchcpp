// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class Target;
class Promise;

struct ExecutionContextPrivate
{
        ExecutionContextPrivate(Target *target);

        Target *target = nullptr;
        std::shared_ptr<Promise> promise;
};

} // namespace libscratchcpp
