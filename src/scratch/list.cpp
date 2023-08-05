// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/list.h>
#include <algorithm>

#include "list_p.h"

using namespace libscratchcpp;

/*! Constructs List. */
List::List(const std::string &id, const std::string &name) :
    Entity(id),
    impl(spimpl::make_unique_impl<ListPrivate>(name))
{
}

/*! Returns the name of the list. */
const std::string &List::name()
{
    return impl->name;
}

/*! Sets the name of the list. */
void List::setName(const std::string &name)
{
    impl->name = name;
}

/*! Returns the index of the given item. */
long List::indexOf(const Value &value) const
{
    auto it = std::find(begin(), end(), value);

    if (it != end())
        return it - begin();
    else
        return -1;
}

/*! Returns true if the list contains the given item. */
bool List::contains(const Value &value) const
{
    return (indexOf(value) != -1);
}

/*! Joins the list items with spaces. */
std::string List::toString() const
{
    std::string ret;
    for (int i = 0; i < size(); i++) {
        ret.append(at(i).toString());
        if (i + 1 < size())
            ret.push_back(' ');
    }
    return ret;
}
