// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <scratchcpp/value.h>
#include <scratchcpp/entity.h>

namespace libscratchcpp
{

class VariablePrivate;

/*! \brief The Variable class represents a Scratch variable. */
class LIBSCRATCHCPP_EXPORT Variable : public Entity
{
    public:
        Variable(const std::string &id, const std::string &name, const Value &value = Value(), bool isCloudVariable = false);
        Variable(const Variable &) = delete;

        const std::string &name() const;

        const Value &value() const;

        Value *valuePtr();

        void setValue(const Value &value);

        bool isCloudVariable() const;
        void setIsCloudVariable(bool isCloudVariable);

    private:
        spimpl::unique_impl_ptr<VariablePrivate> impl;
};

} // namespace libscratchcpp
