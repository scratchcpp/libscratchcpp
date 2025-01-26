// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
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

Rgb VariableBlocks::color() const
{
    return rgb(255, 140, 26);
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "data_variable", &compileVariable);
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariableTo);
    engine->addCompileFunction(this, "data_changevariableby", &compileChangeVariableBy);
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

CompilerValue *VariableBlocks::compileChangeVariableBy(Compiler *compiler)
{
    Field *varField = compiler->field("VARIABLE");
    Variable *var = static_cast<Variable *>(varField->valuePtr().get());
    assert(var);

    if (var) {
        CompilerValue *value = compiler->createAdd(compiler->addVariableValue(var), compiler->addInput("VALUE"));
        compiler->createVariableWrite(var, value);
    }

    return nullptr;
}
