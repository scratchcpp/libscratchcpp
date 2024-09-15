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
    m_dataPtr = &impl->data;
}

/*! Destroys List. */
List::~List()
{
    clear();
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

/*! Returns the sprite or stage this list belongs to. */
Target *List::target() const
{
    return impl->target;
}

/*! Sets the sprite or stage this list belongs to. */
void List::setTarget(Target *target)
{
    impl->target = target;
}

/*! Returns the monitor of this list. */
Monitor *List::monitor() const
{
    return impl->monitor;
}

/*! Sets the monitor of this list. */
void List::setMonitor(Monitor *monitor)
{
    impl->monitor = monitor;
}

/*! Joins the list items with spaces or without any separator if there are only digits. */
std::string List::toString() const
{
    std::string ret;
    veque::veque<std::string> strings;
    strings.reserve(m_dataPtr->size());
    bool digits = true;

    for (const auto &item : *m_dataPtr) {
        strings.push_back(std::string());
        value_toString(&item, &strings.back());

        if (value_isValidNumber(&item) && !strings.back().empty()) {
            double doubleNum = value_toDouble(&item);
            long num = value_toLong(&item);

            if (doubleNum != num) {
                digits = false;
                break;
            }

            if (num < 0 || num >= 10) {
                digits = false;
                break;
            }
        } else {
            digits = false;
            break;
        }
    }

    size_t i;
    std::string s;

    if (digits) {
        for (i = 0; i < strings.size(); i++)
            ret.append(strings[i]);

        for (; i < m_dataPtr->size(); i++) {
            value_toString(&m_dataPtr->operator[](i), &s);
            ret.append(s);
        }
    } else {
        for (i = 0; i < strings.size(); i++) {
            ret.append(strings[i]);

            if (i + 1 < m_dataPtr->size())
                ret.push_back(' ');
        }

        for (; i < m_dataPtr->size(); i++) {
            value_toString(&m_dataPtr->operator[](i), &s);
            ret.append(s);

            if (i + 1 < m_dataPtr->size())
                ret.push_back(' ');
        }
    }

    return ret;
}

/*! Creates a copy of the list. */
std::shared_ptr<List> List::clone()
{
    auto copy = std::make_shared<List>(id(), impl->name);

    for (const ValueData &item : *m_dataPtr)
        copy->append(item);

    return copy;
}
