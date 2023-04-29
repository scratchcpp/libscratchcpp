// SPDX-License-Identifier: Apache-2.0

#include "variable.h"

using namespace libscratchcpp;

/*! Constructs Variable. */
Variable::Variable(std::string id, std::string name, Value value, bool isCloudVariable) :
    m_name(name),
    m_value(value),
    m_isCloudVariable(isCloudVariable)
{
    setId(id);
}

/*! Constructs an empty Variable. */
Variable::Variable(std::string id, std::string name, bool isCloudVariable) :
    Variable(id, name, Value(), isCloudVariable)
{
}

/*! Returns the name of the variable. */
std::string Variable::name() const
{
    return m_name;
}

/*! Returns the value. */
const Value &Variable::value() const
{
    return m_value;
}

/*! Sets the value. */
void Variable::setValue(const Value &value)
{
    m_value = value;
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
