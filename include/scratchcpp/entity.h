// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class EntityPrivate;

/*! \brief The Entity class is the base class of everything that is identified by an ID (Target, Variable, List, etc.). */
class LIBSCRATCHCPP_EXPORT Entity
{
    public:
        Entity(const std::string &id);
        virtual ~Entity() { }

        const std::string &id() const;

        void setId(const std::string &newId);

    private:
        spimpl::impl_ptr<EntityPrivate> impl;
};

} // namespace libscratchcpp
