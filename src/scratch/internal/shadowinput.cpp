// SPDX-License-Identifier: Apache-2.0

#include "shadowinput.h"

namespace libscratchcpp
{

ShadowInput::ShadowInput(std::string name, Type type) :
    Input(name, type)
{
}

Value ShadowInput::value() const
{
    return m_primaryValue.value();
}

} // namespace libscratchcpp
