// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/list.h>

#include "list_functions.h"

using namespace libscratchcpp;

extern "C"
{
    LIBSCRATCHCPP_EXPORT void list_clear(List *list)
    {
        list->clear();
    }

    LIBSCRATCHCPP_EXPORT void list_remove(List *list, size_t index)
    {
        list->removeAt(index);
    }

    LIBSCRATCHCPP_EXPORT ValueData *list_append_empty(List *list)
    {
        return &list->appendEmpty();
    }

    LIBSCRATCHCPP_EXPORT ValueData *list_insert_empty(List *list, size_t index)
    {
        return &list->insertEmpty(index);
    }

    LIBSCRATCHCPP_EXPORT ValueData *list_get_item(List *list, size_t index)
    {
        return &list->operator[](index);
    }

    LIBSCRATCHCPP_EXPORT ValueData *list_data(List *list)
    {
        return list->data();
    }

    LIBSCRATCHCPP_EXPORT ValueData *const *list_data_ptr(List *list)
    {
        return list->dataPtr();
    }

    LIBSCRATCHCPP_EXPORT size_t *list_size_ptr(List *list)
    {
        return list->sizePtr();
    }

    LIBSCRATCHCPP_EXPORT const size_t *list_alloc_size_ptr(List *list)
    {
        return list->allocatedSizePtr();
    }

    LIBSCRATCHCPP_EXPORT size_t list_size(List *list)
    {
        return list->size();
    }

    LIBSCRATCHCPP_EXPORT void list_to_string(List *list, StringPtr *dst)
    {
        list->toStringPtr(dst);
    }
}
