// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace libscratchcpp
{

class List;
struct ValueData;

extern "C"
{
    void list_clear(List *list);
    void list_remove(List *list, size_t index);

    ValueData *list_append_empty(List *list);
    ValueData *list_insert_empty(List *list, size_t index);

    ValueData *list_get_item(List *list, size_t index);
    size_t list_size(List *list);
}

} // namespace libscratchcpp
