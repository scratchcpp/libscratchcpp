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
}
