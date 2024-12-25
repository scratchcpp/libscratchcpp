// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>

#include "variableblocks.h"

using namespace libscratchcpp;

std::string VariableBlocks::name() const
{
    return "Variables";
}

std::string VariableBlocks::description() const
{
    return "Variable blocks";
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "data_variable", &compileVariable);
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariableTo);
}

CompilerValue *VariableBlocks::compileVariable(Compiler *compiler)
{
    Field *varField = compiler->field("VARIABLE");
    Variable *var = static_cast<Variable *>(varField->valuePtr().get());
    assert(var);

    if (var)
        return compiler->addVariableValue(var);
    else
        return compiler->addConstValue(Value());
}

CompilerValue *VariableBlocks::compileSetVariableTo(Compiler *compiler)
{
    Field *varField = compiler->field("VARIABLE");
    Variable *var = static_cast<Variable *>(varField->valuePtr().get());
    assert(var);

    if (var)
        compiler->createVariableWrite(var, compiler->addInput("VALUE"));

    return nullptr;
}
