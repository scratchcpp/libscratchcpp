// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace libscratchcpp
{

class List;
struct ValueData;
struct StringPtr;

extern "C"
{
    void list_clear(List *list);
    void list_remove(List *list, size_t index);

    ValueData *list_append_empty(List *list);
    ValueData *list_insert_empty(List *list, size_t index);

    ValueData *list_get_item(List *list, size_t index);
    ValueData *list_data(List *list);
    size_t *list_size_ptr(List *list);
    const size_t *list_alloc_size_ptr(List *list);
    size_t list_size(List *list);

    StringPtr *list_to_string(List *list);
}

} // namespace libscratchcpp
