// SPDX-License-Identifier: Apache-2.0

#include "inputvalue_p.h"

using namespace libscratchcpp;

InputValuePrivate::InputValuePrivate()
{
}

InputValuePrivate::InputValuePrivate(InputValue::Type type) :
    type(type)
{
}
