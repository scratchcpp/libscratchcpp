// SPDX-License-Identifier: Apache-2.0

#include "blockinput.h"
#include "../block.h"

namespace libscratchcpp
{

BlockInput::BlockInput(std::string name, Type type) :
    Input(name, type)
{
}

Value BlockInput::value() const
{
    return m_primaryValue.valueBlock()->run();
}

} // namespace libscratchcpp
