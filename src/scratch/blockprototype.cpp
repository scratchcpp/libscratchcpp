// SPDX-License-Identifier: Apache-2.0

#include "blockprototype.h"

using namespace libscratchcpp;

/*! Constructs BlockPrototype. */
BlockPrototype::BlockPrototype()
{
}

/*! Constructs BlockPrototype. */
BlockPrototype::BlockPrototype(const std::string &procCode)
{
    setProcCode(procCode);
}

/*! Returns the name of the custom block, including inputs. */
const std::string &BlockPrototype::procCode() const
{
    return m_procCode;
}

/*! Sets the name of the custom block, including inputs. */
void BlockPrototype::setProcCode(const std::string &newProcCode)
{
    m_procCode = newProcCode;
    m_argumentDefaults.clear();
    m_argumentTypes.clear();
    bool arg = false;

    for (auto c : m_procCode) {
        if (c == '%')
            arg = true;
        else if (arg) {
            arg = false;
            switch (c) {
                case 's':
                    m_argumentDefaults.push_back("");
                    m_argumentTypes.push_back(ArgType::StringNum);
                    break;

                case 'b':
                    m_argumentDefaults.push_back(false);
                    m_argumentTypes.push_back(ArgType::Bool);
                    break;

                default:
                    break;
            }
        }
    }
}

/*! Returns the list of argument IDs. */
const std::vector<std::string> &BlockPrototype::argumentIds() const
{
    return m_argumentIds;
}

/*! Sets the list of argument IDs. */
void BlockPrototype::setArgumentIds(const std::vector<std::string> &newArgumentIds)
{
    m_argumentIds = newArgumentIds;
}

/*! Returns the list of argument names. */
const std::vector<std::string> &BlockPrototype::argumentNames() const
{
    return m_argumentNames;
}

/*! Sets the list of argument names. */
void BlockPrototype::setArgumentNames(const std::vector<std::string> &newArgumentNames)
{
    m_argumentNames = newArgumentNames;
}

/*! Returns the list of argument default values. */
const std::vector<Value> &BlockPrototype::argumentDefaults() const
{
    return m_argumentDefaults;
}

/*! Returns the list of argument types. */
const std::vector<BlockPrototype::ArgType> &BlockPrototype::argumentTypes() const
{
    return m_argumentTypes;
}

/*! Returns true if the block is set to run without screen refresh. */
bool BlockPrototype::warp() const
{
    return m_warp;
}

/*! Sets whether to run the block without screen refresh. */
void BlockPrototype::setWarp(bool newWarp)
{
    m_warp = newWarp;
}
