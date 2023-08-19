// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>

#include "block_p.h"

using namespace libscratchcpp;

/*! Constructs Block */
Block::Block(const std::string &id, const std::string &opcode) :
    Entity(id),
    impl(spimpl::make_unique_impl<BlockPrivate>(opcode))
{
}

/*! Calls the compile function. */
void Block::compile(Compiler *compiler)
{
    return impl->compileFunction(compiler);
}

/*! Returns the opcode. */
std::string Block::opcode() const
{
    return impl->opcode;
}

/*! Returns the compile function. \see <a href="blockSections.html">Block sections</a> */
BlockComp Block::compileFunction() const
{
    return impl->compileFunction;
}

/*! Sets the compile function. \see <a href="blockSections.html">Block sections</a> */
void Block::setCompileFunction(BlockComp newCompileFunction)
{
    impl->compileFunction = newCompileFunction;
}

/*! Returns true if the block can have a block following it. */
bool Block::mutationHasNext() const
{
    return impl->mutationHasNext;
}

/*! Sets whether the block can have a block following it. */
void Block::setMutationHasNext(bool newMutationHasNext)
{
    impl->mutationHasNext = newMutationHasNext;
}

/*! Returns the block prototype (in custom block definition). */
BlockPrototype *Block::mutationPrototype()
{
    return &impl->mutationPrototype;
}

/*! Returns the next block. */
std::shared_ptr<Block> Block::next() const
{
    return impl->next;
}

/*! Returns the ID of the next block. */
std::string Block::nextId() const
{
    return impl->nextId;
}

/*! Sets the next block. */
void Block::setNext(std::shared_ptr<Block> block)
{
    impl->next = block;

    if (block)
        impl->nextId = block->id();
    else
        impl->nextId = "";
}

/*! Sets the next block by ID. */
void Block::setNextId(const std::string &nextId)
{
    impl->nextId = nextId;
    impl->next = nullptr;
}

/*! Returns the parent block. */
std::shared_ptr<Block> Block::parent() const
{
    return impl->parent;
}

/*! Returns the ID of the parent block. */
std::string Block::parentId() const
{
    return impl->parentId;
}

/*! Sets the parent block. */
void Block::setParent(std::shared_ptr<Block> block)
{
    impl->parent = block;

    if (block)
        impl->parentId = block->id();
    else
        impl->parentId = "";
}

/*! Sets the parent block by ID. */
void Block::setParentId(const std::string &id)
{
    impl->parentId = id;
    impl->parent = nullptr;
}

/*! Returns the list of inputs. */
std::vector<std::shared_ptr<Input>> Block::inputs() const
{
    return impl->inputs;
}

/*! Adds an input and returns its index. */
int Block::addInput(std::shared_ptr<Input> input)
{
    auto it = std::find(impl->inputs.begin(), impl->inputs.end(), input);

    if (it != impl->inputs.end())
        return it - impl->inputs.begin();

    impl->inputs.push_back(input);
    return impl->inputs.size() - 1;
}

/*! Returns the input at index. */
std::shared_ptr<Input> Block::inputAt(int index) const
{
    return impl->inputs[index];
}

/*! Returns the index of the input with the given name. */
int Block::findInput(const std::string &inputName) const
{
    int i = 0;
    for (auto input : impl->inputs) {
        if (input->name() == inputName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the input with the given ID. */
Input *Block::findInputById(int id) const
{
    if (impl->inputMap.count(id) == 1)
        return impl->inputMap.at(id);
    return nullptr;
}

/*! Updates the map that assigns input IDs to input indexes. Used internally by Engine. */
void Block::updateInputMap()
{
    impl->inputMap.clear();
    for (auto input : impl->inputs)
        impl->inputMap[input->inputId()] = input.get();
}

/*! Returns the list of fields. */
std::vector<std::shared_ptr<Field>> Block::fields() const
{
    return impl->fields;
}

/*! Adds a field and returns its index. */
int Block::addField(std::shared_ptr<Field> field)
{
    auto it = std::find(impl->fields.begin(), impl->fields.end(), field);

    if (it != impl->fields.end())
        return it - impl->fields.begin();

    impl->fields.push_back(field);
    return impl->fields.size() - 1;
}

/*! Returns the field at index. */
std::shared_ptr<Field> Block::fieldAt(int index) const
{
    return impl->fields[index];
}

/*! Returns the index of the field with the given name. */
int Block::findField(const std::string &fieldName) const
{
    int i = 0;
    for (auto field : impl->fields) {
        if (field->name() == fieldName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the index of the field with the given ID. */
Field *Block::findFieldById(int id) const
{
    if (impl->fieldMap.count(id) == 1)
        return impl->fieldMap.at(id);
    return nullptr;
}

/*! Updates the map that assigns input IDs to input indexes. Used internally by Engine. */
void Block::updateFieldMap()
{
    impl->fieldMap.clear();
    for (auto field : impl->fields)
        impl->fieldMap[field->fieldId()] = field.get();
}

/*! Returns true if this is a shadow block. */
bool Block::shadow() const
{
    return impl->shadow;
}

/*! Toggles whether this block is a shadow block. */
void Block::setShadow(bool newShadow)
{
    impl->shadow = newShadow;
}

/*! Returns true if this is a top level block. */
bool Block::topLevel() const
{
    return (impl->parentId == "" && !impl->parent);
}

/*! Sets the Engine. */
void Block::setEngine(IEngine *newEngine)
{
    impl->engine = newEngine;
}

/*! Sets the Target. */
void Block::setTarget(Target *newTarget)
{
    impl->target = newTarget;
}
