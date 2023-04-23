// SPDX-License-Identifier: Apache-2.0

#include "broadcast.h"

using namespace libscratchcpp;

/*! Constructs Broadcast. */
Broadcast::Broadcast(std::string id, std::string name) :
    m_name(name)
{
    setId(id);
}

/*! Returns the name of the broadcast. */
std::string Broadcast::name() const
{
    return m_name;
}

/*! Sets the name of the broadcast. */
void Broadcast::setName(const std::string &newName)
{
    m_name = newName;
}
