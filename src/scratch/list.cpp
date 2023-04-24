// SPDX-License-Identifier: Apache-2.0

#include "list.h"
#include <bits/stdc++.h>

using namespace libscratchcpp;

/*! Constructs List. */
List::List(std::string id, std::string name) :
    m_name(name)
{
    setId(id);
}

/*! Returns the name of the list. */
std::string List::name()
{
    return m_name;
}

/*! Sets the name of the list. */
void List::setName(const std::string &name)
{
    m_name = name;
}

/*! Returns the list of items. */
std::deque<Value> List::items() const
{
    return m_items;
}

/*! Returns the item at index. */
Value List::at(int index) const
{
    return m_items[index];
}

/*! Returns the index of the given item. */
int List::indexOf(const Value &value) const
{
    auto it = std::find(m_items.begin(), m_items.end(), value);

    if (it != m_items.end())
        return it - m_items.begin();
    else
        return -1;
}

/*! Returns the number of items in the list. */
int List::size() const
{
    return m_items.size();
}

/*! \copydoc size() */
int List::length() const
{
    return size();
}

/*! \copydoc size() */
int List::count() const
{
    return size();
}

/*! Returns true if the list contains the given item. */
bool List::contains(const Value &value) const
{
    return (indexOf(value) != -1);
}

/*! Removes all items from the list. */
void List::clear()
{
    m_items.clear();
}

/*! Removes the item at index. */
void List::removeAt(int index)
{
    m_items.erase(m_items.begin() + index);
}

/*! Adds an item. */
void List::append(const Value &value)
{
    m_items.push_back(value);
}

/*! Inserts an item at index. */
void List::insert(int index, const Value &value)
{
    m_items.insert(m_items.begin() + index, value);
}

/*! Replaces the item at index. */
void List::replace(int index, const Value &value)
{
    m_items[index] = value;
}

/*! Joins the list items with spaces. */
std::string List::toString() const
{
    std::string ret;
    for (int i = 0; i < m_items.size(); i++) {
        ret.append(m_items[i].toString());
        if (i + 1 < m_items.size())
            ret.push_back(' ');
    }
    return ret;
}
