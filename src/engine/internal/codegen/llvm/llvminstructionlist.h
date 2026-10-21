// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <functional>

#include "test_export.h"

namespace libscratchcpp
{

struct LLVMInstruction;

class LIBSCRATCHCPP_TEST_EXPORT LLVMInstructionList
{
    public:
        LLVMInstructionList() = default;
        LLVMInstructionList(const LLVMInstructionList &) = delete;

        LLVMInstruction *first() const;
        LLVMInstruction *last() const;

        bool empty() const { return !first(); }

        bool containsInstruction(LLVMInstruction *ins) const;
        bool containsInstruction(std::function<bool(const LLVMInstruction *)> func) const;

        void addInstruction(std::shared_ptr<LLVMInstruction> ins);

    private:
        std::shared_ptr<LLVMInstruction> m_first;
        std::shared_ptr<LLVMInstruction> m_last;
};

} // namespace libscratchcpp
