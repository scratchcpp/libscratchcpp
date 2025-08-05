// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/thread.h>

#include "scriptbuilder_p.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

// TODO: Add support for return values captures when building multiple scripts
static std::unordered_map<const ScriptBuilder *, std::shared_ptr<List>> captureLists;
static ScriptBuilder *currentScriptBuilder = nullptr;

/*! Constructs ScriptBuilder. */
ScriptBuilder::ScriptBuilder(IExtension *extension, IEngine *engine, std::shared_ptr<Target> target, bool createHatBlock) :
    impl(spimpl::make_unique_impl<ScriptBuilderPrivate>(engine, target))
{
    // Create capture list
    captureLists[this] = std::make_shared<List>("", "");

    // Add start hat block
    if (createHatBlock) {
        auto block = std::make_shared<Block>(nextId(), "script_builder_init");
        engine->addCompileFunction(extension, block->opcode(), [](Compiler *compiler) -> CompilerValue * {
            compiler->engine()->addGreenFlagScript(compiler->block());
            return nullptr;
        });
        addBlockToList(block);
        impl->hatBlock = block.get();
    }

    // Add compile function for return value capture block
    engine->addCompileFunction(extension, "script_builder_capture", [](Compiler *compiler) -> CompilerValue * {
        CompilerValue *input = compiler->addInput("VALUE");
        compiler->createListAppend(captureLists[currentScriptBuilder].get(), input);
        return nullptr;
    });
}

/*! Destroys ScriptBuilder. */
ScriptBuilder::~ScriptBuilder()
{
    captureLists.erase(this);
}

/*! Adds the given block to the script. */
void ScriptBuilder::addBlock(std::shared_ptr<Block> block)
{
    block->setId(nextId());
    impl->lastBlock = block.get();
    addBlockToList(block);
}

/*! Adds a block with the given opcode to the script. */
void ScriptBuilder::addBlock(const std::string &opcode)
{
    addBlock(std::make_shared<Block>("", opcode));
}

/*! Captures the return value of the created reporter block. It can be retrieved using capturedValues() later. */
void ScriptBuilder::captureBlockReturnValue()
{
    if (!impl->lastBlock)
        return;

    auto valueBlock = takeBlock();
    addBlock("script_builder_capture");
    addObscuredInput("VALUE", valueBlock);
}

/*! Adds a simple input with a value to the current block. */
void ScriptBuilder::addValueInput(const std::string &name, const Value &value)
{
    if (!impl->lastBlock)
        return;

    auto input = std::make_shared<Input>(name, Input::Type::Shadow);
    input->setPrimaryValue(value);
    impl->lastBlock->addInput(input);
}

/*! Adds a null input (zero) to the current block. */
void ScriptBuilder::addNullInput(const std::string &name)
{
    if (!impl->lastBlock)
        return;

    auto input = std::make_shared<Input>(name, Input::Type::Shadow);
    impl->lastBlock->addInput(input);
}

/*! Adds an input obscured by the given block to the current block. */
void ScriptBuilder::addObscuredInput(const std::string &name, std::shared_ptr<Block> valueBlock)
{
    if (!impl->lastBlock)
        return;

    auto block = valueBlock;
    block->setParent(impl->lastBlock);

    while (block) {
        block->setId(nextId());
        impl->inputBlocks.push_back(block);

        auto parent = block->parent();
        auto next = block->next();

        if (parent && block != valueBlock)
            parent->setNext(block.get());

        if (next)
            next->setParent(block.get());

        block = next ? next->shared_from_this() : nullptr;
    }

    auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
    input->setValueBlock(valueBlock.get());
    impl->lastBlock->addInput(input);
}

