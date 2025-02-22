// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/block.h>
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
    // Blocks
    engine->addCompileFunction(this, "data_variable", &compileVariable);
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariableTo);
    engine->addCompileFunction(this, "data_changevariableby", &compileChangeVariableBy);

    // Monitor names
    engine->addMonitorNameFunction(this, "data_variable", &variableMonitorName);

    // Monitor change functions
    engine->addMonitorChangeFunction(this, "data_variable", &changeVariableMonitorValue);
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

const std::string &VariableBlocks::variableMonitorName(Block *block)
{
    static const std::string empty = "";

    auto field = block->fieldAt(block->findField("VARIABLE"));

    if (!field)
        return empty;

    Variable *var = dynamic_cast<Variable *>(field->valuePtr().get());

    if (var)
        return var->name();
    else
        return empty;
}

void VariableBlocks::changeVariableMonitorValue(Block *block, const Value &newValue)
{
    auto field = block->fieldAt(block->findField("VARIABLE"));

    if (!field)
        return;

    Variable *var = dynamic_cast<Variable *>(field->valuePtr().get());

    if (var)
        var->setValue(newValue);
}
