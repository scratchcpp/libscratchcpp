// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/keyevent.h>

#include "keyevent_p.h"

namespace libscratchcpp
{

/*! Constructs KeyEvent. */
KeyEvent::KeyEvent(Type type) :
    impl(spimpl::make_impl<KeyEventPrivate>(type))
{
}

/*! Constructs KeyEvent from a key name. */
KeyEvent::KeyEvent(const std::string &name) :
    impl(spimpl::make_impl<KeyEventPrivate>(name))
{
}

/*! Returns the type of the key. */
KeyEvent::Type KeyEvent::type() const
{
    return impl->type;
}

/*! Returns the name of the key. */
const std::string &KeyEvent::name() const
{
    return impl->name;
}

} // namespace libscratchcpp
