// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "cppnamedidentifier.h"
#include "cppcodestream.h"

namespace libscratchcpp
{

class CppSwitch
{
    public:
        CppSwitch(CppCodeStream &out, const CppNamedIdentifier *identifier) :
            m_out(out)
        {
            m_out.beginLine();
            m_out.addToLine("switch (");
            m_out.addToLine(identifier->name);
            m_out.addToLine(") {");
            m_out.indent();
        }

        ~CppSwitch()
        {
            if (!m_closed) {
                close();
            }
        }

        CppSwitch(const CppSwitch &) = delete;
        CppSwitch &operator=(const CppSwitch &) = delete;

        CppSwitch &addCase(const std::string &value)
        {
            if (m_inCase)
                m_out.dedent();

            m_inCase = true;

            m_out.beginLine();
            m_out.addToLine("case ");
            m_out.addToLine(value);
            m_out.addToLine(":");

            m_out.indent();

            return *this;
        }

        CppSwitch &addDefaultCase(const std::string &value)
        {
            if (m_inCase)
                m_out.dedent();

            m_inCase = true;

            m_out.beginLine();
            m_out.addToLine("default:");
            m_out.indent();

            return *this;
        }

        void close()
        {
            if (m_inCase)
                m_out.dedent();

            m_closed = true;
            m_out.dedent();
        }

    private:
        CppCodeStream &m_out;
        bool m_inCase = false;
        bool m_closed = false;
};

} // namespace libscratchcpp
