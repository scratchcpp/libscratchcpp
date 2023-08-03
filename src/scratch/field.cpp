// SPDX-License-Identifier: Apache-2.0

#include "field.h"

using namespace libscratchcpp;

/*! Constructs Field. */
Field::Field(std::string name, Value value, std::shared_ptr<Entity> valuePtr) :
    m_name(name),
    m_value(value),
    m_valuePtr(valuePtr)
{
}

/*! Constructs Field. */
Field::Field(std::string name, Value value, std::string valueId) :
    m_name(name),
    m_value(value),
    m_valueId(valueId)
{
}

/*! \copydoc Field() */
Field::Field(std::string name, Value value, const char *valueId) :
    Field(name, value, std::string(valueId))
{
}

/*! Returns the name of the field. */
std::string Field::name() const
{
    return m_name;
}

/*! Returns the ID of the field. */
int Field::fieldId() const
{
    return m_fieldId;
}

/*! Sets the ID of the field. */
void Field::setFieldId(int newFieldId)
{
    m_fieldId = newFieldId;
}

/*! Returns the value of the field. */
Value Field::value() const
{
    return m_value;
}

/*! Returns a pointer to the value (e. g. a variable). */
std::shared_ptr<Entity> Field::valuePtr() const
{
    return m_valuePtr;
}

/*! Sets the value pointer. */
void Field::setValuePtr(const std::shared_ptr<Entity> &newValuePtr)
{
    m_valuePtr = newValuePtr;
}

/*! Returns the ID of the value (e. g. a variable). */
std::string Field::valueId() const
{
    return m_valueId;
}

/*!
 * Returns the ID that was assigned to the value.
 *  \see IBlockSection::addFieldValue()
 */
int Field::specialValueId() const
{
    return m_specialValueId;
}

/*!
 * Assigns an ID to the value.
 *  \see IBlockSection::addFieldValue()
 */
void Field::setSpecialValueId(int newSpecialValueId)
{
    m_specialValueId = newSpecialValueId;
}
