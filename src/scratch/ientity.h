// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <string>

namespace libscratchcpp
{

/*! \brief The IEntity class can be used for anything that is identified by an ID. */
class LIBSCRATCHCPP_EXPORT IEntity
{
    public:
        virtual ~IEntity() { }

        /*! Returns the ID. */
        virtual const std::string &id() const final { return m_id; };
        /*! Sets the ID. */
        virtual void setId(const std::string &newId) final { m_id = newId; };

    private:
        std::string m_id;
};

} // namespace libscratchcpp
