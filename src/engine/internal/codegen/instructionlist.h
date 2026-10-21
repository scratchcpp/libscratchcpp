// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <functional>

#include "test_export.h"

namespace libscratchcpp
{

template<typename TInstruction>
class LIBSCRATCHCPP_TEST_EXPORT InstructionList
{
    public:
        InstructionList() = default;
        InstructionList(const InstructionList &) = delete;

        inline TInstruction *first() const { return m_first.get(); }

        inline TInstruction *last() const { return m_last.get(); }

        inline bool empty() const { return !first(); }

        inline bool containsInstruction(TInstruction *ins) const
        {
            TInstruction *ptr = m_first.get();

            while (ptr) {
                if (ptr == ins)
                    return true;

                ptr = ptr->next;
            }

            return false;
        }

        inline bool containsInstruction(std::function<bool(const TInstruction *)> func) const
        {
            TInstruction *ptr = m_first.get();

            while (ptr) {
                if (func(ptr))
                    return true;

                ptr = ptr->next;
            }

            return false;
        }

        inline void addInstruction(std::shared_ptr<TInstruction> ins)
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

    private:
        std::shared_ptr<TInstruction> m_first;
        std::shared_ptr<TInstruction> m_last;
};

} // namespace libscratchcpp
