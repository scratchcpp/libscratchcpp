// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <scratchcpp/value.h>

#include "spimpl.h"

namespace libscratchcpp
{

class VirtualMachinePrivate;

/*! \brief A namespace for global variables and enumerations of the VirtualMachine. */
namespace vm
{

enum Opcode
{
    OP_START,              /*!< The first instruction of every bytecode. */
    OP_HALT,               /*!< The last instruction of every bytecode. */
    OP_CONST,              /*!< Adds a constant value with the index in the argument to the next register. */
    OP_NULL,               /*!< Adds a null (zero) value to the next register. */
    OP_CHECKPOINT,         /*!< A checkpoint for the VirtualMachine::moveToLastCheckpoint() method. */
    OP_IF,                 /*!< Jumps to the next instruction if the last register holds "true". If it's false, jumps to OP_ELSE or OP_ENDIF. */
    OP_ELSE,               /*!< Jumps to OP_ENDIF. This instruction is typically reached when the if statement condition was "true". */
    OP_ENDIF,              /*!< Doesn't do anything, but is used by OP_IF and OP_ELSE. */
    OP_FOREVER_LOOP,       /*!< Runs a forever loop. */
    OP_REPEAT_LOOP,        /*!< Runs a repeat loop with the number of periods stored in the last register. */
    OP_REPEAT_LOOP_INDEX,  /*!< Returns the index of current repeat loop. */
    OP_REPEAT_LOOP_INDEX1, /*!< Returns the index of current repeat loop plus 1. */
    OP_UNTIL_LOOP,         /*!< Evaluates the condition before OP_BEGIN_UNTIL_LOOP and runs a repeat until loop. */
    OP_BEGIN_UNTIL_LOOP,   /*!< Used by OP_UNTIL_LOOP. */
    OP_LOOP_END,           /*!< Ends the loop. */
    OP_PRINT,              /*!< Prints the value stored in the last register. */
    OP_ADD,                /*!< Adds the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_SUBTRACT,           /*!< Subtracts the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_MULTIPLY,           /*!< Multiplies the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_DIVIDE,             /*!< Divides the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_MOD,                /*!< Calculates module of the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_RANDOM,             /*!< Generates a random value in the range stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_ROUND,              /*!< Rounds the number in the last register and stores the result in the last register. */
    OP_ABS,                /*!< Calculates the absolute value of the number in the last register and stores the result in the last register. */
    OP_FLOOR,              /*!< Calculates the floor of the number in the last register and stores the result in the last register. */
    OP_CEIL,               /*!< Calculates the ceiling of the number in the last register and stores the result in the last register. */
    OP_SQRT,               /*!< Calculates the square root of the number in the last register and stores the result in the last register. */
    OP_SIN,                /*!< Calculates the sine of the number in the last register and stores the result in the last register. */
    OP_COS,                /*!< Calculates the cosine of the number in the last register and stores the result in the last register. */
    OP_TAN,                /*!< Calculates the tangent of the number in the last register and stores the result in the last register. */
    OP_ASIN,               /*!< Calculates the arcsine of the number in the last register and stores the result in the last register. */
    OP_ACOS,               /*!< Calculates the arccosine of the number in the last register and stores the result in the last register. */
    OP_ATAN,               /*!< Calculates the arctangent of the number in the last register and stores the result in the last register. */
    OP_GREATER_THAN,       /*!< Compares (>) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_LESS_THAN,          /*!< Compares (<) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_EQUALS,             /*!< Compares (==) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_AND,                /*!< AND gate. The result is stored in the last registry and the input registers are deleted. */
    OP_OR,                 /*!< OR gate. The result is stored in the last registry and the input registers are deleted. */
    OP_NOT,                /*!< NOT gate. The result is stored in the last registry and the input registers are deleted. */
    OP_SET_VAR,            /*!< Sets the variable with the index in the argument to the value stored in the last register. */
    OP_CHANGE_VAR,         /*!< Increments (or decrements) the variable with the index in the argument by the value stored in the last register. */
    OP_READ_VAR,           /*!< Reads the variable with the index in the argument and stores the value in the last register. */
    OP_READ_LIST,          /*!< Converts the list with the index in the argument to a string and stores the result in the last register. */
    OP_LIST_APPEND,        /*!< Appens the value stored in the last register to the list with the index in the argument. */
    OP_LIST_DEL,           /*!< Deletes the index (or item like "last" or "random") stored in the last register of the list with the index in the argument. */
    OP_LIST_DEL_ALL,       /*!< Clears the list with the index in the argument. */
    OP_LIST_INSERT,    /*!< Inserts the value from the second last register at the index (or item like "last" or "random") stored in the last register to the list with the index in the argument. */
    OP_LIST_REPLACE,   /*!< Replaces the index (or item like "last" or "random") stored in the second last register with the value from the last register in the list with the index in the argument. */
    OP_LIST_GET_ITEM,  /*!< Stores the value at the index (or item like "last" or "random") (of the list with the index in the argument) stored in the last register, in the last register. */
    OP_LIST_INDEX_OF,  /*!< Stores the index of the value from the last register in the last register (of the list with the index in the argument). */
    OP_LIST_LENGTH,    /*!< Stores the length of the list with the index in the argument, in the last register. */
    OP_LIST_CONTAINS,  /*!< Stores true in the last register if the list with the index in the argument contains the value from the last register. */
    OP_STR_CONCAT,     /*!< Concatenates the strings stored in the last 2 registers and stores the result in the last register, deleting the input registers. */
    OP_STR_AT,         /*! Stores the character at index in the last register of the string in the second last register, in the last register. */
    OP_STR_LENGTH,     /*! Stores the length of the string in the last register, in the last register. */
    OP_STR_CONTAINS,   /*! Stores true in the last register if the string stored in the second last register contains the substring in the last register. */
    OP_EXEC,           /*!< Calls the function with the index in the argument. */
    OP_INIT_PROCEDURE, /*!< Initializes the list of procedure (custom block) arguments. */
    OP_CALL_PROCEDURE, /*! Calls the procedure (custom block) with the index in the argument. */
    OP_ADD_ARG,        /*!< Adds a procedure (custom block) argument with the value from the last register. */
    OP_READ_ARG,       /*!< Reads the procedure (custom block) argument with the index in the argument and stores the value in the last register. */
    OP_BREAK_FRAME,    /*!< Breaks current frame at the end of the loop. */
    OP_WARP            /*! Runs the script without screen refresh. */
};

}

class Target;
class IEngine;
class Script;
class List;

/*! \brief The VirtualMachine class is a virtual machine for compiled Scratch scripts. */
class LIBSCRATCHCPP_EXPORT VirtualMachine
{
    public:
        VirtualMachine();
        VirtualMachine(Target *target, IEngine *engine, Script *script);
        VirtualMachine(const VirtualMachine &) = delete;

        void setProcedures(unsigned int **procedures);
        void setFunctions(BlockFunc *functions);
        void setConstValues(const Value *values);
        void setVariables(Value **variables);
        void setLists(List **lists);
        void setVariablesVector(const std::vector<Value *> &variables);
        void setListsVector(const std::vector<List *> &lists);

        void setBytecode(unsigned int *code);

        unsigned int **procedures() const;
        const BlockFunc *functions() const;
        const Value *constValues() const;
        Value **variables() const;
        List **lists() const;

        unsigned int *bytecode() const;

        size_t registerCount() const;

        Target *target() const;
        IEngine *engine() const;
        Script *script() const;

        const Value *getInput(unsigned int index, unsigned int argCount) const;

        void addReturnValue(const Value &v);
        void replaceReturnValue(const Value &v, unsigned int offset);

        void run();
        void reset();
        void moveToLastCheckpoint();

        void stop(bool savePos = true, bool breakFrame = false, bool goBack = false);

        bool atEnd() const;

        bool savePos() const;

    private:
        spimpl::unique_impl_ptr<VirtualMachinePrivate> impl;
};

} // namespace libscratchcpp
