// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/list.h>
#include <iostream>
#include <cassert>

#include "virtualmachine_p.h"
#include "global.h"

#define MAX_REG_COUNT 1024

#define DISPATCH() goto *dispatch_table[*++pos]
#define FREE_REGS(count) regCount -= count
#define ADD_RET_VALUE(value) *regs[regCount++] = value
#define REPLACE_RET_VALUE(value, offset) *regs[regCount - offset] = value
#define GET_NEXT_ARG() constValues[*++pos]
#define READ_REG(index, count) regs[regCount - count + index]
#define READ_LAST_REG() regs[regCount - 1]

#define FIX_LIST_INDEX(index, listSize)                                                                                                                                                                \
    if ((listSize == 0) || (index < 1) || (index > listSize))                                                                                                                                          \
    index = 0

using namespace libscratchcpp;
using namespace vm;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

const unsigned int VirtualMachinePrivate::instruction_arg_count[] = {
    0, // OP_START
    0, // OP_HALT
    1, // OP_CONST
    0, // OP_NULL
    0, // OP_CHECKPOINT
    0, // OP_IF
    0, // OP_ELSE
    0, // OP_ENDIF
    0, // OP_FOREVER_LOOP
    0, // OP_REPEAT_LOOP
    0, // OP_REPEAT_LOOP_INDEX
    0, // OP_REPEAT_LOOP_INDEX1
    0, // OP_UNTIL_LOOP
    0, // OP_BEGIN_UNTIL_LOOP
    0, // OP_LOOP_END
    0, // OP_PRINT
    0, // OP_ADD
    0, // OP_SUBTRACT
    0, // OP_MULTIPLY
    0, // OP_DIVIDE
    0, // OP_MOD
    0, // OP_RANDOM
    0, // OP_ROUND
    0, // OP_ABS
    0, // OP_FLOOR
    0, // OP_CEIL
    0, // OP_SQRT
    0, // OP_SIN
    0, // OP_COS
    0, // OP_TAN
    0, // OP_ASIN
    0, // OP_ACOS
    0, // OP_ATAN
    0, // OP_GREATER_THAN
    0, // OP_LESS_THAN
    0, // OP_EQUALS
    0, // OP_AND
    0, // OP_OR
    0, // OP_NOT
    1, // OP_SET_VAR
    1, // OP_CHANGE_VAR
    1, // OP_READ_VAR
    1, // OP_READ_LIST
    1, // OP_LIST_APPEND
    1, // OP_LIST_DEL
    1, // OP_LIST_DEL_ALL
    1, // OP_LIST_INSERT
    1, // OP_LIST_REPLACE
    1, // OP_LIST_GET_ITEM
    1, // OP_LIST_INDEX_OF
    1, // OP_LIST_LENGTH
    1, // OP_LIST_CONTAINS
    0, // OP_STR_CONCAT
    0, // OP_STR_AT
    0, // OP_STR_LENGTH
    0, // OP_STR_CONTAINS
    1, // OP_EXEC
    0, // OP_INIT_PROCEDURE
    1, // OP_CALL_PROCEDURE
    0, // OP_ADD_ARG
    1, // OP_READ_ARG
    0, // OP_BREAK_ATOMIC
    0  // OP_WARP
};

VirtualMachinePrivate::VirtualMachinePrivate(VirtualMachine *vm, Target *target, IEngine *engine, Script *script) :
    vm(vm),
    target(target),
    engine(engine),
    script(script)
{
    regs = new Value *[MAX_REG_COUNT];
    for (int i = 0; i < MAX_REG_COUNT; i++)
        regs[i] = new Value();
    loops.reserve(256);
    callTree.reserve(1024);
}

VirtualMachinePrivate::~VirtualMachinePrivate()
{
    for (int i = 0; i < MAX_REG_COUNT; i++)
        delete regs[i];
    delete regs;
}

