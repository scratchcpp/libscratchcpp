#include <scratchcpp/value.h>
#include <utf8.h>

using namespace libscratchcpp;

std::u16string Value::toUtf16() const
{
    return utf8::utf8to16(toString());
}
