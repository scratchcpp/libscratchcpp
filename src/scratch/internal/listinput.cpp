// SPDX-License-Identifier: Apache-2.0

#include "listinput.h"
#include "../list.h"

namespace libscratchcpp
{

ListInput::ListInput(std::string name, Type type) :
    Input(name, type)
{
}

Value ListInput::value() const
{
    return reinterpret_cast<List *>(m_primaryValue.valuePtr().get())->toString();
}

} // namespace libscratchcpp
