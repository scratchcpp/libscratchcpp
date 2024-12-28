// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "compiler.h"

namespace libscratchcpp
{

class CompilerLocalVariablePrivate;

/*! \brief The CompilerLocalVariable class represents a statically typed local variable in compiled code. */
class LIBSCRATCHCPP_EXPORT CompilerLocalVariable
{
    public:
        CompilerLocalVariable(CompilerValue *ptr);
        CompilerLocalVariable(const CompilerLocalVariable &) = delete;

        CompilerValue *ptr() const;
        Compiler::StaticType type() const;

    private:
        spimpl::unique_impl_ptr<CompilerLocalVariablePrivate> impl;
};

} // namespace libscratchcpp
