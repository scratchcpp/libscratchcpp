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

    ValueData *list_data(List *list)
    {
        return list->data();
    }

    ValueData *const *list_data_ptr(List *list)
    {
        return list->dataPtr();
    }

    size_t *list_size_ptr(List *list)
    {
        return list->sizePtr();
    }

    const size_t *list_alloc_size_ptr(List *list)
    {
        return list->allocatedSizePtr();
    }

    size_t list_size(List *list)
    {
        return list->size();
    }

    void list_to_string(List *list, StringPtr *dst)
    {
        list->toStringPtr(dst);
    }
}
