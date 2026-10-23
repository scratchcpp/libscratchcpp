// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructionlistbase.h"

namespace libscratchcpp
{

template<typename TInstruction>
class InstructionList : public InstructionListBase
{
    public:
        inline TInstruction *first() const { return static_cast<TInstruction *>(InstructionListBase::first()); }

        inline TInstruction *last() const { return static_cast<TInstruction *>(InstructionListBase::last()); }

        bool containsInstruction(InstructionBase *ins) const { return InstructionListBase::containsInstruction(ins); }

        bool containsInstruction(std::function<bool(const TInstruction *)> func) const
        {
            return InstructionListBase::containsInstruction([&](const InstructionBase *i) { return func(static_cast<const TInstruction *>(i)); });
        }

        void addInstruction(std::shared_ptr<TInstruction> ins) { return InstructionListBase::addInstruction(std::move(ins)); }
};

} // namespace libscratchcpp
