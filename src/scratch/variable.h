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
        Variable(std::string id, std::string name, Value value = Value(), bool isCloudVariable = false);
        Variable(std::string id, std::string name, bool isCloudVariable);
        Variable(const Variable &) = delete;

        std::string name() const;

        const Value &value() const;
        void setValue(const Value &value);

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
