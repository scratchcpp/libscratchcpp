// SPDX-License-Identifier: Apache-2.0

#include "operatorblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

OperatorBlocks::OperatorBlocks()
{
    // Blocks
    addCompileFunction("operator_add", &compileAdd);
    addCompileFunction("operator_subtract", &compileSubtract);
    addCompileFunction("operator_multiply", &compileMultiply);
    addCompileFunction("operator_divide", &compileDivide);
    addCompileFunction("operator_random", &compilePickRandom);
    addCompileFunction("operator_lt", &compileLessThan);
    addCompileFunction("operator_equals", &compileEquals);
    addCompileFunction("operator_gt", &compileGreaterThan);
    addCompileFunction("operator_and", &compileAnd);
    addCompileFunction("operator_or", &compileOr);
    addCompileFunction("operator_not", &compileNot);
    addCompileFunction("operator_join", &compileJoin);
    addCompileFunction("operator_letter_of", &compileLetterOf);
    addCompileFunction("operator_length", &compileLength);
    addCompileFunction("operator_contains", &compileContains);
    addCompileFunction("operator_mod", &compileMod);
    addCompileFunction("operator_round", &compileRound);
    addCompileFunction("operator_mathop", &compileMathOp);

    // Inputs
    addInput("NUM1", NUM1);
    addInput("NUM2", NUM2);
    addInput("FROM", FROM);
    addInput("TO", TO);
    addInput("OPERAND1", OPERAND1);
    addInput("OPERAND2", OPERAND2);
    addInput("OPERAND", OPERAND);
    addInput("STRING1", STRING1);
    addInput("STRING2", STRING2);
    addInput("LETTER", LETTER);
    addInput("STRING", STRING);
    addInput("NUM", NUM);

    // Fields
    addField("OPERATOR", OPERATOR);

    // Field values
    addFieldValue("abs", Abs);
    addFieldValue("floor", Floor);
    addFieldValue("ceiling", Ceiling);
    addFieldValue("sqrt", Sqrt);
    addFieldValue("sin", Sin);
    addFieldValue("cos", Cos);
    addFieldValue("tan", Tan);
    addFieldValue("asin", Asin);
    addFieldValue("acos", Acos);
    addFieldValue("atan", Atan);
    addFieldValue("ln", Ln);
    addFieldValue("log", Log);
    addFieldValue("e ^", Eexp);
    addFieldValue("10 ^", Op_10exp);
}

std::string OperatorBlocks::name() const
{
    return "Operators";
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
