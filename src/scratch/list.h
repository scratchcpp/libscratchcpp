// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ientity.h"
#include "value.h"
#include <string>
#include <deque>

namespace libscratchcpp
{

/*! \brief The List class represents a Scratch list. */
class LIBSCRATCHCPP_EXPORT List
    : public std::deque<Value>
    , public IEntity
{
    public:
        List(std::string id, std::string name);
        List(const List &) = delete;

        std::string name();
        void setName(const std::string &name);

        size_t indexOf(const Value &value) const;
        bool contains(const Value &value) const;

        /*! Removes the item at index. */
        inline void removeAt(int index) { erase(begin() + index); }

        /*! Inserts an item at index. */
        inline void insert(int index, const Value &value) { std::deque<Value>::insert(begin() + index, value); }

        /*! Replaces the item at index. */
        inline void replace(int index, const Value &value) { at(index) = value; }

        std::string toString() const;

    private:
        std::string m_name;
};

} // namespace libscratchcpp