unsigned int *VirtualMachinePrivate::run(unsigned int *pos)
{
    static const void *dispatch_table[] = {
        nullptr,
        &&do_halt,
        &&do_const,
        &&do_null,
        &&do_checkpoint,
        &&do_if,
        &&do_else,
        &&do_endif,
        &&do_forever_loop,
        &&do_repeat_loop,
        &&do_repeat_loop_index,
        &&do_repeat_loop_index1,
        &&do_until_loop,
        &&do_begin_until_loop,
        &&do_loop_end,
        &&do_print,
        &&do_add,
        &&do_subtract,
        &&do_multiply,
        &&do_divide,
        &&do_mod,
        &&do_random,
        &&do_round,
        &&do_abs,
        &&do_floor,
        &&do_ceil,
        &&do_sqrt,
        &&do_sin,
        &&do_cos,
        &&do_tan,
        &&do_asin,
        &&do_acos,
        &&do_atan,
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
        &&do_str_concat,
        &&do_str_at,
        &&do_str_length,
        &&do_str_contains,
        &&do_exec,
        &&do_init_procedure,
        &&do_call_procedure,
        &&do_add_arg,
        &&do_read_arg,
        &&do_break_atomic,
        &&do_warp
    };
    assert(pos);
    unsigned int *loopStart;
    unsigned int *loopEnd;
    size_t loopCount;
    atEnd = false;
    atomic = true;
    warp = false;
    DISPATCH();

do_halt:
    if (regCount > 0) {
        std::cout << "warning: VM: " << regCount << " registers were leaked by the script; this is most likely a bug in the VM or in the compiler" << std::endl;
    }
    if (callTree.empty()) {
        atEnd = true;
        return pos;
    } else {
        pos = callTree.back();
        callTree.pop_back();
        procedureArgTree.pop_back();
        if (procedureArgTree.empty())
            procedureArgs = nullptr;
        else
            procedureArgs = &procedureArgTree.back();
        DISPATCH();
    }

do_const:
    ADD_RET_VALUE(GET_NEXT_ARG());
    DISPATCH();

do_null:
    ADD_RET_VALUE(Value());
    DISPATCH();

do_checkpoint:
    checkpoint = pos - 1;
    DISPATCH();

do_if:
    if (!READ_LAST_REG()->toBool()) {
        while (*pos != OP_ELSE && *pos != OP_ENDIF)
            pos += instruction_arg_count[*pos++];
    }
    FREE_REGS(1);
    DISPATCH();

do_else:
    while (*pos != OP_ENDIF)
        pos += instruction_arg_count[*pos++];

do_endif:
    DISPATCH();

do_forever_loop:
    Loop l;
    l.isRepeatLoop = true;
    l.start = pos;
    l.index = -1;
    loops.push_back(l);
    DISPATCH();

do_repeat_loop:
    loopCount = READ_LAST_REG()->toLong();
    FREE_REGS(1);
    if (loopCount <= 0) {
        loopEnd = pos;
        while (*loopEnd != OP_LOOP_END)
            loopEnd += instruction_arg_count[*loopEnd++];
        pos = loopEnd;
    } else {
        Loop l;
        l.isRepeatLoop = true;
        l.start = pos;
        l.index = 0;
        l.max = loopCount;
        loops.push_back(l);
    }
    DISPATCH();

do_repeat_loop_index : {
    assert(!loops.empty());
    Loop &l = loops.back();
    assert(l.isRepeatLoop);
    ADD_RET_VALUE(static_cast<long>(l.index));
    DISPATCH();
}

do_repeat_loop_index1 : {
    assert(!loops.empty());
    Loop &l = loops.back();
    assert(l.isRepeatLoop);
    ADD_RET_VALUE(static_cast<long>(l.index + 1));
    DISPATCH();
}

do_until_loop:
    loopStart = run(pos);
    if (!READ_LAST_REG()->toBool()) {
        Loop l;
        l.isRepeatLoop = false;
        l.start = pos;
        loops.push_back(l);
        pos = loopStart;
    } else {
        pos = loopStart;
        while (*pos != OP_LOOP_END)
            pos += instruction_arg_count[*pos++];
    }
    FREE_REGS(1);
    DISPATCH();

do_begin_until_loop:
    return pos;

do_loop_end : {
    Loop &l = loops.back();
    if (l.isRepeatLoop) {
        if ((l.index == -1) || (++l.index < l.max))
            pos = l.start;
        else
            loops.pop_back();
        if (!atomic && !warp) {
            engine->breakFrame();
            return pos;
        }
        DISPATCH();
    } else {
        if (!atomic && !warp) {
            engine->breakFrame();
            return pos - 1;
        }
        loopStart = run(l.start);
        if (!READ_LAST_REG()->toBool())
            pos = loopStart;
        else
            loops.pop_back();
        FREE_REGS(1);
        DISPATCH();
    }
}

do_print:
    std::cout << READ_LAST_REG()->toString() << std::endl;
    FREE_REGS(1);
    DISPATCH();

do_add:
    READ_REG(0, 2)->add(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_subtract:
    READ_REG(0, 2)->subtract(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_multiply:
    READ_REG(0, 2)->multiply(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_divide:
    READ_REG(0, 2)->divide(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_mod:
    READ_REG(0, 2)->mod(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

do_random:
    REPLACE_RET_VALUE(randint<long>(READ_REG(0, 2)->toDouble(), READ_REG(1, 2)->toDouble()), 2);
    FREE_REGS(1);
    DISPATCH();

do_round : {
    const Value *v = READ_REG(0, 1);
    if (!v->isInfinity() && !v->isNegativeInfinity()) {
        if (v->toDouble() < 0) {
            REPLACE_RET_VALUE(static_cast<long>(std::floor(v->toDouble() + 0.5)), 1);
        } else
            REPLACE_RET_VALUE(static_cast<long>(v->toDouble() + 0.5), 1);
    }
    DISPATCH();
}

do_abs : {
    const Value *v = READ_REG(0, 1);
    if (v->isNegativeInfinity())
        REPLACE_RET_VALUE(Value(Value::SpecialValue::Infinity), 1);
    else if (!v->isInfinity())
        REPLACE_RET_VALUE(std::abs(v->toDouble()), 1);
    DISPATCH();
}

do_floor : {
    const Value *v = READ_REG(0, 1);
    if (!v->isInfinity() && !v->isNegativeInfinity())
        REPLACE_RET_VALUE(std::floor(v->toDouble()), 1);
    DISPATCH();
}

do_ceil : {
    const Value *v = READ_REG(0, 1);
    if (!v->isInfinity() && !v->isNegativeInfinity())
        REPLACE_RET_VALUE(std::ceil(v->toDouble()), 1);
    DISPATCH();
}

do_sqrt : {
    const Value &v = *READ_REG(0, 1);
    if (v < 0)
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else if (!v.isInfinity())
        REPLACE_RET_VALUE(std::sqrt(v.toDouble()), 1);
    DISPATCH();
}

do_sin : {
    const Value *v = READ_REG(0, 1);
    if (v->isInfinity() || v->isNegativeInfinity())
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else
        REPLACE_RET_VALUE(std::sin(v->toDouble() * pi / 180), 1);
    DISPATCH();
}

do_cos : {
    const Value *v = READ_REG(0, 1);
    if (v->isInfinity() || v->isNegativeInfinity())
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else
        REPLACE_RET_VALUE(std::cos(v->toDouble() * pi / 180), 1);
    DISPATCH();
}

do_tan : {
    const Value *v = READ_REG(0, 1);
    if (v->isInfinity() || v->isNegativeInfinity())
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else {
        long mod;
        if (v->toLong() < 0)
            mod = (v->toLong() + 360) % 360;
        else
            mod = v->toLong() % 360;
        if (mod == 90)
            REPLACE_RET_VALUE(Value(Value::SpecialValue::Infinity), 1);
        else if (mod == 270)
            REPLACE_RET_VALUE(Value(Value::SpecialValue::NegativeInfinity), 1);
        else
            REPLACE_RET_VALUE(std::tan(v->toDouble() * pi / 180), 1);
    }
    DISPATCH();
}

do_asin : {
    const Value &v = *READ_REG(0, 1);
    if (v < -1 || v > 1)
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else
        REPLACE_RET_VALUE(std::asin(v.toDouble()) * 180 / pi, 1);
    DISPATCH();
}

do_acos : {
    const Value &v = *READ_REG(0, 1);
    if (v < -1 || v > 1)
        REPLACE_RET_VALUE(Value(Value::SpecialValue::NaN), 1);
    else
        REPLACE_RET_VALUE(std::acos(v.toDouble()) * 180 / pi, 1);
    DISPATCH();
}

do_atan : {
    const Value &v = *READ_REG(0, 1);
    if (v.isInfinity())
        REPLACE_RET_VALUE(90, 1);
    else if (v.isNegativeInfinity())
        REPLACE_RET_VALUE(-90, 1);
    else
        REPLACE_RET_VALUE(std::atan(v.toDouble()) * 180 / pi, 1);
    DISPATCH();
}

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
    *variables[*++pos] = *READ_LAST_REG();
    FREE_REGS(1);
    DISPATCH();

do_read_var:
    ADD_RET_VALUE(*variables[*++pos]);
    DISPATCH();

do_change_var:
    variables[*++pos]->add(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

do_read_list:
    ADD_RET_VALUE(lists[*++pos]->toString());
    DISPATCH();

do_list_append:
    lists[*++pos]->push_back(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

do_list_del : {
    const Value *indexValue = READ_LAST_REG();
    size_t index;
    List *list = lists[*++pos];
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
    lists[*++pos]->clear();
    DISPATCH();

do_list_insert : {
    const Value *indexValue = READ_REG(1, 2);
    size_t index;
    List *list = lists[*++pos];
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
    List *list = lists[*++pos];
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
    List *list = lists[*++pos];
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
    REPLACE_RET_VALUE(static_cast<long>(lists[*++pos]->indexOf(*READ_LAST_REG()) + 1), 1);
    DISPATCH();

do_list_length:
    // TODO: Add size_t support to Value and remove the static_cast
    ADD_RET_VALUE(static_cast<long>(lists[*++pos]->size()));
    DISPATCH();

do_list_contains:
    REPLACE_RET_VALUE(lists[*++pos]->contains(*READ_LAST_REG()), 1);
    DISPATCH();

do_str_concat:
    REPLACE_RET_VALUE(READ_REG(0, 2)->toString() + READ_REG(1, 2)->toString(), 2);
    FREE_REGS(1);
    DISPATCH();

do_str_at : {
    size_t index = READ_REG(1, 2)->toLong() - 1;
    std::u16string str = READ_REG(0, 2)->toUtf16();
    if (index < 0 || index >= str.size())
        REPLACE_RET_VALUE("", 2);
    else
        REPLACE_RET_VALUE(utf8::utf16to8(std::u16string({ str[index] })), 2);
    FREE_REGS(1);
    DISPATCH();
}

do_str_length:
    REPLACE_RET_VALUE(static_cast<long>(READ_REG(0, 1)->toUtf16().size()), 1);
    DISPATCH();

do_str_contains:
    REPLACE_RET_VALUE(READ_REG(0, 2)->toUtf16().find(READ_REG(1, 2)->toUtf16()) != std::u16string::npos, 2);
    FREE_REGS(1);
    DISPATCH();

do_exec : {
    auto ret = functions[*++pos](vm);
    if (updatePos) {
        pos = this->pos;
        updatePos = false;
    }
    if (stop) {
        stop = false;
        callTree.clear();
        procedureArgTree.clear();
        procedureArgs = nullptr;
        nextProcedureArgs = nullptr;
        if (!atomic)
            engine->breakFrame();
        if (goBack) {
            goBack = false;
            pos -= instruction_arg_count[OP_EXEC] + 1;
        } else
            FREE_REGS(ret);
        return pos;
    }
    FREE_REGS(ret);
    DISPATCH();
}

do_init_procedure:
    procedureArgTree.push_back({});
    if (procedureArgTree.size() >= 2)
        procedureArgs = &procedureArgTree[procedureArgTree.size() - 2];
    nextProcedureArgs = &procedureArgTree.back();
    DISPATCH();

do_call_procedure:
    callTree.push_back(++pos);
    procedureArgs = nextProcedureArgs;
    nextProcedureArgs = nullptr;
    pos = procedures[*pos];
    DISPATCH();

do_add_arg:
    nextProcedureArgs->push_back(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

do_read_arg:
    ADD_RET_VALUE(procedureArgs->operator[](*++pos));
    DISPATCH();

do_break_atomic:
    atomic = false;
    DISPATCH();

do_warp:
    warp = true;
    DISPATCH();
}
