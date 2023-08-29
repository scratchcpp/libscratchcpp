// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "entity.h"
#include "spimpl.h"
#include "global.h"

namespace libscratchcpp
{

class AssetPrivate;

/*! \brief The Asset class represents a Scratch asset, for example a Costume or a Sound. */
class LIBSCRATCHCPP_EXPORT Asset : public Entity
{
    public:
        Asset(const std::string &name, const std::string &id, const std::string &format);
        Asset(const Asset &) = delete;

        void setId(const std::string &id);

        const std::string &name() const;

        const std::string &md5ext() const;

        const std::string &dataFormat() const;

    private:
        spimpl::impl_ptr<AssetPrivate> impl;
};

} // namespace libscratchcpp
