// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/value.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/target.h>
#include <scratchcpp/executioncontext.h>
#include <utf8.h>
#include <iostream>

using namespace libscratchcpp;

// Functions called by the generated test code
extern "C"
{
    void cpp_test_empty()
    {
        std::cout << "empty" << std::endl;
    }

    double cpp_test_add(double a, double b)
    {
        return a + b;
    }

    StringPtr *cpp_test_string(const StringPtr *str)
    {
        StringPtr *ret = string_pool_new();
        std::string s = Value(str->data ? utf8::utf16to8(std::u16string(str->data)) : "").toString();
        std::string result = "string: " + s;
        string_assign_cstring(ret, result.c_str());
        return ret;
    }

    ValueData cpp_test_value(ValueData *v, bool flag)
    {
        ValueData ret;
        value_init(&ret);
        std::string str = "value: " + Value(*v).toString() + (flag ? " true" : " false");
        value_assign_cstring(&ret, str.c_str());
        return ret;
    }

    void cpp_test_target(Target *target)
    {
        std::cout << "target: " << (target != nullptr) << std::endl;
    }

    bool cpp_test_ctx(ExecutionContext *ctx)
    {
        return ctx != nullptr;
    }
}
