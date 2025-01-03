// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>

#include "llvmregister.h"

namespace libscratchcpp
{

class BlockPrototype;

struct LLVMInstruction
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
            CallProcedure,
            ProcedureArg
        };

        LLVMInstruction(Type type) :
            type(type)
        {
        }

        Type type;
        std::string functionName;
        std::vector<std::pair<Compiler::StaticType, LLVMRegister *>> args; // target type, register
        LLVMRegister *functionReturnReg = nullptr;
        bool functionTargetArg = false;   // whether to add target ptr to function parameters
        bool functionCtxArg = false;      // whether to add execution context ptr to function parameters
        Variable *workVariable = nullptr; // for variables
        List *workList = nullptr;         // for lists
        BlockPrototype *procedurePrototype = nullptr;
        size_t procedureArgIndex = 0;
};

} // namespace libscratchcpp
