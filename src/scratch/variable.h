// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include "value.h"
#include <string>

namespace libscratchcpp
{

/*! \brief The Variable class represents a Scratch variable. */
class LIBSCRATCHCPP_EXPORT Variable : public IEntity
{
    public:
        Variable(const std::string &id, const std::string &name, const Value &value = Value(), bool isCloudVariable = false);
        Variable(const std::string &id, const std::string &name, bool isCloudVariable);
        Variable(const Variable &) = delete;

        const std::string &name() const;

        /*! Returns the value. */
        inline const Value &value() const { return m_value; }

        /*! Returns a pointer to the value. */
        inline Value *valuePtr() { return &m_value; }

        /*! Sets the value. */
        inline void setValue(const Value &value) { m_value = value; }

        bool isCloudVariable() const;
        void setIsCloudVariable(bool isCloudVariable);

        /*! Adds the given value to the variable's value. \see Value::add() */
        inline void add(const Value &v) { m_value.add(v); }

    private:
        std::string m_name;
        Value m_value;
        bool m_isCloudVariable;
};

} // namespace libscratchcpp
