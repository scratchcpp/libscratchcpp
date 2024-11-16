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
}

} // namespace libscratchcpp
