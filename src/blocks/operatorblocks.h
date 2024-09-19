// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class Compiler;
class VirtualMachine;

/*! \brief The OperatorBlocks class contains the implementation of operator blocks. */
class OperatorBlocks : public IExtension
{
    public:
        enum Inputs
        {
            NUM1,
            NUM2,
            FROM,
            TO,
            OPERAND1,
            OPERAND2,
            OPERAND,
            STRING1,
            STRING2,
            LETTER,
            STRING,
            NUM
        };

        enum Fields
        {
            OPERATOR
        };

        enum FieldValues
        {
            Abs,
            Floor,
            Ceiling,
            Sqrt,
            Sin,
            Cos,
            Tan,
            Asin,
            Acos,
            Atan,
            Ln,
            Log,
            Eexp,
            Op_10exp
        };

        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileAdd(Compiler *compiler);
        static void compileSubtract(Compiler *compiler);
        static void compileMultiply(Compiler *compiler);
        static void compileDivide(Compiler *compiler);
        static void compilePickRandom(Compiler *compiler);
        static void compileLessThan(Compiler *compiler);
        static void compileEquals(Compiler *compiler);
        static void compileGreaterThan(Compiler *compiler);
        static void compileAnd(Compiler *compiler);
        static void compileOr(Compiler *compiler);
        static void compileNot(Compiler *compiler);
        static void compileJoin(Compiler *compiler);
        static void compileLetterOf(Compiler *compiler);
        static void compileLength(Compiler *compiler);
        static void compileContains(Compiler *compiler);
        static void compileMod(Compiler *compiler);
        static void compileRound(Compiler *compiler);
        static void compileMathOp(Compiler *compiler);

        static unsigned int op_ln(VirtualMachine *vm);
        static unsigned int op_log(VirtualMachine *vm);
        static unsigned int op_eexp(VirtualMachine *vm);
        static unsigned int op_10exp(VirtualMachine *vm);
};

} // namespace libscratchcpp
