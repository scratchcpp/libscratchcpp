// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilervalue.h>

#include "compilervalue_p.h"

using namespace libscratchcpp;

/*! Constructs CompilerValue. */
CompilerValue::CompilerValue(Compiler::StaticType type) :
    impl(spimpl::make_unique_impl<CompilerValuePrivate>(type))
{
}

/*! Returns the type of this value. */
Compiler::StaticType CompilerValue::type() const
{
    return impl->type;
}

/*! Sets the type of this value. */
void CompilerValue::setType(Compiler::StaticType type)
{
    impl->type = type;
}
