// SPDX-License-Identifier: Apache-2.0

#include "block.h"
#include "../engine/engine.h"
#include "runningscript.h"
#include <iostream>

using namespace libscratchcpp;

/*! Constructs Block */
Block::Block(std::string id, std::string opcode) :
    m_opcode(opcode)
{
    setId(id);
}

/*!
 * Calls the block implementation.
 * \param[in] script The script from which the block was called. Call be nullptr for reporter blocks.
 * \param[out] defaultRetValue Value to return when the block isn't supported.
 */
Value Block::run(RunningScript *script, Value defaultRetValue)
{
    if (m_implementation)
        return m_implementation(BlockArgs(m_target, m_engine, script, this));
    else
        std::cout << "warning: unsupported block: " << m_opcode << std::endl;
    return defaultRetValue;
}

/*! Returns the opcode. */
std::string Block::opcode() const
{
    return m_opcode;
}

/*! Returns the implementation function. \see <a href="blockSections.html">Block sections</a> */
BlockImpl Block::implementation() const
{
    return m_implementation;
}

/*! Sets the implementation function. \see <a href="blockSections.html">Block sections</a> */
void Block::setImplementation(BlockImpl impl)
{
    m_implementation = impl;
}

/*! Returns the next block. */
std::shared_ptr<Block> Block::next() const
{
    return m_next;
}

/*! Returns the ID of the next block. */
std::string Block::nextId() const
{
    if (m_next)
        return m_next->id();
    else
        return m_nextId;
}

/*! Sets the next block. */
void Block::setNext(std::shared_ptr<Block> block)
{
    m_next = block;
}

/*! Sets the next block by ID. */
void Block::setNextId(const std::string &nextId)
{
    m_nextId = nextId;
    m_next = nullptr;
}

/*! Returns the parent block. */
std::shared_ptr<Block> Block::parent() const
{
    return m_parent;
}

/*! Returns the ID of the parent block. */
std::string Block::parentId() const
{
    if (m_parent)
        return m_parent->id();
    else
        return m_parentId;
}

/*! Sets the parent block. */
void Block::setParent(std::shared_ptr<Block> block)
{
    m_parent = block;
}

/*! Sets the parent block by ID. */
void Block::setParentId(const std::string &id)
{
    m_parentId = id;
    m_parent = nullptr;
}

/*! Returns the list of inputs. */
std::vector<std::shared_ptr<Input>> Block::inputs() const
{
    return m_inputs;
}

/*! Adds an input and returns its index. */
int Block::addInput(std::shared_ptr<Input> input)
{
    m_inputs.push_back(input);
    return m_inputs.size() - 1;
}

/*! Returns the input at index. */
std::shared_ptr<Input> Block::inputAt(int index) const
{
    return m_inputs[index];
}

/*! Returns the index of the input with the given name. */
int Block::findInput(const std::string &inputName) const
{
    int i = 0;
    for (auto input : m_inputs) {
        if (input->name() == inputName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the input with the given ID. */
Input *Block::findInputById(int id) const
{
    if (m_inputMap.count(id) == 1)
        return m_inputMap.at(id);
    return nullptr;
}

/*! Updates the map that assigns input IDs to input indexes. Used internally by Engine. */
void Block::updateInputMap()
{
    m_inputMap.clear();
    for (auto input : m_inputs)
        m_inputMap[input->inputId()] = input.get();
}

/*! Returns the list of fields. */
std::vector<std::shared_ptr<Field>> Block::fields() const
{
    return m_fields;
}

/*! Adds a field and returns its index. */
int Block::addField(std::shared_ptr<Field> field)
{
    m_fields.push_back(field);
    return m_fields.size() - 1;
}

/*! Returns the field at index. */
std::shared_ptr<Field> Block::fieldAt(int index) const
{
    return m_fields[index];
}

/*! Returns the index of the field with the given name. */
int Block::findField(const std::string &fieldName) const
{
    int i = 0;
    for (auto field : m_fields) {
        if (field->name() == fieldName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the index of the field with the given ID. */
Field *Block::findFieldById(int id) const
{
    if (m_fieldMap.count(id) == 1)
        return m_fieldMap.at(id);
    return nullptr;
}

/*! Updates the map that assigns input IDs to input indexes. Used internally by Engine. */
void Block::updateFieldMap()
{
    m_fieldMap.clear();
    for (auto field : m_fields)
        m_fieldMap[field->fieldId()] = field.get();
}

/*! Returns true if this is a shadow block. */
bool Block::shadow() const
{
    return m_shadow;
}

/*! Toggles whether this block is a shadow block. */
void Block::setShadow(bool newShadow)
{
    m_shadow = newShadow;
}

/*! Returns true if this is a top level block. */
bool Block::topLevel() const
{
    // TODO: Return true if parentId() == ""
    // and remove the setter
    return m_topLevel;
}

/*! Toggles whether this block is a top level block. */
void Block::setTopLevel(bool newTopLevel)
{
    m_topLevel = newTopLevel;
}

/*! Sets the Engine. */
void Block::setEngine(Engine *newEngine)
{
    m_engine = newEngine;
}

/*! Sets the Target. */
void Block::setTarget(Target *newTarget)
{
    m_target = newTarget;
}
