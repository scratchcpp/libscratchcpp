// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/field.h>
#include <scratchcpp/entity.h>

#include "field_p.h"

using namespace libscratchcpp;

/*! Constructs Field. */
Field::Field(const std::string &name, const libscratchcpp::Value &value, std::shared_ptr<Entity> valuePtr) :
    impl(spimpl::make_unique_impl<FieldPrivate>(name, value, valuePtr))
{
}

/*! Constructs Field. */
Field::Field(const std::string &name, const libscratchcpp::Value &value, const std::string &valueId) :
    impl(spimpl::make_unique_impl<FieldPrivate>(name, value, valueId))
{
}

/*! \copydoc Field() */
Field::Field(const std::string &name, const libscratchcpp::Value &value, const char *valueId) :
    Field(name, value, std::string(valueId))
{
}

/*! Returns the name of the field. */
const std::string &Field::name() const
{
    return impl->name;
}

/*! Returns the ID of the field. */
int Field::fieldId() const
{
    return impl->fieldId;
}

/*! Sets the ID of the field. */
void Field::setFieldId(int newFieldId)
{
    impl->fieldId = newFieldId;
}

/*! Returns the value of the field. */
const Value &Field::value() const
{
    return impl->value;
}

/*! Returns a pointer to the value (e. g. a variable). */
std::shared_ptr<Entity> Field::valuePtr() const
{
    return impl->valuePtr;
}

/*! Sets the value pointer. */
void Field::setValuePtr(const std::shared_ptr<Entity> &newValuePtr)
{
    impl->valuePtr = newValuePtr;

    if (newValuePtr)
        impl->valueId = newValuePtr->id();
    else
        impl->valueId = "";
}

/*! Returns the ID of the value (e. g. a variable). */
const std::string &Field::valueId() const
{
    return impl->valueId;
}

/*!
 * Returns the ID that was assigned to the value.
 *  \see IBlockSection::addFieldValue()
 */
int Field::specialValueId() const
{
    return impl->specialValueId;
}

/*!
 * Assigns an ID to the value.
 *  \see IBlockSection::addFieldValue()
 */
void Field::setSpecialValueId(int newSpecialValueId)
{
    impl->specialValueId = newSpecialValueId;
}
