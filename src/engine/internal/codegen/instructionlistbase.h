// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <functional>

#include "test_export.h"
#include "instructionbase.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT InstructionListBase
{
    public:
        InstructionListBase() = default;
        InstructionListBase(const InstructionListBase &) = delete;

        inline InstructionBase *first() const { return m_first.get(); }

        inline InstructionBase *last() const { return m_last.get(); }

        inline bool empty() const { return !first(); }

        bool containsInstruction(InstructionBase *ins) const;

        bool containsInstruction(std::function<bool(const InstructionBase *)> func) const;

        void addInstruction(std::shared_ptr<InstructionBase> ins);

    private:
        std::shared_ptr<InstructionBase> m_first;
        std::shared_ptr<InstructionBase> m_last;
};

} // namespace libscratchcpp
