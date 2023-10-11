// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/variable.h>

#include "variable_p.h"

using namespace libscratchcpp;

/*! Constructs Variable. */
Variable::Variable(const std::string &id, const std::string &name, const Value &value, bool isCloudVariable) :
    Entity(id),
    impl(spimpl::make_unique_impl<VariablePrivate>(name, value, isCloudVariable))
{
}

/*! Returns the name of the variable. */
const std::string &Variable::name() const
{
    return impl->name;
}

/*! Returns the value. */
const Value &Variable::value() const
{
    return impl->value;
}

/*! Returns a pointer to the value. */
Value *Variable::valuePtr()
{
    return &impl->value;
}

/*! Sets the value. */
void Variable::setValue(const Value &value)
{
    impl->value = value;
}

/*! Returns true if the variable is a cloud variable. */
bool Variable::isCloudVariable() const
{
    return impl->isCloudVariable;
}

/*! Toggles whether the variable is a cloud variable. */
void Variable::setIsCloudVariable(bool isCloudVariable)
{
    impl->isCloudVariable = isCloudVariable;
}

/*! Creates a copy of the variable. */
std::shared_ptr<Variable> Variable::clone()
{
    return std::make_shared<Variable>(id(), impl->name, impl->value, impl->isCloudVariable);
}
