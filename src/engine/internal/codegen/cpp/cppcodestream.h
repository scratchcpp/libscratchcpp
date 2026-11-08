// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <sstream>

namespace libscratchcpp
{

class CppCodeStream
{
    public:
        inline void beginLine() { m_code << std::string(m_indentLevel * 4, ' '); }
        inline void addToLine(const std::string &code) { m_code << code; }
        inline void endLine() { m_code << "\n"; }

        inline void indent() { m_indentLevel++; };
        inline void dedent() { m_indentLevel--; };

        inline std::string code() const
        {
            // TODO: Avoid copying the code
            return m_code.str();
        };

    private:
        std::ostringstream m_code;
        int m_indentLevel = 0;
};

} // namespace libscratchcpp
