// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructionbase.h"

namespace libscratchcpp
{

template<typename TInstruction, typename TRegister>
struct Instruction : public InstructionBase
{
        using Type = InstructionBase::Type;
        using InstructionBase::InstructionBase; // inherit constructor

        inline TInstruction *getPrevious() const { return static_cast<TInstruction *>(previous); }

        inline TInstruction *getNext() const { return static_cast<TInstruction *>(next); }

        inline std::vector<std::pair<Compiler::StaticType, TRegister *>> getArgs() const
        {
            // TODO: Replace this with getArg(index) and argCount() methods
            std::vector<std::pair<Compiler::StaticType, TRegister *>> out;
            out.reserve(args.size());

            for (auto &a : args)
                out.emplace_back(a.first, dynamic_cast<TRegister *>(a.second));

            return out;
        }

        inline TRegister *getFunctionReturnReg() const { return dynamic_cast<TRegister *>(functionReturnReg); }
        inline void setFunctionReturnReg(TRegister *reg) { functionReturnReg = reg; }
};

} // namespace libscratchcpp
