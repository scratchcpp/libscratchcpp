// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/list.h>

#include "list_functions.h"

using namespace libscratchcpp;

extern "C"
{
    void list_clear(List *list)
    {
        list->clear();
    }

    void list_remove(List *list, size_t index)
    {
        list->removeAt(index);
    }

    ValueData *list_append_empty(List *list)
    {
        return &list->appendEmpty();
    }

    ValueData *list_insert_empty(List *list, size_t index)
    {
        return &list->insertEmpty(index);
    }

    ValueData *list_get_item(List *list, size_t index)
    {
        return &list->operator[](index);
    }

    size_t list_size(List *list)
    {
        return list->size();
    }
}
