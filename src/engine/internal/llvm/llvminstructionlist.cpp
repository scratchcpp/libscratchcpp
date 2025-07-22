// SPDX-License-Identifier: Apache-2.0

#include <cassert>

#include "llvminstructionlist.h"
#include "llvminstruction.h"

using namespace libscratchcpp;

LLVMInstruction *LLVMInstructionList::first() const
{
    return m_first.get();
}

LLVMInstruction *LLVMInstructionList::last() const
{
    return m_last.get();
}

bool LLVMInstructionList::containsInstruction(LLVMInstruction *ins) const
{
    LLVMInstruction *ptr = m_first.get();

    while (ptr) {
        if (ptr == ins)
            return true;

        ptr = ptr->next;
    }

    return false;
}

bool LLVMInstructionList::containsInstruction(std::function<bool(const LLVMInstruction *)> func) const
{
    LLVMInstruction *ptr = m_first.get();

    while (ptr) {
        if (func(ptr))
            return true;

        ptr = ptr->next;
    }

    return false;
}

void LLVMInstructionList::addInstruction(std::shared_ptr<LLVMInstruction> ins)
{
    if (!(m_first && m_last)) {
        assert(!m_first && !m_last);

        m_first = ins;
        m_last = ins;

        ins->previous = nullptr;
        ins->next = nullptr;
    } else {
        m_last->next = ins.get();
        ins->previous = m_last.get();
        m_last = ins;
    }
}
