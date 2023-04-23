// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include <string>

namespace libscratchcpp
{

/*! \brief The Broadcast class represents a Scratch broadcast. */
class LIBSCRATCHCPP_EXPORT Broadcast : public IEntity
{
    public:
        Broadcast(std::string id, std::string name);
        Broadcast(const Broadcast &) = delete;

        std::string name() const;
        void setName(const std::string &newName);

    private:
        std::string m_name;
};

} // namespace libscratchcpp
