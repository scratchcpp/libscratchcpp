// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "compiler.h"

namespace libscratchcpp
{

class CompilerValuePrivate;

/*! \brief The CompilerValue class represents a local value in compiled code. */
class LIBSCRATCHCPP_EXPORT CompilerValue
{
    public:
        CompilerValue(Compiler::StaticType type);
        CompilerValue(const CompilerValue &) = delete;
        virtual ~CompilerValue() { }

        Compiler::StaticType type() const;
        void setType(Compiler::StaticType type);

        virtual bool isConst() const { return false; };

    private:
        spimpl::unique_impl_ptr<CompilerValuePrivate> impl;
};

} // namespace libscratchcpp
