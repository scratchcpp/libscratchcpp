// SPDX-License-Identifier: Apache-2.0

#include "broadcast.h"

using namespace libscratchcpp;

/*! Constructs Broadcast. */
Broadcast::Broadcast(const std::string &id, const std::string &name) :
    Entity(id),
    m_name(name)
{
}

/*! Returns the name of the broadcast. */
const std::string &Broadcast::name() const
{
    return m_name;
}

/*! Sets the name of the broadcast. */
void Broadcast::setName(const std::string &newName)
{
    m_name = newName;
}
