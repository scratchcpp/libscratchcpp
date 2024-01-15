// SPDX-License-Identifier: Apache-2.0

#include "blocksectioncontainer.h"

using namespace libscratchcpp;

static void dummyFunction(Compiler *compiler){};

void BlockSectionContainer::addCompileFunction(const std::string &opcode, BlockComp f)
{
    m_compileFunctions[opcode] = f;
}

void BlockSectionContainer::addMonitorNameFunction(const std::string &opcode, MonitorNameFunc f)
{
    m_monitorNameFunctions[opcode] = f;
}

void BlockSectionContainer::addHatBlock(const std::string &opcode)
{
    m_compileFunctions[opcode] = &dummyFunction;
}

void BlockSectionContainer::addInput(const std::string &name, int id)
{
    m_inputs[name] = id;
}

void BlockSectionContainer::addField(const std::string &name, int id)
{
    m_fields[name] = id;
}

void BlockSectionContainer::addFieldValue(const std::string &value, int id)
{
    m_fieldValues[value] = id;
}

BlockComp BlockSectionContainer::resolveBlockCompileFunc(const std::string &opcode) const
{
    if (m_compileFunctions.count(opcode) == 1)
        return m_compileFunctions.at(opcode);
    return nullptr;
}

MonitorNameFunc BlockSectionContainer::resolveMonitorNameFunc(const std::string &opcode) const
{
    if (m_monitorNameFunctions.count(opcode) == 1)
        return m_monitorNameFunctions.at(opcode);
    return nullptr;
}

int BlockSectionContainer::resolveInput(const std::string &name) const
{
    if (m_inputs.count(name) == 1)
        return m_inputs.at(name);
    return -1;
}

int BlockSectionContainer::resolveField(const std::string &name) const
{
    if (m_fields.count(name) == 1)
        return m_fields.at(name);
    return -1;
}

int BlockSectionContainer::resolveFieldValue(const std::string &value) const
{
    if (m_fieldValues.count(value) == 1)
        return m_fieldValues.at(value);
    return -1;
}
