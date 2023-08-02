// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>

#include "operatorblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

std::string OperatorBlocks::name() const
{
    return "Operators";
}

void OperatorBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "operator_add", &compileAdd);
    engine->addCompileFunction(this, "operator_subtract", &compileSubtract);
    engine->addCompileFunction(this, "operator_multiply", &compileMultiply);
    engine->addCompileFunction(this, "operator_divide", &compileDivide);
    engine->addCompileFunction(this, "operator_random", &compilePickRandom);
    engine->addCompileFunction(this, "operator_lt", &compileLessThan);
    engine->addCompileFunction(this, "operator_equals", &compileEquals);
    engine->addCompileFunction(this, "operator_gt", &compileGreaterThan);
    engine->addCompileFunction(this, "operator_and", &compileAnd);
    engine->addCompileFunction(this, "operator_or", &compileOr);
    engine->addCompileFunction(this, "operator_not", &compileNot);
    engine->addCompileFunction(this, "operator_join", &compileJoin);
    engine->addCompileFunction(this, "operator_letter_of", &compileLetterOf);
    engine->addCompileFunction(this, "operator_length", &compileLength);
    engine->addCompileFunction(this, "operator_contains", &compileContains);
    engine->addCompileFunction(this, "operator_mod", &compileMod);
    engine->addCompileFunction(this, "operator_round", &compileRound);
    engine->addCompileFunction(this, "operator_mathop", &compileMathOp);

    // Inputs
    engine->addInput(this, "NUM1", NUM1);
    engine->addInput(this, "NUM2", NUM2);
    engine->addInput(this, "FROM", FROM);
    engine->addInput(this, "TO", TO);
    engine->addInput(this, "OPERAND1", OPERAND1);
    engine->addInput(this, "OPERAND2", OPERAND2);
    engine->addInput(this, "OPERAND", OPERAND);
    engine->addInput(this, "STRING1", STRING1);
    engine->addInput(this, "STRING2", STRING2);
    engine->addInput(this, "LETTER", LETTER);
    engine->addInput(this, "STRING", STRING);
    engine->addInput(this, "NUM", NUM);

    // Fields
    engine->addField(this, "OPERATOR", OPERATOR);

    // Field values
    engine->addFieldValue(this, "abs", Abs);
    engine->addFieldValue(this, "floor", Floor);
    engine->addFieldValue(this, "ceiling", Ceiling);
    engine->addFieldValue(this, "sqrt", Sqrt);
    engine->addFieldValue(this, "sin", Sin);
    engine->addFieldValue(this, "cos", Cos);
    engine->addFieldValue(this, "tan", Tan);
    engine->addFieldValue(this, "asin", Asin);
    engine->addFieldValue(this, "acos", Acos);
    engine->addFieldValue(this, "atan", Atan);
    engine->addFieldValue(this, "ln", Ln);
    engine->addFieldValue(this, "log", Log);
    engine->addFieldValue(this, "e ^", Eexp);
    engine->addFieldValue(this, "10 ^", Op_10exp);
}

void OperatorBlocks::compileAdd(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_ADD);
}

void OperatorBlocks::compileSubtract(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_SUBTRACT);
}

void OperatorBlocks::compileMultiply(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_MULTIPLY);
}

void OperatorBlocks::compileDivide(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_DIVIDE);
}

void OperatorBlocks::compilePickRandom(Compiler *compiler)
{
    compiler->addInput(FROM);
    compiler->addInput(TO);
    compiler->addInstruction(vm::OP_RANDOM);
}

void OperatorBlocks::compileLessThan(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_LESS_THAN);
}

void OperatorBlocks::compileEquals(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_EQUALS);
}

void OperatorBlocks::compileGreaterThan(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_GREATER_THAN);
}

void OperatorBlocks::compileAnd(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_AND);
}

void OperatorBlocks::compileOr(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_OR);
}

void OperatorBlocks::compileNot(Compiler *compiler)
{
    compiler->addInput(OPERAND);
    compiler->addInstruction(vm::OP_NOT);
}

