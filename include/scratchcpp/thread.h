// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class VirtualMachine;
class Target;
class IEngine;
class Script;
class ThreadPrivate;

/*! \brief The Thread class represents a running Scratch script. */
class LIBSCRATCHCPP_EXPORT Thread
{
    public:
        Thread(Target *target, IEngine *engine, Script *script);
        Thread(const Thread &) = delete;

        VirtualMachine *vm() const;
        Target *target() const;
        IEngine *engine() const;
        Script *script() const;

        void run();
        void kill();
        void reset();

        bool isFinished() const;

        void promise();
        void resolvePromise();

    private:
        spimpl::unique_impl_ptr<ThreadPrivate> impl;
};

} // namespace libscratchcpp