// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class Thread;
class Target;
class Promise;

struct ExecutionContextPrivate
{
        ExecutionContextPrivate(Thread *thread);

        Thread *thread = nullptr;
        std::shared_ptr<Promise> promise;
};

} // namespace libscratchcpp
