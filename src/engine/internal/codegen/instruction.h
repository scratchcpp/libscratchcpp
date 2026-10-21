// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

class BlockPrototype;

template<typename TInstruction, typename TRegister>
struct Instruction
{
        enum class Type
        {
            FunctionCall,
            Add,
            Sub,
            Mul,
            Div,
            Random,
            RandomInt,
            CmpEQ,
            CmpGT,
            CmpLT,
            StrCmpEQCS, // case sensitive
            StrCmpEQCI, // case insensitive
            And,
            Or,
            Not,
            Mod,
            Round,
            Abs,
            Floor,
            Ceil,
            Sqrt,
            Sin,
            Cos,
            Tan,
            Asin,
            Acos,
            Atan,
            Ln,
            Log10,
            Exp,
            Exp10,
            StringConcat,
            StringChar,
            StringLength,
            Select,
            CreateLocalVariable,
            WriteLocalVariable,
            ReadLocalVariable,
            WriteVariable,
            ReadVariable,
            ClearList,
            RemoveListItem,
            AppendToList,
            InsertToList,
            ListReplace,
            GetListContents,
            GetListItem,
            GetListSize,
            GetListItemIndex,
            ListContainsItem,
            Yield,
            BeginIf,
            BeginElse,
            EndIf,
            BeginRepeatLoop,
            LoopIndex,
            BeginWhileLoop,
            BeginRepeatUntilLoop,
            BeginLoopCondition,
            EndLoop,
            Stop,
            ThreadStop,
            InvalidateTarget,
            CallProcedure,
            ProcedureArg,
            Print
        };

        Instruction(Type type, bool loopCondition) :
            type(type),
            loopCondition(loopCondition)
        {
        }

        Type type;
        std::string functionName;
        std::vector<std::pair<Compiler::StaticType, TRegister *>> args; // target type, register
        TRegister *functionReturnReg = nullptr;
        bool functionTargetArg = false;                                  // whether to add target ptr to function parameters
        bool functionCtxArg = false;                                     // whether to add execution context ptr to function parameters
        Variable *targetVariable = nullptr;                              // for variables
        List *targetList = nullptr;                                      // for lists
        Compiler::StaticType targetType = Compiler::StaticType::Unknown; // variable or list type (before read/write operation)
        BlockPrototype *procedurePrototype = nullptr;
        size_t procedureArgIndex = 0;
        bool loopCondition = false; // whether the instruction is part of a loop condition

        // Linked list
        TInstruction *previous = nullptr;
        TInstruction *next = nullptr;
        std::shared_ptr<TInstruction> _next; // ensure items are not deleted
};

} // namespace libscratchcpp
