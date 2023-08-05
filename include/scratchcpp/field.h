// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <memory>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Entity;
class Value;
class FieldPrivate;

/*! \brief The Field class represents a Scratch block field. */
class LIBSCRATCHCPP_EXPORT Field
{
    public:
        Field(const std::string &name, const Value &value, std::shared_ptr<Entity> valuePtr = nullptr);
        Field(const std::string &name, const Value &value, const std::string &valueId);
        Field(const std::string &name, const Value &value, const char *valueId);
        Field(const Field &) = delete;

        const std::string &name() const;

        int fieldId() const;
        void setFieldId(int newFieldId);

        const Value &value() const;

        std::shared_ptr<Entity> valuePtr() const;
        void setValuePtr(const std::shared_ptr<Entity> &newValuePtr);

        const std::string &valueId() const;

        int specialValueId() const;
        void setSpecialValueId(int newSpecialValueId);

    private:
        spimpl::unique_impl_ptr<FieldPrivate> impl;
};

} // namespace libscratchcpp
