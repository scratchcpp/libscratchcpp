// SPDX-License-Identifier: Apache-2.0

#include "virtualmachine.h"

#define DISPATCH() goto *dispatch_table[*++pos]
#define FREE_REGS(count) m_regCount -= count
#define ADD_RET_VALUE(value) *m_regs[m_regCount++] = value
#define REPLACE_RET_VALUE(value, offset) *m_regs[m_regCount - offset] = value
#define GET_NEXT_ARG() m_constValues[*++pos]
#define READ_REG(index, count) m_regs[m_regCount - count + index]
#define READ_LAST_REG() m_regs[m_regCount - 1]

#define FIX_LIST_INDEX(index, listSize)                                                                                                                                                                \
    if ((listSize == 0) || (index < 1) || (index > listSize))                                                                                                                                          \
    index = 0

using namespace libscratchcpp;
using namespace vm;

/*! Constructs VirtualMachine. */
VirtualMachine::VirtualMachine()
{
    m_regs = new Value *[1024];
    for (int i = 0; i < 1024; i++)
        m_regs[i] = new Value();
}

/*! Destroys the VirtualMachine object. */
VirtualMachine::~VirtualMachine()
{
    for (int i = 0; i < 1024; i++)
        delete m_regs[i];
    delete m_regs;
}

/*! Sets the list of functions. */
void VirtualMachine::setFunctions(const std::vector<BlockFunc> &functions)
{
    m_functionsVector = functions;
    m_functions = m_functionsVector.data();
}

/*! Sets the list of constant values. */
void VirtualMachine::setConstValues(const std::vector<Value> &values)
{
    m_constValuesVector = values;
    m_constValues = m_constValuesVector.data();
}

/*! Sets the list of variables. */
void VirtualMachine::setVariables(const std::vector<Value *> &variables)
{
    m_variablesVector = variables;
    m_variables = m_variablesVector.data();
}

/*! Sets the list of lists. */
void VirtualMachine::setLists(const std::vector<List *> &lists)
{
    m_listsVector = lists;
    m_lists = m_listsVector.data();
}

/*! Sets the bytecode of the script. */
void VirtualMachine::setBytecode(const std::vector<unsigned int> &code)
{
    m_bytecodeVector = code;
    m_bytecode = m_bytecodeVector.data();
}

/*! Runs the script. */
unsigned int *VirtualMachine::run(RunningScript *script)
{
    return run(m_bytecode, script);
}

