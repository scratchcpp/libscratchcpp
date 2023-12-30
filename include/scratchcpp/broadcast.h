// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "entity.h"

namespace libscratchcpp
{

class BroadcastPrivate;

/*! \brief The Broadcast class represents a Scratch broadcast. */
class LIBSCRATCHCPP_EXPORT Broadcast : public Entity
{
    public:
        Broadcast(const std::string &id, const std::string &name, bool isBackdropBroadcast = false);
        Broadcast(const Broadcast &) = delete;

        const std::string &name() const;
        void setName(const std::string &newName);

        bool isBackdropBroadcast() const;

    private:
        spimpl::unique_impl_ptr<BroadcastPrivate> impl;
};

} // namespace libscratchcpp
