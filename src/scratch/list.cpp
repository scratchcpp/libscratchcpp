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

/*! Creates a copy of the list. */
std::shared_ptr<List> List::clone()
{
    auto copy = std::make_shared<List>(id(), impl->name);

    for (const ValueData &item : *m_dataPtr)
        copy->append(item);

    return copy;
}
