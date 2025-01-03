// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../global.h"
#include "../spimpl.h"

namespace libscratchcpp
{

class IEngine;
class Target;
class CompilerContextPrivate;

/*! \brief The CompilerContext represents a context for a specific target which is used with the Compiler class. */
class LIBSCRATCHCPP_EXPORT CompilerContext
{
    public:
        CompilerContext(IEngine *engine, Target *target);
        CompilerContext(const CompilerContext &) = delete;
        virtual ~CompilerContext() { }

        IEngine *engine() const;
        Target *target() const;

        /*!
         * Optimizes compiled scripts ahead of time.
         * \see Compiler#preoptimize()
         */
        virtual void preoptimize() { }

    private:
        spimpl::unique_impl_ptr<CompilerContextPrivate> impl;
};

} // namespace libscratchcpp
