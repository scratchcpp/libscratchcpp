// SPDX-License-Identifier: Apache-2.0

#include "variable.h"

using namespace libscratchcpp;

/*! Constructs Variable. */
Variable::Variable(const std::string &id, const std::string &name, const Value &value, bool isCloudVariable) :
    Entity(id),
    m_name(name),
    m_value(value),
    m_isCloudVariable(isCloudVariable)
{
}

/*! Constructs an empty Variable. */
Variable::Variable(const std::string &id, const std::string &name, bool isCloudVariable) :
    Variable(id, name, Value(), isCloudVariable)
{
}

/*! Returns the name of the variable. */
const std::string &Variable::name() const
{
    return m_name;
}

/*! Returns true if the variable is a cloud variable. */
bool Variable::isCloudVariable() const
{
    return m_isCloudVariable;
}

/*! Toggles whether the variable is a cloud variable. */
void Variable::setIsCloudVariable(bool isCloudVariable)
{
    m_isCloudVariable = isCloudVariable;
}
