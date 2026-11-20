// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "cppnamedidentifier.h"
#include "cppcodestream.h"

namespace libscratchcpp
{

class CppFunctionCall
{
    public:
        CppFunctionCall(CppCodeStream &out, const std::string &name) :
            m_out(out)
        {
            m_out.addToLine(name);
            m_out.addToLine("(");
        }

        CppFunctionCall(CppCodeStream &out, const CppNamedIdentifier *identifier, const std::string &name) :
            m_out(out)
        {
            if (!identifier->isMemberAccessible()) {
                throw std::runtime_error("cannot call method '" + name + "' on '" + name + "'");
            }

            m_out.addToLine(identifier->name);
            m_out.addToLine(identifier->isPointer() ? "->" : ".");
            m_out.addToLine(name);
            m_out.addToLine("(");
        }

        ~CppFunctionCall()
        {
            if (!m_closed)
                m_out.addToLine(")");
        }

        CppFunctionCall(const CppFunctionCall &) = delete;
        CppFunctionCall &operator=(const CppFunctionCall &) = delete;

        CppFunctionCall &param(const CppNamedIdentifier *p)
        {
            sep();
            m_out.addToLine(p->name);
            return *this;
        }

        CppFunctionCall &param(const std::string &expr)
        {
            sep();
            m_out.addToLine(expr);
            return *this;
        }

        void close()
        {
            m_closed = true;
            m_out.addToLine(")");
        }

    private:
        void sep()
        {
            if (!m_first)
                m_out.addToLine(", ");

            m_first = false;
        }

        CppCodeStream &m_out;
        bool m_first = true;
        bool m_closed = false;
};

} // namespace libscratchcpp
