// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/list.h>
#include <iostream>
#include <cassert>
#include <utf8.h>

#include "virtualmachine_p.h"
#include "internal/randomgenerator.h"

#ifdef ENABLE_COMPUTED_GOTO
#define DISPATCH() goto *dispatch_table[*++pos]
#define OP(opcode) DO_##opcode
#else
#define DISPATCH() break
#define OP(opcode) case Opcode::OP_##opcode
#endif

#define FREE_REGS(count) regCount -= count
#define ADD_RET_VALUE(value)                                                                                                                                                                           \
    if (regCount + 1 >= regsVector.size()) {                                                                                                                                                           \
        regsVector.reserve(regsVector.size() + 1024);                                                                                                                                                  \
        for (size_t i = 0; i < 1024; i++)                                                                                                                                                              \
            regsVector.push_back(new Value());                                                                                                                                                         \
        regs = regsVector.data();                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    *regs[regCount++] = value
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

IRandomGenerator *VirtualMachinePrivate::rng = nullptr;

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
    0, // OP_BREAK_FRAME
    0  // OP_WARP
};

VirtualMachinePrivate::VirtualMachinePrivate(VirtualMachine *vm, Target *target, IEngine *engine, Script *script, Thread *thread) :
    vm(vm),
    target(target),
    engine(engine),
    script(script),
    thread(thread)
{
    regsVector.reserve(1024);
    for (int i = 0; i < 1024; i++)
        regsVector.push_back(new Value());
    regs = regsVector.data();
    loops.reserve(256);
    callTree.reserve(1024);
    loopCountTree.reserve(1024);

    if (!rng)
        rng = RandomGenerator::instance().get();
}

VirtualMachinePrivate::~VirtualMachinePrivate()
{
    for (int i = 0; i < regsVector.size(); i++)
        delete regsVector[i];
}

