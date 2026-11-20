// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "cppnamedidentifier.h"
#include "cppcodestream.h"

namespace libscratchcpp
{

class CppAssignment
{
    public:
        CppAssignment(CppCodeStream &out, const CppNamedIdentifier *dst) :
            m_out(out)
        {
            if (!dst->isMutable) {
                throw std::runtime_error("cannot assign to '" + dst->name + "'");
            }

            m_out.addToLine(dst->name);
            m_out.addToLine(" = ");
        }

        CppAssignment(CppCodeStream &out, const CppNamedIdentifier *dstObj, const std::string &dstMember) :
            m_out(out)
        {
            if (!dstObj->isMutable) {
                throw std::runtime_error("cannot assign to member of '" + dstObj->name + "'");
            }

            m_out.addToLine(dstObj->name);
            m_out.addToLine(".");
            m_out.addToLine(dstMember);
            m_out.addToLine(" = ");
        }

        ~CppAssignment()
        {
            if (!m_closed)
                close();
        }

        CppAssignment(const CppAssignment &) = delete;
        CppAssignment &operator=(const CppAssignment &) = delete;

        void close()
        {
            m_closed = true;
            m_out.addToLine(";");
        }

    private:
        CppCodeStream &m_out;
        bool m_closed = false;
};

} // namespace libscratchcpp
