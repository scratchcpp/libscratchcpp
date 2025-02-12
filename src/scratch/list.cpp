// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/list.h>
#include <utf8.h>

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
    reserve(m_size);
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

/*! Same as the other method, but returns the result as std::string. */
std::string List::toString() const
{
    StringPtr *str = toStringPtr();
    std::string ret = utf8::utf16to8(std::u16string(str->data));
    string_pool_free(str);
    return ret;
}

/*! Creates a copy of the list. */
std::shared_ptr<List> List::clone()
{
    auto copy = std::make_shared<List>(id(), impl->name);
    copy->reserve(m_size);

    for (size_t i = 0; i < m_size; i++)
        copy->append(m_dataPtr->operator[](i));

    return copy;
}