/*! Adds an input obscured by a block which returns zero to the current block. */
void ScriptBuilder::addNullObscuredInput(const std::string &name)
{
    if (!impl->lastBlock)
        return;

    auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
    auto block = std::make_shared<Block>(nextId(), "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(Value()); });
    input->setValueBlock(block.get());
    impl->inputBlocks.push_back(block);
    impl->lastBlock->addInput(input);
}

/*! Adds a dropdown menu input to the current block. */
void ScriptBuilder::addDropdownInput(const std::string &name, const std::string &selectedValue)
{
    if (!impl->lastBlock)
        return;

    auto input = std::make_shared<Input>(name, Input::Type::Shadow);
    impl->lastBlock->addInput(input);

    auto menu = std::make_shared<Block>(nextId(), impl->lastBlock->opcode() + "_menu");
    menu->setShadow(true);
    impl->inputBlocks.push_back(menu);
    input->setValueBlock(menu.get());

    auto field = std::make_shared<Field>(name, selectedValue);
    menu->addField(field);
}

/*! Adds a dropdown field to the current block. */
void ScriptBuilder::addDropdownField(const std::string &name, const std::string &selectedValue)
{
    if (!impl->lastBlock)
        return;

    auto field = std::make_shared<Field>(name, selectedValue);
    impl->lastBlock->addField(field);
}

/*! Adds an input pointing to an entity (variable, list, broadcast, etc.) */
void ScriptBuilder::addEntityInput(const std::string &name, const std::string &entityName, InputValue::Type entityType, std::shared_ptr<Entity> entity)
{
    if (!impl->lastBlock)
        return;

    if (std::find(impl->entities.begin(), impl->entities.end(), entity) == impl->entities.end()) {
        entity->setId(nextId());
        impl->entities.push_back(entity);
    }

    auto input = std::make_shared<Input>(name, Input::Type::Shadow);
    input->setPrimaryValue(entityName);
    input->primaryValue()->setValuePtr(entity);
    input->primaryValue()->setType(entityType);
    impl->lastBlock->addInput(input);
}

/*! Adds a field pointing to an entity (variable, list, broadcast, etc.) */
void ScriptBuilder::addEntityField(const std::string &name, std::shared_ptr<Entity> entity)
{
    if (!impl->lastBlock)
        return;

    if (std::find(impl->entities.begin(), impl->entities.end(), entity) == impl->entities.end()) {
        entity->setId(nextId());
        impl->entities.push_back(entity);
    }

    Value entityName = "";
    auto var = std::dynamic_pointer_cast<Variable>(entity);
    auto list = std::dynamic_pointer_cast<List>(entity);
    auto broadcast = std::dynamic_pointer_cast<Broadcast>(entity);

    if (var)
        entityName = var->name();
    else if (list)
        entityName = list->name();
    else if (broadcast)
        entityName = broadcast->name();

    auto field = std::make_shared<Field>(name, entityName, entity);
    impl->lastBlock->addField(field);
}

/*!
 * Returns the current block (can be used e. g. with a custom Compiler instance).\n
 * The script is automatically built to set the compile function of the block.
 * \note This method is not intended for building scripts, use build() for that.
 */
Block *ScriptBuilder::currentBlock()
{
    if (!impl->lastBlock)
        return nullptr;

    if (!impl->lastBlock->compileFunction()) {
        auto target = std::make_shared<Sprite>();
        const auto &variables = impl->target->variables();
        const auto &lists = impl->target->lists();

        for (auto var : variables)
            target->addVariable(var);

        for (auto list : lists)
            target->addList(list);

        build(target);

        std::vector<std::shared_ptr<Target>> targets = impl->engine->targets();
        targets.erase(std::remove(targets.begin(), targets.end(), target), targets.end());
        impl->engine->setTargets(targets);
    }

    return impl->lastBlock;
}

/*! Removes the current block from the script and returns it. Can be used in inputs later. */
std::shared_ptr<Block> ScriptBuilder::takeBlock()
{
    if (!impl->lastBlock)
        return nullptr;

    auto block = impl->blocks.back();
    impl->blocks.pop_back();
    impl->lastBlock = nullptr;

    if (!impl->blocks.empty())
        impl->blocks.back()->setNext(nullptr);

    block->setParent(nullptr);
    block->setNext(nullptr);

    return block;
}

/*! Builds and compiles the script. */
void ScriptBuilder::build()
{
    build(impl->target);
}

/*! Runs the built script. */
void ScriptBuilder::run()
{
    impl->engine->startScript(impl->hatBlock, impl->target.get());

    do {
        impl->engine->step();
    } while (impl->engine->isRunning());
}

/*! Returns the list of captured block return values. */
List *ScriptBuilder::capturedValues() const
{
    return captureLists[this].get();
}

/*!
 * Builds multiple scripts using the given script builders.
 * \note Using run() on any of the script builders will result in all scripts without a custom hat block being called. Use this only with a single when flag clicked block.
 * \note Return value capturing is not supported when building multiple scripts.
 */
void ScriptBuilder::buildMultiple(const std::vector<ScriptBuilder *> &builders)
{
    std::unordered_set<IEngine *> engines;

    for (ScriptBuilder *builder : builders) {
        auto target = builder->impl->target;
        addBlocksToTarget(target.get(), builder->impl->blocks);
        addBlocksToTarget(target.get(), builder->impl->inputBlocks);
        addTargetToEngine(builder->impl->engine, target);
        engines.insert(builder->impl->engine);
    }

    for (IEngine *engine : engines)
        engine->compile();
}

void ScriptBuilder::addBlockToList(std::shared_ptr<Block> block)
{
    if (!impl->blocks.empty()) {
        auto lastBlock = impl->blocks.back();
        lastBlock->setNext(block.get());
        block->setParent(lastBlock.get());
    }

    impl->blocks.push_back(block);
}

void ScriptBuilder::build(std::shared_ptr<Target> target)
{
    currentScriptBuilder = this;

    addBlocksToTarget(target.get(), impl->blocks);
    addBlocksToTarget(target.get(), impl->inputBlocks);
    addTargetToEngine(impl->engine, target);

    impl->engine->compile();
    currentScriptBuilder = nullptr;
}

std::string ScriptBuilder::nextId()
{
    return std::to_string((uintptr_t)this) + '.' + std::to_string(impl->blockId++);
}

void ScriptBuilder::addBlocksToTarget(Target *target, const std::vector<std::shared_ptr<Block>> &blocks)
{
    auto targetBlocks = target->blocks();

    for (auto block : blocks) {
        if (std::find(targetBlocks.begin(), targetBlocks.end(), block) == targetBlocks.end())
            target->addBlock(block);
    }
}

void ScriptBuilder::addTargetToEngine(IEngine *engine, std::shared_ptr<Target> target)
{
    std::vector<std::shared_ptr<Target>> targets = engine->targets();

    if (std::find(targets.begin(), targets.end(), target) == targets.end()) {
        targets.push_back(target);
        engine->setTargets(targets);
    }
}
