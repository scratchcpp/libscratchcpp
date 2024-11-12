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
}
