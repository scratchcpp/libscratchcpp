// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerlocalvariable.h>
#include <scratchcpp/compilervalue.h>

#include "compilerlocalvariable_p.h"

using namespace libscratchcpp;

CompilerLocalVariable::CompilerLocalVariable(CompilerValue *ptr) :
    impl(spimpl::make_unique_impl<CompilerLocalVariablePrivate>(ptr))
{
}

CompilerValue *CompilerLocalVariable::ptr() const
{
    return impl->ptr;
}

Compiler::StaticType CompilerLocalVariable::type() const
{
    return impl->ptr->type();
}
