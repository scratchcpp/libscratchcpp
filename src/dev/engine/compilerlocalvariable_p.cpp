// SPDX-License-Identifier: Apache-2.0

#include <cassert>

#include "compilerlocalvariable_p.h"

using namespace libscratchcpp;

CompilerLocalVariablePrivate::CompilerLocalVariablePrivate(CompilerValue *ptr) :
    ptr(ptr)
{
    assert(ptr);
}