unsigned int *VirtualMachinePrivate::run(unsigned int *pos, bool reset)
{
#ifdef ENABLE_COMPUTED_GOTO
    static const void *dispatch_table[] = {
        nullptr,
        &&DO_HALT,
        &&DO_CONST,
        &&DO_NULL,
        &&DO_CHECKPOINT,
        &&DO_IF,
        &&DO_ELSE,
        &&DO_ENDIF,
        &&DO_FOREVER_LOOP,
        &&DO_REPEAT_LOOP,
        &&DO_REPEAT_LOOP_INDEX,
        &&DO_REPEAT_LOOP_INDEX1,
        &&DO_UNTIL_LOOP,
        &&DO_BEGIN_UNTIL_LOOP,
        &&DO_LOOP_END,
        &&DO_PRINT,
        &&DO_ADD,
        &&DO_SUBTRACT,
        &&DO_MULTIPLY,
        &&DO_DIVIDE,
        &&DO_MOD,
        &&DO_RANDOM,
        &&DO_ROUND,
        &&DO_ABS,
        &&DO_FLOOR,
        &&DO_CEIL,
        &&DO_SQRT,
        &&DO_SIN,
        &&DO_COS,
        &&DO_TAN,
        &&DO_ASIN,
        &&DO_ACOS,
        &&DO_ATAN,
        &&DO_GREATER_THAN,
        &&DO_LESS_THAN,
        &&DO_EQUALS,
        &&DO_AND,
        &&DO_OR,
        &&DO_NOT,
        &&DO_SET_VAR,
        &&DO_CHANGE_VAR,
        &&DO_READ_VAR,
        &&DO_READ_LIST,
        &&DO_LIST_APPEND,
        &&DO_LIST_DEL,
        &&DO_LIST_DEL_ALL,
        &&DO_LIST_INSERT,
        &&DO_LIST_REPLACE,
        &&DO_LIST_GET_ITEM,
        &&DO_LIST_INDEX_OF,
        &&DO_LIST_LENGTH,
        &&DO_LIST_CONTAINS,
        &&DO_STR_CONCAT,
        &&DO_STR_AT,
        &&DO_STR_LENGTH,
        &&DO_STR_CONTAINS,
        &&DO_EXEC,
        &&DO_INIT_PROCEDURE,
        &&DO_CALL_PROCEDURE,
        &&DO_ADD_ARG,
        &&DO_READ_ARG,
        &&DO_BREAK_FRAME,
        &&DO_WARP
    };
#endif // ENABLE_COMPUTED_GOTO

    assert(pos);
    unsigned int *loopStart;
    unsigned int *loopEnd;
    long loopCount;
    if (reset) {
        atEnd = false;
        noBreak = true;
        warp = false;
    }

#ifdef ENABLE_COMPUTED_GOTO
    DISPATCH();
#else
    while (true)
        switch (*++pos) {
#endif

    OP(HALT) :
        if (regCount > 0)
    {
        std::cout << "warning: VM: " << regCount << " registers were leaked by the script; this is most likely a bug in the VM or in the compiler" << std::endl;
    }
    if (callTree.empty()) {
        atEnd = true;
        return pos;
    } else {
        const auto &oldState = callTree.back();
        pos = oldState.first;
        warp = oldState.second;
        callTree.pop_back();
        procedureArgTree.pop_back();

        if (procedureArgTree.empty())
            procedureArgs = nullptr;
        else
            procedureArgs = &procedureArgTree.back();

        int loopsToRemove = loops.size() - loopCountTree.back();
        assert(loopsToRemove >= 0);
        loopCountTree.pop_back();

        for (int i = 0; i < loopsToRemove; i++)
            loops.pop_back();

        DISPATCH();
    }

    OP(CONST) :
        ADD_RET_VALUE(GET_NEXT_ARG());
    DISPATCH();

    OP(NULL) :
        ADD_RET_VALUE(Value());
    DISPATCH();

    OP(CHECKPOINT) :
        checkpoint = pos - 1;
    DISPATCH();

    OP(IF) :
    {
        if (!READ_LAST_REG()->toBool()) {
            unsigned int ifCounter = 1;
            while (!((*pos == OP_ELSE && ifCounter == 1) || (*pos == OP_ENDIF && ifCounter == 0))) {
                pos += instruction_arg_count[*pos++];

                if ((*pos == OP_IF) || (*pos == OP_FOREVER_LOOP) || (*pos == OP_REPEAT_LOOP) || (*pos == OP_UNTIL_LOOP))
                    ifCounter++;
                else if ((*pos == OP_ENDIF) || (*pos == OP_LOOP_END)) {
                    assert(ifCounter > 0);
                    ifCounter--;
                }
            }
        }
        FREE_REGS(1);
        DISPATCH();
    }

    OP(ELSE) :
    {
        unsigned int ifCounter = 1;
        while (!(*pos == OP_ENDIF && ifCounter == 0)) {
            pos += instruction_arg_count[*pos++];

            if ((*pos == OP_IF) || (*pos == OP_FOREVER_LOOP) || (*pos == OP_REPEAT_LOOP) || (*pos == OP_UNTIL_LOOP))
                ifCounter++;
            else if ((*pos == OP_ENDIF) || (*pos == OP_LOOP_END)) {
                assert(ifCounter > 0);
                ifCounter--;
            }

            assert(!(*pos == OP_ELSE && ifCounter == 1));
        }
    }

    OP(ENDIF) :
        DISPATCH();

    OP(FOREVER_LOOP) :
        Loop l;
    l.isRepeatLoop = true;
    l.start = pos;
    l.index = -1;
    loops.push_back(l);
    DISPATCH();

    OP(REPEAT_LOOP) :
        loopCount = std::round(READ_LAST_REG()->toDouble());
    FREE_REGS(1);
    if (loopCount <= 0) {
        loopEnd = pos;
        unsigned int loopCounter = 1;
        while ((*loopEnd != OP_LOOP_END) || (loopCounter > 0)) {
            loopEnd += instruction_arg_count[*loopEnd++];

            if ((*loopEnd == OP_IF) || (*loopEnd == OP_FOREVER_LOOP) || (*loopEnd == OP_REPEAT_LOOP) || (*loopEnd == OP_UNTIL_LOOP))
                loopCounter++;
            else if ((*loopEnd == OP_ENDIF) || (*loopEnd == OP_LOOP_END)) {
                assert(loopCounter > 0);
                loopCounter--;
            }
        }
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

    OP(REPEAT_LOOP_INDEX) :
    {
        assert(!loops.empty());
        Loop &l = loops.back();
        assert(l.isRepeatLoop);
        ADD_RET_VALUE(static_cast<long>(l.index));
        DISPATCH();
    }

    OP(REPEAT_LOOP_INDEX1) :
    {
        assert(!loops.empty());
        Loop &l = loops.back();
        assert(l.isRepeatLoop);
        ADD_RET_VALUE(static_cast<long>(l.index + 1));
        DISPATCH();
    }

    OP(UNTIL_LOOP) :
        loopStart = run(pos, false);
    if (!READ_LAST_REG()->toBool()) {
        Loop l;
        l.isRepeatLoop = false;
        l.start = pos;
        loops.push_back(l);
        pos = loopStart;
    } else {
        pos = loopStart;
        unsigned int loopCounter = 1;
        while ((*pos != OP_LOOP_END) || (loopCounter > 0)) {
            pos += instruction_arg_count[*pos++];

            if ((*pos == OP_IF) || (*pos == OP_FOREVER_LOOP) || (*pos == OP_REPEAT_LOOP) || (*pos == OP_UNTIL_LOOP))
                loopCounter++;
            else if ((*pos == OP_ENDIF) || (*pos == OP_LOOP_END)) {
                assert(loopCounter > 0);
                loopCounter--;
            }
        }
    }
    FREE_REGS(1);
    DISPATCH();

    OP(BEGIN_UNTIL_LOOP) :
        return pos;

    OP(LOOP_END) :
    {
        assert(!loops.empty());
        Loop &l = loops.back();
        if (l.isRepeatLoop) {
            if ((l.index == -1) || (++l.index < l.max))
                pos = l.start;
            else
                loops.pop_back();
            if (!noBreak && !warp)
                return pos;
            DISPATCH();
        } else {
            if (!noBreak && !warp)
                return pos - 1;
            loopStart = run(l.start, false);
            if (!READ_LAST_REG()->toBool())
                pos = loopStart;
            else
                loops.pop_back();
            FREE_REGS(1);
            DISPATCH();
        }
    }

    OP(PRINT) :
        std::cout << READ_LAST_REG()->toString() << std::endl;
    FREE_REGS(1);
    DISPATCH();

    OP(ADD) :
        READ_REG(0, 2)->add(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

    OP(SUBTRACT) :
        READ_REG(0, 2)->subtract(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

    OP(MULTIPLY) :
        READ_REG(0, 2)->multiply(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

    OP(DIVIDE) :
        READ_REG(0, 2)->divide(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

    OP(MOD) :
        READ_REG(0, 2)->mod(*READ_REG(1, 2));
    FREE_REGS(1);
    DISPATCH();

    OP(RANDOM) :
        if (READ_REG(0, 2)->isInt() && READ_REG(1, 2)->isInt()) REPLACE_RET_VALUE(rng->randint(READ_REG(0, 2)->toInt(), READ_REG(1, 2)->toInt()), 2);
    else REPLACE_RET_VALUE(rng->randintDouble(READ_REG(0, 2)->toDouble(), READ_REG(1, 2)->toDouble()), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(ROUND) :
    {
        const Value *v = READ_REG(0, 1);
        if (!v->isInfinity() && !v->isNegativeInfinity()) {
            if (v->toDouble() < 0) {
                REPLACE_RET_VALUE(static_cast<long>(std::floor(v->toDouble() + 0.5)), 1);
            } else
                REPLACE_RET_VALUE(static_cast<long>(v->toDouble() + 0.5), 1);
        }
        DISPATCH();
    }

    OP(ABS) :
    {
        const Value *v = READ_REG(0, 1);
        if (v->isNegativeInfinity())
            REPLACE_RET_VALUE(Value(SpecialValue::Infinity), 1);
        else if (!v->isInfinity())
            REPLACE_RET_VALUE(std::abs(v->toDouble()), 1);
        DISPATCH();
    }

    OP(FLOOR) :
    {
        const Value *v = READ_REG(0, 1);
        if (!v->isInfinity() && !v->isNegativeInfinity())
            REPLACE_RET_VALUE(std::floor(v->toDouble()), 1);
        DISPATCH();
    }

    OP(CEIL) :
    {
        const Value *v = READ_REG(0, 1);
        if (!v->isInfinity() && !v->isNegativeInfinity())
            REPLACE_RET_VALUE(std::ceil(v->toDouble()), 1);
        DISPATCH();
    }

    OP(SQRT) :
    {
        const Value &v = *READ_REG(0, 1);
        if (v < 0)
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else if (!v.isInfinity())
            REPLACE_RET_VALUE(std::sqrt(v.toDouble()), 1);
        DISPATCH();
    }

    OP(SIN) :
    {
        const Value *v = READ_REG(0, 1);
        if (v->isInfinity() || v->isNegativeInfinity())
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else
            REPLACE_RET_VALUE(std::sin(v->toDouble() * pi / 180), 1);
        DISPATCH();
    }

    OP(COS) :
    {
        const Value *v = READ_REG(0, 1);
        if (v->isInfinity() || v->isNegativeInfinity())
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else
            REPLACE_RET_VALUE(std::cos(v->toDouble() * pi / 180), 1);
        DISPATCH();
    }

    OP(TAN) :
    {
        const Value *v = READ_REG(0, 1);
        if (v->isInfinity() || v->isNegativeInfinity())
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else {
            long mod;
            if (v->toLong() < 0)
                mod = (v->toLong() + 360) % 360;
            else
                mod = v->toLong() % 360;
            if (mod == 90)
                REPLACE_RET_VALUE(Value(SpecialValue::Infinity), 1);
            else if (mod == 270)
                REPLACE_RET_VALUE(Value(SpecialValue::NegativeInfinity), 1);
            else
                REPLACE_RET_VALUE(std::tan(v->toDouble() * pi / 180), 1);
        }
        DISPATCH();
    }

    OP(ASIN) :
    {
        const Value &v = *READ_REG(0, 1);
        if (v < -1 || v > 1)
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else
            REPLACE_RET_VALUE(std::asin(v.toDouble()) * 180 / pi, 1);
        DISPATCH();
    }

    OP(ACOS) :
    {
        const Value &v = *READ_REG(0, 1);
        if (v < -1 || v > 1)
            REPLACE_RET_VALUE(Value(SpecialValue::NaN), 1);
        else
            REPLACE_RET_VALUE(std::acos(v.toDouble()) * 180 / pi, 1);
        DISPATCH();
    }

    OP(ATAN) :
    {
        const Value &v = *READ_REG(0, 1);
        if (v.isInfinity())
            REPLACE_RET_VALUE(90, 1);
        else if (v.isNegativeInfinity())
            REPLACE_RET_VALUE(-90, 1);
        else
            REPLACE_RET_VALUE(std::atan(v.toDouble()) * 180 / pi, 1);
        DISPATCH();
    }

    OP(GREATER_THAN) :
        REPLACE_RET_VALUE(*READ_REG(0, 2) > *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(LESS_THAN) :
        REPLACE_RET_VALUE(*READ_REG(0, 2) < *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(EQUALS) :
        REPLACE_RET_VALUE(*READ_REG(0, 2) == *READ_REG(1, 2), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(AND) :
        REPLACE_RET_VALUE(READ_REG(0, 2)->toBool() && READ_REG(1, 2)->toBool(), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(OR) :
        REPLACE_RET_VALUE(READ_REG(0, 2)->toBool() || READ_REG(1, 2)->toBool(), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(NOT) :
        REPLACE_RET_VALUE(!READ_LAST_REG()->toBool(), 1);
    DISPATCH();

    OP(SET_VAR) :
        *variables[*++pos] = *READ_LAST_REG();
    FREE_REGS(1);
    DISPATCH();

    OP(READ_VAR) :
        ADD_RET_VALUE(*variables[*++pos]);
    DISPATCH();

    OP(CHANGE_VAR) :
        variables[*++pos]->add(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

    OP(READ_LIST) :
    {
        std::string s;
        lists[*++pos]->toString(s);
        ADD_RET_VALUE(s);
    }
    DISPATCH();

    OP(LIST_APPEND) :
        lists[*++pos]->append(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

    OP(LIST_DEL) :
    {
        const Value *indexValue = READ_LAST_REG();
        size_t index;
        List *list = lists[*++pos];
        if (indexValue->isValidNumber()) {
            index = indexValue->toLong();
            FIX_LIST_INDEX(index, list->size());
        } else {
            const std::string &str = indexValue->toString();
            if (str == "last") {
                index = list->size();
            } else if (str == "all") {
                list->clear();
                index = 0;
            } else if (str == "random") {
                size_t size = list->size();
                index = size == 0 ? 0 : rng->randint(1, size);
            } else
                index = 0;
        }
        if (index != 0)
            list->removeAt(index - 1);
        FREE_REGS(1);
        DISPATCH();
    }

    OP(LIST_DEL_ALL) :
        lists[*++pos]->clear();
    DISPATCH();

    OP(LIST_INSERT) :
    {
        const Value *indexValue = READ_REG(1, 2);
        size_t index;
        List *list = lists[*++pos];
        if (indexValue->isValidNumber()) {
            index = indexValue->toLong();
            FIX_LIST_INDEX(index, list->size());
        } else {
            const std::string &str = indexValue->toString();
            if (str == "last") {
                list->append(*READ_REG(0, 2));
                index = 0;
            } else if (str == "random") {
                size_t size = list->size();
                index = size == 0 ? 1 : rng->randint(1, size);
            } else
                index = 0;
        }
        if ((index != 0) || list->empty()) {
            if (list->empty())
                list->append(*READ_REG(0, 2));
            else
                list->insert(index - 1, *READ_REG(0, 2));
        }
        FREE_REGS(2);
        DISPATCH();
    }

    OP(LIST_REPLACE) :
    {
        const Value *indexValue = READ_REG(0, 2);
        size_t index;
        List *list = lists[*++pos];
        if (indexValue->isValidNumber()) {
            index = indexValue->toLong();
            FIX_LIST_INDEX(index, list->size());
        } else {
            std::string str = indexValue->toString();
            if (str == "last")
                index = list->size();
            else if (str == "random") {
                size_t size = list->size();
                index = size == 0 ? 0 : rng->randint(1, size);
            } else
                index = 0;
        }
        if (index != 0)
            list->replace(index - 1, *READ_REG(1, 2));
        FREE_REGS(2);
        DISPATCH();
    }

    OP(LIST_GET_ITEM) :
    {
        const Value *indexValue = READ_LAST_REG();
        size_t index;
        List *list = lists[*++pos];
        if (indexValue->isValidNumber()) {
            index = indexValue->toLong();
            FIX_LIST_INDEX(index, list->size());
        } else {
            std::string str = indexValue->toString();
            if (str == "last")
                index = list->size();
            else if (str == "random") {
                size_t size = list->size();
                index = size == 0 ? 0 : rng->randint(1, size);
            } else
                index = 0;
        }
        if (index == 0) {
            REPLACE_RET_VALUE("", 1);
        } else {
            REPLACE_RET_VALUE(list->operator[](index - 1), 1);
        }
        DISPATCH();
    }

    OP(LIST_INDEX_OF) :
        // TODO: Add size_t support to Value and remove the static_cast
        REPLACE_RET_VALUE(static_cast<long>(lists[*++pos]->indexOf(*READ_LAST_REG()) + 1), 1);
    DISPATCH();

    OP(LIST_LENGTH) :
        // TODO: Add size_t support to Value and remove the static_cast
        ADD_RET_VALUE(static_cast<long>(lists[*++pos]->size()));
    DISPATCH();

    OP(LIST_CONTAINS) :
        REPLACE_RET_VALUE(lists[*++pos]->contains(*READ_LAST_REG()), 1);
    DISPATCH();

    OP(STR_CONCAT) :
        REPLACE_RET_VALUE(READ_REG(0, 2)->toString() + READ_REG(1, 2)->toString(), 2);
    FREE_REGS(1);
    DISPATCH();

    OP(STR_AT) :
    {
        size_t index = READ_REG(1, 2)->toLong() - 1;
        {
            std::u16string str = READ_REG(0, 2)->toUtf16();
            if (index < 0 || index >= str.size())
                REPLACE_RET_VALUE("", 2);
            else
                REPLACE_RET_VALUE(utf8::utf16to8(std::u16string({ str[index] })), 2);
            FREE_REGS(1);
        }
        DISPATCH();
    }

    OP(STR_LENGTH) :
        REPLACE_RET_VALUE(static_cast<long>(READ_REG(0, 1)->toUtf16().size()), 1);
    DISPATCH();

    OP(STR_CONTAINS) :
    {
        std::u16string string1 = READ_REG(0, 2)->toUtf16();
        std::u16string string2 = READ_REG(1, 2)->toUtf16();
        std::transform(string1.begin(), string1.end(), string1.begin(), ::tolower);
        std::transform(string2.begin(), string2.end(), string2.begin(), ::tolower);
        REPLACE_RET_VALUE(string1.find(string2) != std::u16string::npos, 2);
        FREE_REGS(1);
    }
    DISPATCH();

    OP(EXEC) :
    {
        auto ret = functions[*++pos](vm);
        if (updatePos) {
            pos = this->pos;
            updatePos = false;
        }
        if (stop) {
            stop = false;
            if (goBack) {
                goBack = false;
                pos -= instruction_arg_count[OP_EXEC] + 1;
                // NOTE: Going back leaks all registers for the next time the same function is called.
                // This is for example used in the wait block (to call it again with the same time value).
            } else
                FREE_REGS(ret);

            if (!warp) // TODO: This should always return if there's a "warp timer" enabled
                return pos;

            DISPATCH(); // this avoids freeing registers after "stopping" a warp script
        }
        FREE_REGS(ret);

        if (atEnd) {
            if (regCount > 0)
                std::cout << "warning: VM: " << regCount << " registers were leaked by the script; this is most likely a bug in the VM or in the compiler" << std::endl;

            return pos;
        }

        DISPATCH();
    }

    OP(INIT_PROCEDURE) :
        procedureArgTree.push_back({});
    if (procedureArgTree.size() >= 2)
        procedureArgs = &procedureArgTree[procedureArgTree.size() - 2];
    nextProcedureArgs = &procedureArgTree.back();
    DISPATCH();

    OP(CALL_PROCEDURE) :
    {
        unsigned int *procedurePos = procedures[pos[1]];

        if (procedurePos) {
            callTree.push_back({ ++pos, warp });
            procedureArgs = nextProcedureArgs;
            nextProcedureArgs = nullptr;
            loopCountTree.push_back(loops.size());
            pos = procedurePos;
        } else
            pos++;

        DISPATCH();
    }

    OP(ADD_ARG) :
        nextProcedureArgs->push_back(*READ_LAST_REG());
    FREE_REGS(1);
    DISPATCH();

    OP(READ_ARG) :
        ADD_RET_VALUE(procedureArgs->operator[](*++pos));
    DISPATCH();

    OP(BREAK_FRAME) :
        noBreak = false;
    DISPATCH();

    OP(WARP) :
        warp = true;
    DISPATCH();
#if !defined(ENABLE_COMPUTED_GOTO)
}
#endif
}
