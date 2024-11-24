// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>

#include "llvmregister.h"

namespace libscratchcpp
{

struct LLVMInstruction
{
        enum class Type
        {
            FunctionCall,
            Add,
            Sub,
            Mul,
            Div,
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
            BeginWhileLoop,
            BeginRepeatUntilLoop,
            BeginLoopCondition,
            EndLoop
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
};

} // namespace libscratchcpp