void OperatorBlocks::compileJoin(Compiler *compiler)
{
    compiler->addInput(STRING1);
    compiler->addInput(STRING2);
    compiler->addInstruction(vm::OP_STR_CONCAT);
}

void OperatorBlocks::compileLetterOf(Compiler *compiler)
{
    compiler->addInput(STRING);
    compiler->addInput(LETTER);
    compiler->addInstruction(vm::OP_STR_AT);
}

void OperatorBlocks::compileLength(Compiler *compiler)
{
    compiler->addInput(STRING);
    compiler->addInstruction(vm::OP_STR_LENGTH);
}

void OperatorBlocks::compileContains(Compiler *compiler)
{
    compiler->addInput(STRING1);
    compiler->addInput(STRING2);
    compiler->addInstruction(vm::OP_STR_CONTAINS);
}

void OperatorBlocks::compileMod(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_MOD);
}

void OperatorBlocks::compileRound(Compiler *compiler)
{
    compiler->addInput(NUM);
    compiler->addInstruction(vm::OP_ROUND);
}

void OperatorBlocks::compileMathOp(Compiler *compiler)
{
    compiler->addInput(NUM);

    int id = compiler->field(OPERATOR)->specialValueId();
    switch (id) {
        case Abs:
            compiler->addInstruction(vm::OP_ABS);
            break;

        case Floor:
            compiler->addInstruction(vm::OP_FLOOR);
            break;

        case Ceiling:
            compiler->addInstruction(vm::OP_CEIL);
            break;

        case Sqrt:
            compiler->addInstruction(vm::OP_SQRT);
            break;

        case Sin:
            compiler->addInstruction(vm::OP_SIN);
            break;

        case Cos:
            compiler->addInstruction(vm::OP_COS);
            break;

        case Tan:
            compiler->addInstruction(vm::OP_TAN);
            break;

        case Asin:
            compiler->addInstruction(vm::OP_ASIN);
            break;

        case Acos:
            compiler->addInstruction(vm::OP_ACOS);
            break;

        case Atan:
            compiler->addInstruction(vm::OP_ATAN);
            break;

        case Ln:
            compiler->addFunctionCall(&op_ln);
            break;

        case Log:
            compiler->addFunctionCall(&op_log);
            break;

        case Eexp:
            compiler->addFunctionCall(&op_eexp);
            break;

        case Op_10exp:
            compiler->addFunctionCall(&op_10exp);
            break;

        default:
            break;
    }
}

unsigned int OperatorBlocks::op_ln(VirtualMachine *vm)
{
    const Value &v = *vm->getInput(0, 1);
    if (v < 0)
        vm->replaceReturnValue(Value(Value::SpecialValue::NaN), 1);
    else if (v == 0)
        vm->replaceReturnValue(Value(Value::SpecialValue::NegativeInfinity), 1);
    else if (!v.isInfinity())
        vm->replaceReturnValue(std::log(v.toDouble()), 1);
    return 0;
}

unsigned int OperatorBlocks::op_log(VirtualMachine *vm)
{
    const Value &v = *vm->getInput(0, 1);
    if (v < 0)
        vm->replaceReturnValue(Value(Value::SpecialValue::NaN), 1);
    else if (v == 0)
        vm->replaceReturnValue(Value(Value::SpecialValue::NegativeInfinity), 1);
    else if (!v.isInfinity())
        vm->replaceReturnValue(std::log10(v.toDouble()), 1);
    return 0;
}

unsigned int OperatorBlocks::op_eexp(VirtualMachine *vm)
{
    const Value *v = vm->getInput(0, 1);
    if (v->isNegativeInfinity())
        vm->replaceReturnValue(0, 1);
    else if (!v->isInfinity())
        vm->replaceReturnValue(std::exp(v->toDouble()), 1);
    return 0;
}

unsigned int OperatorBlocks::op_10exp(VirtualMachine *vm)
{
    const Value *v = vm->getInput(0, 1);
    if (v->isNegativeInfinity())
        vm->replaceReturnValue(0, 1);
    else if (!v->isInfinity())
        vm->replaceReturnValue(std::pow(10, v->toDouble()), 1);
    return 0;
}
