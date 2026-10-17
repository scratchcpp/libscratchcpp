// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilerconstant.h>

namespace libscratchcpp
{

/*!
 * \brief The CppRegister struct represents a value in generated C++ code.
 * The C++ type of the expression depends on the static type:
 * - Number: double
 * - Bool: bool
 * - String: StringPtr *
 * - Pointer: void *
 * - Unknown (or any combination): ValueData (lvalue)
 */
struct CppRegister : public virtual CompilerValue
{
        CppRegister(Compiler::StaticType type, const std::string &expr) :
            CompilerValue(type),
            expr(expr)
        {
        }

        std::string expr;
};

struct CppConstantRegister
    : public CompilerConstant
    , public CppRegister
{
        CppConstantRegister(Compiler::StaticType type, const Value &value, const std::string &expr) :
            CompilerValue(type),
            CompilerConstant(type, value),
            CppRegister(type, expr)
        {
        }
};

} // namespace libscratchcpp