unsigned int *VirtualMachine::run(unsigned int *pos, RunningScript *script)
{
    static const void *dispatch_table[] = {
        nullptr,
        &&do_halt,
        &&do_const,
        &&do_null,
        &&do_if,
        &&do_else,
        &&do_endif,
        &&do_forever_loop,
        &&do_repeat_loop,
        &&do_until_loop,
        &&do_begin_until_loop,
        &&do_loop_end,
        &&do_print,
        &&do_add,
        &&do_subtract,
        &&do_multiply,
        &&do_divide,
        &&do_random,
        &&do_greater_than,
        &&do_less_than,
        &&do_equals,
        &&do_and,
        &&do_or,
        &&do_not,
        &&do_set_var,
        &&do_change_var,
        &&do_read_var,
        &&do_read_list,
        &&do_list_append,
        &&do_list_del,
        &&do_list_del_all,
        &&do_list_insert,
        &&do_list_replace,
        &&do_list_get_item,
        &&do_list_index_of,
        &&do_list_length,
        &&do_list_contains,
        &&do_exec
    };
    unsigned int *loopStart;
    unsigned int *loopEnd;
    size_t loopCount;
    DISPATCH();

do_halt:
    return pos;

do_const:
    ADD_RET_VALUE(GET_NEXT_ARG());
    DISPATCH();

do_null:
    ADD_RET_VALUE(Value());
    DISPATCH();

do_if:
    if (!READ_LAST_REG()->toBool()) {
        while (*pos != OP_ELSE && *pos != OP_ENDIF && *pos != OP_HALT)
            pos += instruction_arg_count[*pos++];
    }
    FREE_REGS(1);
    DISPATCH();

do_else:
    while (*pos != OP_ENDIF && *pos != OP_HALT)
        pos += instruction_arg_count[*pos++];

do_endif:
    DISPATCH();

do_forever_loop:
    while (true)
        run(pos, script);

do_repeat_loop:
    loopCount = READ_LAST_REG()->toLong();
    FREE_REGS(1);
    if (loopCount <= 0) {
        loopEnd = pos;
        while (*loopEnd != OP_LOOP_END && *loopEnd != OP_HALT)
            loopEnd += instruction_arg_count[*loopEnd++];
    } else {
        for (size_t i = 0; i < loopCount; i++)
            loopEnd = run(pos, script);
    }
    pos = loopEnd;
    DISPATCH();

do_until_loop:
    loopStart = run(pos, script);
    if (!READ_LAST_REG()->toBool()) {
        do {
            FREE_REGS(1);
            loopEnd = run(loopStart, script);
            run(pos, script);
        } while (!READ_LAST_REG()->toBool());
    } else {
        loopEnd = loopStart;
        while (*loopEnd != OP_LOOP_END && *loopEnd != OP_HALT)
            loopEnd += instruction_arg_count[*loopEnd++];
    }
    pos = loopEnd;
    DISPATCH();

do_begin_until_loop:
    return pos;

do_loop_end:
    return pos;

do_print:
    std::cout << READ_LAST_REG()->toString() << std::endl;
    FREE_REGS(1);
    DISPATCH();

do_add:
    REPLACE_RET_VALUE(*READ_REG(0, 2), 2);
    READ_REG(0, 2)->add(READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_subtract:
    REPLACE_RET_VALUE(*READ_REG(0, 2), 2);
    READ_REG(0, 2)->subtract(READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_multiply:
    REPLACE_RET_VALUE(*READ_REG(0, 2), 2);
    READ_REG(0, 2)->multiply(READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_divide:
    REPLACE_RET_VALUE(*READ_REG(0, 2), 2);
    READ_REG(0, 2)->divide(READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_random:
    REPLACE_RET_VALUE(randint<long>(READ_REG(0, 2)->toDouble(), READ_REG(1, 2)->toDouble()), 2);
    FREE_REGS(1);
    DISPATCH();

do_greater_than:
    REPLACE_RET_VALUE(*READ_REG(0, 2) > *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

do_less_than:
    REPLACE_RET_VALUE(*READ_REG(0, 2) < *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

do_equals:
    REPLACE_RET_VALUE(*READ_REG(0, 2) == *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

do_and:
    REPLACE_RET_VALUE(READ_REG(0, 2)->toBool() && READ_REG(1, 2)->toBool(), 2);
    FREE_REGS(1);
    DISPATCH();

do_or:
    REPLACE_RET_VALUE(READ_REG(0, 2)->toBool() || READ_REG(1, 2)->toBool(), 2);
    FREE_REGS(1);
    DISPATCH();

do_not:
    REPLACE_RET_VALUE(!READ_LAST_REG()->toBool(), 1);
    DISPATCH();

do_set_var:
    *m_variables[*++pos] = *READ_LAST_REG();
    FREE_REGS(1);
    DISPATCH();

do_read_var:
    ADD_RET_VALUE(*m_variables[*++pos]);
    DISPATCH();

do_change_var:
    m_variables[*++pos]->add(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

do_read_list:
    ADD_RET_VALUE(m_lists[*++pos]->toString());
    DISPATCH();

do_list_append:
    m_lists[*++pos]->push_back(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

do_list_del : {
    const Value *indexValue = READ_LAST_REG();
    size_t index;
    List *list = m_lists[*++pos];
    if (indexValue->isString()) {
        const std::string &str = indexValue->toString();
        if (str == "last") {
            index = list->size();
        } else if (str == "all") {
            list->clear();
            index = 0;
        } else if (str == "random") {
            size_t size = list->size();
            index = size == 0 ? 0 : randint<size_t>(1, size);
        } else
            index = 0;
    } else {
        index = indexValue->toLong();
        FIX_LIST_INDEX(index, list->size());
    }
    if (index != 0)
        list->removeAt(index - 1);
    FREE_REGS(1);
    DISPATCH();
}

do_list_del_all:
    m_lists[*++pos]->clear();
    DISPATCH();

do_list_insert : {
    const Value *indexValue = READ_REG(1, 2);
    size_t index;
    List *list = m_lists[*++pos];
    if (indexValue->isString()) {
        const std::string &str = indexValue->toString();
        if (str == "last") {
            list->push_back(*READ_REG(0, 2));
            index = 0;
        } else if (str == "random") {
            size_t size = list->size();
            index = size == 0 ? 1 : randint<size_t>(1, size);
        } else
            index = 0;
    } else {
        index = indexValue->toLong();
        FIX_LIST_INDEX(index, list->size());
    }
    if ((index != 0) || list->empty()) {
        if (list->empty())
            list->push_back(*READ_REG(0, 2));
        else
            list->insert(index - 1, *READ_REG(0, 2));
    }
    FREE_REGS(2);
    DISPATCH();
}

do_list_replace : {
    const Value *indexValue = READ_REG(0, 2);
    size_t index;
    List *list = m_lists[*++pos];
    if (indexValue->isString()) {
        std::string str = indexValue->toString();
        if (str == "last")
            index = list->size();
        else if (str == "random") {
            size_t size = list->size();
            index = size == 0 ? 0 : randint<size_t>(1, size);
        } else
            index = 0;
    } else {
        index = indexValue->toLong();
        FIX_LIST_INDEX(index, list->size());
    }
    if (index != 0)
        list->operator[](index - 1) = *READ_REG(1, 2);
    FREE_REGS(2);
    DISPATCH();
}

do_list_get_item : {
    const Value *indexValue = READ_LAST_REG();
    size_t index;
    List *list = m_lists[*++pos];
    if (indexValue->isString()) {
        std::string str = indexValue->toString();
        if (str == "last")
            index = list->size();
        else if (str == "random") {
            size_t size = list->size();
            index = size == 0 ? 0 : randint<size_t>(1, size);
        } else
            index = 0;
    } else {
        index = indexValue->toLong();
        FIX_LIST_INDEX(index, list->size());
    }
    if (index == 0) {
        REPLACE_RET_VALUE("", 1);
    } else {
        REPLACE_RET_VALUE(list->operator[](index - 1), 1);
    }
    DISPATCH();
}

do_list_index_of:
    // TODO: Add size_t support to Value and remove the static_cast
    REPLACE_RET_VALUE(static_cast<long>(m_lists[*++pos]->indexOf(*READ_LAST_REG()) + 1), 1);
    DISPATCH();

do_list_length:
    // TODO: Add size_t support to Value and remove the static_cast
    ADD_RET_VALUE(static_cast<long>(m_lists[*++pos]->size()));
    DISPATCH();

do_list_contains:
    REPLACE_RET_VALUE(m_lists[*++pos]->contains(*READ_LAST_REG()), 1);
    DISPATCH();

do_exec:
    m_globalPos = pos;
    m_script = script;
    int ret = m_functions[*++pos](this);
    FREE_REGS(ret);
    pos += ret;
    DISPATCH();
}
