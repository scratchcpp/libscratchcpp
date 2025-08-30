// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>

using namespace libscratchcpp;

StringPtr::StringPtr(const std::string &str)
{
    string_assign_cstring(this, str.c_str());
}
