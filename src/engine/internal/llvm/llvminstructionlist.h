// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

struct LLVMInstruction;

class LLVMInstructionList
{
    public:
        LLVMInstructionList() = default;
        LLVMInstructionList(const LLVMInstructionList &) = delete;

        LLVMInstruction *first() const;
        LLVMInstruction *last() const;

        bool containsInstruction(LLVMInstruction *ins) const;
        void addInstruction(std::shared_ptr<LLVMInstruction> ins);

    private:
        std::shared_ptr<LLVMInstruction> m_first;
        std::shared_ptr<LLVMInstruction> m_last;
};

} // namespace libscratchcpp
