// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/monitor.h>

#include "variableblocks.h"

using namespace libscratchcpp;

std::string VariableBlocks::name() const
{
    return "Variables";
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "data_variable", &compileVariable);
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariable);
    engine->addCompileFunction(this, "data_changevariableby", &compileChangeVariableBy);
    engine->addCompileFunction(this, "data_showvariable", &compileShowVariable);
    engine->addCompileFunction(this, "data_hidevariable", &compileHideVariable);

    // Monitor names
    engine->addMonitorNameFunction(this, "data_variable", &variableMonitorName);

    // Monitor change functions
    engine->addMonitorChangeFunction(this, "data_variable", &changeVariableMonitorValue);

    // Inputs
    engine->addInput(this, "VALUE", VALUE);

    // Fields
    engine->addField(this, "VARIABLE", VARIABLE);
}

void VariableBlocks::compileVariable(Compiler *compiler)
{
    // NOTE: This block is only used by variable monitors
    compiler->addInstruction(vm::OP_READ_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}

void VariableBlocks::compileSetVariable(Compiler *compiler)
{
    compiler->addInput(VALUE);
    compiler->addInstruction(vm::OP_SET_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}

void VariableBlocks::compileChangeVariableBy(Compiler *compiler)
{
    compiler->addInput(VALUE);
    compiler->addInstruction(vm::OP_CHANGE_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}

void VariableBlocks::compileShowVariable(Compiler *compiler)
{
    Field *field = compiler->field(VARIABLE);
    assert(field);
    Variable *var = static_cast<Variable *>(field->valuePtr().get());
    assert(var);

    compiler->addConstValue(var->id());

    if (var->target() == static_cast<Target *>(compiler->engine()->stage()))
        compiler->addFunctionCall(&showGlobalVariable);
    else
        compiler->addFunctionCall(&showVariable);
}

void VariableBlocks::compileHideVariable(Compiler *compiler)
{
    Field *field = compiler->field(VARIABLE);
    assert(field);
    Variable *var = static_cast<Variable *>(field->valuePtr().get());
    assert(var);

    compiler->addConstValue(var->id());

    if (var->target() == static_cast<Target *>(compiler->engine()->stage()))
        compiler->addFunctionCall(&hideGlobalVariable);
    else
        compiler->addFunctionCall(&hideVariable);
}

void VariableBlocks::setVarVisible(std::shared_ptr<Variable> var, bool visible)
{
    if (var) {
        assert(var->monitor());
        var->monitor()->setVisible(visible);
    }
}

unsigned int VariableBlocks::showGlobalVariable(VirtualMachine *vm)
{
    if (Stage *target = vm->engine()->stage()) {
        int index = target->findVariableById(vm->getInput(0, 1)->toString());
        setVarVisible(target->variableAt(index), true);
    }

    return 1;
}

unsigned int VariableBlocks::showVariable(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        int index = target->findVariableById(vm->getInput(0, 1)->toString());
        setVarVisible(target->variableAt(index), true);
    }

    return 1;
}

unsigned int VariableBlocks::hideGlobalVariable(VirtualMachine *vm)
{
    if (Stage *target = vm->engine()->stage()) {
        int index = target->findVariableById(vm->getInput(0, 1)->toString());
        setVarVisible(target->variableAt(index), false);
    }

    return 1;
}

unsigned int VariableBlocks::hideVariable(VirtualMachine *vm)
{
    if (Target *target = vm->target()) {
        int index = target->findVariableById(vm->getInput(0, 1)->toString());
        setVarVisible(target->variableAt(index), false);
    }

    return 1;
}

const std::string &VariableBlocks::variableMonitorName(Block *block)
{
    Variable *var = dynamic_cast<Variable *>(block->findFieldById(VARIABLE)->valuePtr().get());

    if (var)
        return var->name();
    else {
        static const std::string empty = "";
        return empty;
    }
}

void VariableBlocks::changeVariableMonitorValue(Block *block, const Value &newValue)
{
    Variable *var = dynamic_cast<Variable *>(block->findFieldById(VARIABLE)->valuePtr().get());

    if (var)
        var->setValue(newValue);
}
