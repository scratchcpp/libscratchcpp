// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/entity.h>

#include "entity_p.h"

using namespace libscratchcpp;

/*! Constructs Entity. */
Entity::Entity(const std::string &id) :
    impl(spimpl::make_unique_impl<EntityPrivate>(id))
{
}

/*! Returns the ID. */
const std::string &Entity::id() const
{
    return impl->id;
}

/*! Sets the ID. */
void Entity::setId(const std::string &newId)
{
    impl->id = newId;
}
