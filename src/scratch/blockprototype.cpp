// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/blockprototype.h>

#include "blockprototype_p.h"

using namespace libscratchcpp;

/*! Constructs BlockPrototype. */
BlockPrototype::BlockPrototype() :
    impl(spimpl::make_impl<BlockPrototypePrivate>())
{
}

/*! Constructs BlockPrototype with the given proc code. */
BlockPrototype::BlockPrototype(const std::string &procCode) :
    impl(spimpl::make_impl<BlockPrototypePrivate>(procCode))
{
}

/*! Returns the name of the custom block, including inputs. */
const std::string &BlockPrototype::procCode() const
{
    return impl->procCode;
}

/*! Sets the name of the custom block, including inputs. */
void BlockPrototype::setProcCode(const std::string &newProcCode)
{
    impl->setProcCode(newProcCode);
}

/*! Returns the list of argument IDs. */
const std::vector<std::string> &BlockPrototype::argumentIds() const
{
    return impl->argumentIds;
}

/*! Sets the list of argument IDs. */
void BlockPrototype::setArgumentIds(const std::vector<std::string> &newArgumentIds)
{
    impl->argumentIds = newArgumentIds;
}

/*! Returns the list of argument names. */
const std::vector<std::string> &BlockPrototype::argumentNames() const
{
    return impl->argumentNames;
}

/*! Sets the list of argument names. */
void BlockPrototype::setArgumentNames(const std::vector<std::string> &newArgumentNames)
{
    impl->argumentNames = newArgumentNames;
}

/*! Returns the list of argument default values. */
const std::vector<Value> &BlockPrototype::argumentDefaults() const
{
    return impl->argumentDefaults;
}

/*! Returns the list of argument types. */
const std::vector<BlockPrototype::ArgType> &BlockPrototype::argumentTypes() const
{
    return impl->argumentTypes;
}

/*! Returns true if the block is set to run without screen refresh. */
bool BlockPrototype::warp() const
{
    return impl->warp;
}

/*! Sets whether to run the block without screen refresh. */
void BlockPrototype::setWarp(bool newWarp)
{
    impl->warp = newWarp;
}
