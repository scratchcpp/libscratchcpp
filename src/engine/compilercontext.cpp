// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilercontext.h>

#include "compilercontext_p.h"

using namespace libscratchcpp;

/*! Constructs CompilerContext. */
CompilerContext::CompilerContext(IEngine *engine, Target *target, const std::unordered_set<std::string> &unsafeConstants) :
    impl(spimpl::make_unique_impl<CompilerContextPrivate>(engine, target, unsafeConstants))
{
}

/*! Returns the engine of the project. */
IEngine *CompilerContext::engine() const
{
    return impl->engine;
}

/*! Returns the target of this context. */
Target *CompilerContext::target() const
{
    return impl->target;
}
