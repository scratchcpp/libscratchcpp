// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <deque>

#include "value.h"
#include "entity.h"

namespace libscratchcpp
{

class ListPrivate;

/*! \brief The List class represents a Scratch list. */
class LIBSCRATCHCPP_EXPORT List
    : public std::deque<Value>
    , public Entity
{
    public:
        List(const std::string &id, const std::string &name);
        List(const List &) = delete;

        const std::string &name();
        void setName(const std::string &name);

        long indexOf(const Value &value) const;
        bool contains(const Value &value) const;

        /*! Removes the item at index. */
        void removeAt(int index) { erase(begin() + index); }

        /*! Inserts an item at index. */
        void insert(int index, const Value &value) { std::deque<Value>::insert(begin() + index, value); }

        /*! Replaces the item at index. */
        void replace(int index, const Value &value) { at(index) = value; }

        std::string toString() const;

    private:
        spimpl::unique_impl_ptr<ListPrivate> impl;
};

} // namespace libscratchcpp
