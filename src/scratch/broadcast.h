// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/entity.h>
#include <string>

namespace libscratchcpp
{

/*! \brief The Broadcast class represents a Scratch broadcast. */
class LIBSCRATCHCPP_EXPORT Broadcast : public Entity
{
    public:
        Broadcast(const std::string &id, const std::string &name);
        Broadcast(const Broadcast &) = delete;

        const std::string &name() const;
        void setName(const std::string &newName);

    private:
        std::string m_name;
};

} // namespace libscratchcpp
