// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include "value.h"
#include <memory>

namespace libscratchcpp
{

/*! \brief The Field class represents a Scratch block field. */
class LIBSCRATCHCPP_EXPORT Field
{
    public:
        Field(std::string name, Value value, std::shared_ptr<IEntity> valuePtr = nullptr);
        Field(std::string name, Value value, std::string valueId);
        Field(std::string name, Value value, const char *valueId);
        Field(const Field &) = delete;

        std::string name() const;

        int fieldId() const;
        void setFieldId(int newFieldId);

        Value value() const;

        std::shared_ptr<IEntity> valuePtr() const;
        void setValuePtr(const std::shared_ptr<IEntity> &newValuePtr);

        std::string valueId() const;

    private:
        std::string m_name;
        int m_fieldId = -1;
        Value m_value;
        std::shared_ptr<IEntity> m_valuePtr = nullptr;
        std::string m_valueId;
};

} // namespace libscratchcpp
