// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../scratch/runningscript.h"

namespace libscratchcpp
{

/*! \brief A namespace for global variables and enumerations of the VirtualMachine. */
namespace vm
{

enum Opcode
{
    OP_START,            /*!< The first instruction of every bytecode. */
    OP_HALT,             /*!< The last instruction of every bytecode. */
    OP_CONST,            /*!< Adds a constant value with the index in the argument to the next register. */
    OP_NULL,             /*!< Adds a null (zero) value to the next register. */
    OP_IF,               /*!< Jumps to the next instruction if the last register holds "true". If it's false, jumps to OP_ELSE or OP_ENDIF. */
    OP_ELSE,             /*!< Jumps to OP_ENDIF. This instruction is typically reached when the if statement condition was "true". */
    OP_ENDIF,            /*!< Doesn't do anything, but is used by OP_IF and OP_ELSE. */
    OP_FOREVER_LOOP,     /*!< Runs a forever loop. */
    OP_REPEAT_LOOP,      /*!< Runs a repeat loop with the number of periods stored in the last register. */
    OP_UNTIL_LOOP,       /*!< Evaluates the condition before OP_BEGIN_UNTIL_LOOP and runs a repeat until loop. */
    OP_BEGIN_UNTIL_LOOP, /*!< Used by OP_UNTIL_LOOP. */
    OP_LOOP_END,         /*!< Ends the loop. */
    OP_PRINT,            /*!< Prints the value stored in the last register. */
    OP_ADD,              /*!< Adds the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_SUBTRACT,         /*!< Subtracts the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_MULTIPLY,         /*!< Multiplies the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_DIVIDE,           /*!< Divides the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_RANDOM,           /*!< Generates a random value in the range stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_GREATER_THAN,     /*!< Compares (>) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_LESS_THAN,        /*!< Compares (<) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_EQUALS,           /*!< Compares (==) the values stored in the last 2 registers and stores the result in the last registry, deleting the input registers. */
    OP_AND,              /*!< AND gate. The result is stored in the last registry and the input registers are deleted. */
    OP_OR,               /*!< OR gate. The result is stored in the last registry and the input registers are deleted. */
    OP_NOT,              /*!< NOT gate. The result is stored in the last registry and the input registers are deleted. */
    OP_SET_VAR,          /*!< Sets the variable with the index in the argument to the value stored in the last register. */
    OP_CHANGE_VAR,       /*!< Increments (or decrements) the variable with the index in the argument by the value stored in the last register. */
    OP_READ_VAR,         /*!< Reads the variable with the index in the argument and stores the value in the last register. */
    OP_READ_LIST,        /*!< Converts the list with the index in the argument to a string and stores the result in the last register. */
    OP_LIST_APPEND,      /*!< Appens the value stored in the last register to the list with the index in the argument. */
    OP_LIST_DEL,         /*!< Deletes the index (or item like "last" or "random") stored in the last register of the list with the index in the argument. */
    OP_LIST_DEL_ALL,     /*!< Clears the list with the index in the argument. */
    OP_LIST_INSERT,      /*!< Inserts the value from the second last register at the index (or item like "last" or "random") stored in the last register to the list with the index in the argument. */
    OP_LIST_REPLACE,  /*!< Replaces the index (or item like "last" or "random") stored in the second last register with the value from the last register in the list with the index in the argument. */
    OP_LIST_GET_ITEM, /*!< Stores the value at the index (or item like "last" or "random") (of the list with the index in the argument) stored in the last register, in the last register. */
    OP_LIST_INDEX_OF, /*!< Stores the index of the value from the last register in the last register (of the list with the index in the argument). */
    OP_LIST_LENGTH,   /*!< Stores the length of the list with the index in the argument, in the last register. */
    OP_LIST_CONTAINS, /*!< Stores true in the last register if the list with the index in the argument contains the value from the last register. */
    OP_EXEC           /*!< Calls the function with the index in the argument. */
};

}

/*! \brief The VirtualMachine class is a virtual machine for compiled Scratch scripts. */
class LIBSCRATCHCPP_EXPORT VirtualMachine
{
    public:
        VirtualMachine();
        VirtualMachine(const VirtualMachine &) = delete;
        ~VirtualMachine();

        void setFunctions(const std::vector<BlockFunc> &functions);
        void setConstValues(const std::vector<Value> &values);
        void setVariables(const std::vector<Value *> &variables);
        void setLists(const std::vector<List *> &lists);
        void setBytecode(const std::vector<unsigned int> &code);

        const Value *getInput(unsigned int index, unsigned int argCount) const { return m_regs[m_regCount - argCount + index]; };
        unsigned int getFieldIndex(unsigned int index) const { return m_globalPos[index]; };

        void addReturnValue(const Value &v) { *m_regs[m_regCount++] = v; };
        void replaceReturnValue(const Value &v, unsigned int offset) { *m_regs[m_regCount - offset] = v; };

        unsigned int *run(RunningScript *script);

        /*! Returns true if the VM has reached the vm::OP_HALT instruction. */
        bool atEnd() const { return m_atEnd; };

    private:
        unsigned int *run(unsigned int *pos, RunningScript *script);

        static inline const unsigned int instruction_arg_count[] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

        unsigned int *m_bytecode;
        std::vector<unsigned int> m_bytecodeVector;

        Target *m_target = nullptr;
        Engine *m_engine = nullptr;
        RunningScript *m_script = nullptr;
        unsigned int *m_globalPos = nullptr;
        bool m_atEnd = false;

        BlockFunc *m_functions;
        std::vector<BlockFunc> m_functionsVector;

        const Value *m_constValues = nullptr;
        std::vector<Value> m_constValuesVector;

        Value **m_regs = nullptr;
        size_t m_regCount = 0;

        Value **m_variables = nullptr;
        std::vector<Value *> m_variablesVector;

        List **m_lists = nullptr;
        std::vector<List *> m_listsVector;
};

} // namespace libscratchcpp
