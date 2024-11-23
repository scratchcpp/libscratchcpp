// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "compilervalue.h"

namespace libscratchcpp
{

class CompilerConstantPrivate;

/*! \brief The CompilerConstant class represents a constant value in compiled code. */
class LIBSCRATCHCPP_EXPORT CompilerConstant : public CompilerValue
{
    public:
        CompilerConstant(Compiler::StaticType type, const Value &value);
        CompilerConstant(const CompilerConstant &) = delete;

        bool isConst() const override final { return true; };

        const Value &value() const;

    private:
        spimpl::unique_impl_ptr<CompilerConstantPrivate> impl;
};

} // namespace libscratchcpp
