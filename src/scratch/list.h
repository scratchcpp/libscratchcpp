// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include "value.h"
#include <string>
#include <deque>

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

        std::deque<Value> items() const;
        const Value &at(int index) const;
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

        std::string toString() const;

    private:
        std::string m_name;
        std::deque<Value> m_items;
};

} // namespace libscratchcpp
