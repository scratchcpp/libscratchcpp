// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include "value.h"
#include <string>
#include <vector>

namespace libscratchcpp
{

/*! \brief The List class represents a Scratch list. */
class LIBSCRATCHCPP_EXPORT List : public IEntity
{
    public:
        List(std::string id, std::string name);
        List(const List &) = delete;

        std::string name();
        void setName(const std::string &name);

        std::vector<Value> items() const;
        Value at(int index) const;
        int indexOf(const Value &value) const;
        int size() const;
        int length() const;
        int count() const;
        bool contains(const Value &value) const;

        void clear();
        void removeAt(int index);
        void append(const Value &value);
        void insert(int index, const Value &value);
        void replace(int index, const Value &value);

    private:
        std::string m_name;
        std::vector<Value> m_items;
};

} // namespace libscratchcpp
