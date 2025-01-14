// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerconstant.h>

#include "compilerconstant_p.h"

using namespace libscratchcpp;

/*! Constructs CompilerConstant. */
CompilerConstant::CompilerConstant(Compiler::StaticType type, const Value &value) :
    CompilerValue(type),
    impl(spimpl::make_unique_impl<CompilerConstantPrivate>(value))
{
}

/*! Returns the constant value. */
const Value &CompilerConstant::value() const
{
    return impl->value;
}
