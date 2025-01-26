#include "extension1.h"

using namespace libscratchcpp;

std::string Extension1::name() const
{
    return "ext 1";
}

Rgb Extension1::color() const
{
    return rgb(0, 0, 0);
}
