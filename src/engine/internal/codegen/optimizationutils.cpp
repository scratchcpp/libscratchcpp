// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/compilercontext.h>
#include <scratchcpp/value.h>

#include "optimizationutils.h"

using namespace libscratchcpp;

Compiler::StaticType OptimizationUtils::optimizeValueType(CompilerContext *ctx, const CompilerValue *value)
{
    Compiler::StaticType ret = value->type();

    // Optimize string constants that represent numbers
    if (value->isConst() && value->type() == Compiler::StaticType::String) {
        const Value &constValue = dynamic_cast<const CompilerConstant *>(value)->value();
        Value numberValue(constValue.toDouble());
        std::string str = constValue.toString();

        // Apply this optimization only if the number matches the string and the constant is safe
        const auto &unsafeConstants = ctx->unsafeConstants();

        if (constValue.isValidNumber() && numberValue.toString() == str && unsafeConstants.find(str) == unsafeConstants.cend())
            ret = Compiler::StaticType::Number;
    }

    return ret;
}
