// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <iostream>

#include "block_p.h"

using namespace libscratchcpp;

/*! Constructs Block */
Block::Block(const std::string &id, const std::string &opcode) :
    Entity(id),
    impl(spimpl::make_unique_impl<BlockPrivate>(opcode))
{
}

/*! Calls the compile function. */
CompilerValue *Block::compile(Compiler *compiler)
{
    if (impl->compileFunction)
        return impl->compileFunction(compiler);
    else
        return nullptr;
}

/*! Returns the opcode. */
const std::string &Block::opcode() const
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

/*! Returns the edge-activated hat predicate compile function. \see <a href="blockSections.html">Block sections</a> */
HatPredicateCompileFunc Block::hatPredicateCompileFunction() const
{
    return impl->hatPredicateCompileFunction;
}

/*! Sets the edge-activated hat predicate compile function. \see <a href="blockSections.html">Block sections</a> */
void Block::setHatPredicateCompileFunction(HatPredicateCompileFunc newHatPredicateCompileFunction)
{
    if (newHatPredicateCompileFunction && !topLevel()) {
        std::cerr << "error: predicate compile functions can be only used on top-level blocks" << std::endl;
        assert(false);
        return;
    }

    impl->hatPredicateCompileFunction = newHatPredicateCompileFunction;
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

/*! Returns true if this is a top level reporter block for a variable or a list. */
bool Block::isTopLevelReporter() const
{
    return impl->isTopLevelReporter;
}

/*!
 * Sets whether this block is a top level reporter block for a variable or a list.
 * \note Setting this to true will allow the use of topLevelReporterInfo().
 */
void Block::setIsTopLevelReporter(bool isTopLevelReporter)
{
    impl->isTopLevelReporter = isTopLevelReporter;

    if (isTopLevelReporter && !impl->topLevelReporterInfo)
        impl->topLevelReporterInfo = std::make_unique<InputValue>(InputValue::Type::Variable);
    else if (impl->topLevelReporterInfo) {
        impl->topLevelReporterInfo.reset();
        impl->topLevelReporterInfo = nullptr;
    }
}

/*!
 * Returns the information about this top level reporter block (if this is a top level reporter block).
 * \note This function will return nullptr if this isn't a top level reporter block. Use setIsTopLevelReporter(true)
 * before using this function.
 */
InputValue *Block::topLevelReporterInfo()
{
    if (impl->topLevelReporterInfo)
        return impl->topLevelReporterInfo.get();
    else
        return nullptr;
}

/*! Returns the next block. */
std::shared_ptr<Block> Block::next() const
{
    return impl->next;
}

/*! Returns the ID of the next block. */
const std::string &Block::nextId() const
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
const std::string &Block::parentId() const
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
const std::vector<std::shared_ptr<Input>> &Block::inputs() const
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
    impl->inputMap[input->inputId()] = input.get();

    return impl->inputs.size() - 1;
}

/*! Returns the input at index. */
std::shared_ptr<Input> Block::inputAt(int index) const
{
    if (index < 0 || index >= impl->inputs.size())
        return nullptr;

    return impl->inputs[index];
}

/*! Returns the index of the input with the given name. */
int Block::findInput(const std::string &inputName) const
{
    auto it = std::find_if(impl->inputs.begin(), impl->inputs.end(), [inputName](std::shared_ptr<Input> input) { return input->name() == inputName; });

    if (it == impl->inputs.end())
        return -1;
    else
        return it - impl->inputs.begin();
}

/*! Returns the input with the given ID. */
Input *Block::findInputById(int id) const
{
    if (impl->inputMap.count(id) == 1)
        return impl->inputMap.at(id);
    else {
        auto it = std::find_if(impl->inputs.begin(), impl->inputs.end(), [id](std::shared_ptr<Input> input) { return input->inputId() == id; });

        if (it != impl->inputs.end())
            return it->get();
    }
    return nullptr;
}

void Block::updateInputMap()
{
    impl->inputMap.clear();
    for (auto input : impl->inputs)
        impl->inputMap[input->inputId()] = input.get();
}

/*! Returns the list of fields. */
const std::vector<std::shared_ptr<Field>> &Block::fields() const
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
    impl->fieldMap[field->fieldId()] = field.get();

    return impl->fields.size() - 1;
}

/*! Returns the field at index. */
std::shared_ptr<Field> Block::fieldAt(int index) const
{
    if (index < 0 || index >= impl->fields.size())
        return nullptr;

    return impl->fields[index];
}

/*! Returns the index of the field with the given name. */
int Block::findField(const std::string &fieldName) const
{
    auto it = std::find_if(impl->fields.begin(), impl->fields.end(), [fieldName](std::shared_ptr<Field> field) { return field->name() == fieldName; });

    if (it == impl->fields.end())
        return -1;
    else
        return it - impl->fields.begin();
}

/*! Returns the index of the field with the given ID. */
Field *Block::findFieldById(int id) const
{
    if (impl->fieldMap.count(id) == 1)
        return impl->fieldMap.at(id);
    else {
        auto it = std::find_if(impl->fields.begin(), impl->fields.end(), [id](std::shared_ptr<Field> field) { return field->fieldId() == id; });

        if (it != impl->fields.end())
            return it->get();
    }
    return nullptr;
}

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

/*! Returns the X-coordinate of the block in the code area (only for top level blocks). */
int Block::x() const
{
    return impl->x;
}

/*! Sets the X-coordinate of the block in the code area (only for top level blocks). */
void Block::setX(int x)
{
    if (!topLevel())
        std::cout << "warning: setting X-coordinate of a block which is not a top level block" << std::endl;

    impl->x = x;
}

/*! Returns the Y-coordinate of the block in the code area (only for top level blocks). */
int Block::y() const
{
    return impl->y;
}

/*! Sets the Y-coordinate of the block in the code area (only for top level blocks). */
void Block::setY(int y)
{
    if (!topLevel())
        std::cout << "warning: setting Y-coordinate of a block which is not a top level block" << std::endl;

    impl->y = y;
}

/*! Returns the comment which is attached to this block. */
std::shared_ptr<Comment> Block::comment() const
{
    return impl->comment;
}

/*! Returns the ID of the comment which is attached to this block. */
const std::string &Block::commentId() const
{
    return impl->commentId;
}

/*! Sets the comment which is attached to this block. */
void Block::setComment(std::shared_ptr<Comment> comment)
{
    impl->comment = comment;

    if (comment)
        impl->commentId = comment->id();
    else
        impl->commentId = "";
}

/*! Sets the ID of the comment which is attached to this block. */
void Block::setCommentId(const std::string &commentId)
{
    impl->commentId = commentId;
    impl->comment = nullptr;
}

/*! Sets the Engine. */
void Block::setEngine(IEngine *newEngine)
{
    impl->engine = newEngine;
}

/*! Returns the Engine. */
IEngine *Block::engine() const
{
    return impl->engine;
}

/*! Sets the Target. */
void Block::setTarget(Target *newTarget)
{
    impl->target = newTarget;
}

/*! Returns the Target. */
Target *Block::target() const
{
    return impl->target;
}
