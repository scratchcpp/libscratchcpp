// SPDX-License-Identifier: Apache-2.0

#include "variableinput.h"
#include "../variable.h"

namespace libscratchcpp
{

VariableInput::VariableInput(std::string name, Type type) :
    Input(name, type)
{
}

Value VariableInput::value() const
{
    return reinterpret_cast<Variable *>(m_primaryValue.valuePtr().get())->value();
}

} // namespace libscratchcpp
