// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "../../engine/internal/stacktimer.h"

namespace libscratchcpp
{

class Thread;
class Promise;

struct ExecutionContextPrivate
{
        ExecutionContextPrivate(Thread *thread);

        Thread *thread = nullptr;
        std::shared_ptr<Promise> promise;
        std::unique_ptr<IStackTimer> defaultStackTimer;
        IStackTimer *stackTimer = nullptr;
};

} // namespace libscratchcpp
