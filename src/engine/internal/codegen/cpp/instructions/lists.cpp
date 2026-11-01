// SPDX-License-Identifier: Apache-2.0

#include "lists.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Lists::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::ClearList:
            ret.next = buildClearList(ins);
            break;

        case CppInstruction::Type::RemoveListItem:
            ret.next = buildRemoveListItem(ins);
            break;

        case CppInstruction::Type::AppendToList:
            ret.next = buildAppendToList(ins);
            break;

        case CppInstruction::Type::InsertToList:
            ret.next = buildInsertToList(ins);
            break;

        case CppInstruction::Type::ListReplace:
            ret.next = buildListReplace(ins);
            break;

        case CppInstruction::Type::GetListContents:
            ret.next = buildGetListContents(ins);
            break;

        case CppInstruction::Type::GetListItem:
            ret.next = buildGetListItem(ins);
            break;

        case CppInstruction::Type::GetListSize:
            ret.next = buildGetListSize(ins);
            break;

        case CppInstruction::Type::GetListItemIndex:
            ret.next = buildGetListItemIndex(ins);
            break;

        case CppInstruction::Type::ListContainsItem:
            ret.next = buildListContainsItem(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Lists::buildClearList(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildRemoveListItem(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildAppendToList(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildInsertToList(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildListReplace(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildGetListContents(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildGetListItem(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildGetListSize(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildGetListItemIndex(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Lists::buildListContainsItem(CppInstruction *ins)
{
    return ins->getNext();
}
