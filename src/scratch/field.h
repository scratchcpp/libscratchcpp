// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>
#include <memory>

namespace libscratchcpp
{

class Entity;

/*! \brief The Field class represents a Scratch block field. */
class LIBSCRATCHCPP_EXPORT Field
{
    public:
        Field(std::string name, Value value, std::shared_ptr<Entity> valuePtr = nullptr);
        Field(std::string name, Value value, std::string valueId);
        Field(std::string name, Value value, const char *valueId);
        Field(const Field &) = delete;

        std::string name() const;

        int fieldId() const;
        void setFieldId(int newFieldId);

        Value value() const;

        std::shared_ptr<Entity> valuePtr() const;
        void setValuePtr(const std::shared_ptr<Entity> &newValuePtr);

        std::string valueId() const;

        int specialValueId() const;
        void setSpecialValueId(int newSpecialValueId);

    private:
        std::string m_name;
        int m_fieldId = -1;
        Value m_value;
        std::shared_ptr<Entity> m_valuePtr = nullptr;
        std::string m_valueId;
        int m_specialValueId = -1;
};

} // namespace libscratchcpp
