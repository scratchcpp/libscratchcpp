#include "instructionlistbase.h"

using namespace libscratchcpp;

bool InstructionListBase::containsInstruction(InstructionBase *ins) const
{
    InstructionBase *ptr = m_first.get();

    while (ptr) {
        if (ptr == ins)
            return true;

        ptr = ptr->next;
    }

    return false;
}

bool InstructionListBase::containsInstruction(std::function<bool(const InstructionBase *)> func) const
{
    InstructionBase *ptr = m_first.get();

    while (ptr) {
        if (func(ptr))
            return true;

        ptr = ptr->next;
    }

    return false;
}

void InstructionListBase::addInstruction(std::shared_ptr<InstructionBase> ins)
{
    if (!(m_first && m_last)) {
        assert(!m_first && !m_last);

        m_first = ins;
        m_last = ins;

        ins->previous = nullptr;
        ins->next = nullptr;
        ins->_next = nullptr;
    } else {
        m_last->next = ins.get();
        m_last->_next = ins;
        ins->previous = m_last.get();
        m_last = ins;
    }
}
