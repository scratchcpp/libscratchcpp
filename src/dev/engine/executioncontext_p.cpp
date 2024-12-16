// SPDX-License-Identifier: Apache-2.0

#include "executioncontext_p.h"

using namespace libscratchcpp;

ExecutionContextPrivate::ExecutionContextPrivate(Thread *thread) :
    thread(thread),
    defaultStackTimer(std::make_unique<StackTimer>()),
    stackTimer(defaultStackTimer.get())
{
}
