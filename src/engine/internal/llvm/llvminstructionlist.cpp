// SPDX-License-Identifier: Apache-2.0

#include <cassert>

#include "llvminstructionlist.h"
#include "llvminstruction.h"

using namespace libscratchcpp;

LLVMInstruction *LLVMInstructionList::first()
{
    return m_first.get();
}

LLVMInstruction *LLVMInstructionList::last()
{
    return m_last.get();
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
