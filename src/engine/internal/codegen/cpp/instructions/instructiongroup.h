// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "processresult.h"

namespace libscratchcpp
{

class CppCodeGenerator;

namespace cppins
{

class InstructionGroup
{
    public:
        InstructionGroup(CppCodeGenerator &codeGen);

        virtual ProcessResult process(CppInstruction *ins) = 0;

    protected:
        CppCodeGenerator &m_codeGen;
};

} // namespace cppins

} // namespace libscratchcpp
