// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "entity.h"
#include "spimpl.h"
#include "global.h"

namespace libscratchcpp
{

class Target;
class AssetPrivate;

/*! \brief The Asset class represents a Scratch asset, for example a Costume or a Sound. */
class LIBSCRATCHCPP_EXPORT Asset : public Entity
{
    public:
        Asset(const std::string &name, const std::string &id, const std::string &format);
        Asset(const Asset &) = delete;

        virtual ~Asset() { }

        void setId(const std::string &id);

        const std::string &name() const;

        const std::string &fileName() const;

        const std::string &dataFormat() const;

        const void *data() const;
        unsigned int dataSize() const;
        void setData(unsigned int size, void *data);

        Target *target() const;
        void setTarget(Target *target);

    protected:
        virtual void processData(unsigned int size, void *data) { }

    private:
        spimpl::unique_impl_ptr<AssetPrivate> impl;
};

} // namespace libscratchcpp
