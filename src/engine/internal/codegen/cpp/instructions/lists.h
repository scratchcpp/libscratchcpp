// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

#include "instructiongroup.h"

namespace libscratchcpp
{

namespace cppins
{

class Lists : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildClearList(CppInstruction *ins);
        CppInstruction *buildRemoveListItem(CppInstruction *ins);
        CppInstruction *buildAppendToList(CppInstruction *ins);
        CppInstruction *buildInsertToList(CppInstruction *ins);
        CppInstruction *buildListReplace(CppInstruction *ins);
        CppInstruction *buildGetListContents(CppInstruction *ins);
        CppInstruction *buildGetListItem(CppInstruction *ins);
        CppInstruction *buildGetListSize(CppInstruction *ins);
        CppInstruction *buildGetListItemIndex(CppInstruction *ins);
        CppInstruction *buildListContainsItem(CppInstruction *ins);
};

} // namespace cppins

} // namespace libscratchcpp
