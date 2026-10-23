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

        inline TInstruction *getPrevious() { return static_cast<TInstruction *>(previous); }

        inline TInstruction *getNext() { return static_cast<TInstruction *>(next); }

        std::vector<std::pair<Compiler::StaticType, TRegister *>> args; // target type, register
        TRegister *functionReturnReg = nullptr;
};

} // namespace libscratchcpp
