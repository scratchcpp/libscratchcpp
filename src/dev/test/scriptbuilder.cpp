// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/test/scriptbuilder.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/dev/executablecode.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/list.h>

#include "scriptbuilder_p.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

static std::unordered_map<IEngine *, std::shared_ptr<List>> captureLists;

/*! Constructs ScriptBuilder. */
ScriptBuilder::ScriptBuilder(IExtension *extension, IEngine *engine, std::shared_ptr<Target> target) :
    impl(spimpl::make_unique_impl<ScriptBuilderPrivate>(engine, target))
{
    // Create capture list
    if (captureLists.find(engine) != captureLists.cend()) {
        std::cerr << "error: only one ScriptBuilder can be created for each engine" << std::endl;
        return;
    }

    captureLists[engine] = std::make_shared<List>("", "");

    // Add start hat block
    auto block = std::make_shared<Block>(std::to_string(impl->blockId++), "script_builder_init");
    engine->addCompileFunction(extension, block->opcode(), [](Compiler *compiler) -> CompilerValue * {
        compiler->engine()->addGreenFlagScript(compiler->block());
        return nullptr;
    });
    addBlock(block);

    // Add compile function for return value capture block
    engine->addCompileFunction(extension, "script_builder_capture", [](Compiler *compiler) -> CompilerValue * {
        CompilerValue *input = compiler->addInput("VALUE");
        compiler->createListAppend(captureLists[compiler->engine()].get(), input);
        return nullptr;
    });
}

/*! Destroys ScriptBuilder. */
ScriptBuilder::~ScriptBuilder()
{
    captureLists.erase(impl->engine);
}

/*! Adds a block with the given opcode to the script. */
void ScriptBuilder::addBlock(const std::string &opcode)
{
    impl->lastBlock = std::make_shared<Block>(std::to_string(impl->blockId++), opcode);
    addBlock(impl->lastBlock);
}

/*! Creates a reporter block with the given opcode to be used with captureBlockReturnValue() later. */
void ScriptBuilder::addReporterBlock(const std::string &opcode)
{
    impl->lastBlock = std::make_shared<Block>(std::to_string(impl->blockId++), opcode);
}

/*! Captures the return value of the created reporter block. It can be retrieved using capturedValues() later. */
void ScriptBuilder::captureBlockReturnValue()
{
    if (!impl->lastBlock)
        return;

    auto valueBlock = impl->lastBlock;
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

    valueBlock->setId(std::to_string(impl->blockId++));
    impl->inputBlocks.push_back(valueBlock);

    auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
    input->setValueBlock(valueBlock);
    impl->lastBlock->addInput(input);
}

/*! Adds an input obscured by a block which returns zero to the current block. */
void ScriptBuilder::addNullObscuredInput(const std::string &name)
{
    if (!impl->lastBlock)
        return;

    auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
    auto block = std::make_shared<Block>(std::to_string(impl->blockId++), "");
    block->setCompileFunction([](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(Value()); });
    input->setValueBlock(block);
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

    auto menu = std::make_shared<Block>(std::to_string(impl->blockId++), impl->lastBlock->opcode() + "_menu");
    menu->setShadow(true);
    impl->inputBlocks.push_back(menu);
    input->setValueBlock(menu);

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

    entity->setId(std::to_string(impl->blockId++));
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

    entity->setId(std::to_string(impl->blockId++));
    auto field = std::make_shared<Field>(name, Value(), entity);
    impl->lastBlock->addField(field);
}

/*!
 * Returns the current block (can be used e. g. with a custom Compiler instance).\n
 * The script is automatically built to set the compile function of the block.
 * \note This method is not intended for building scripts, use build() for that.
 */
std::shared_ptr<Block> ScriptBuilder::currentBlock()
{
    if (!impl->lastBlock)
        return nullptr;

    if (!impl->lastBlock->compileFunction())
        build(std::make_shared<Sprite>());

    return impl->lastBlock;
}

/*! Builds and compiles the script. */
void ScriptBuilder::build()
{
    build(impl->target);
}

/*! Runs the built script. */
void ScriptBuilder::run()
{
    impl->engine->run();
}

/*! Returns the list of captured block return values. */
List *ScriptBuilder::capturedValues() const
{
    return captureLists[impl->engine].get();
}

void ScriptBuilder::addBlock(std::shared_ptr<Block> block)
{
    if (!impl->blocks.empty()) {
        auto lastBlock = impl->blocks.back();
        lastBlock->setNext(block);
        block->setParent(lastBlock);
    }

    impl->blocks.push_back(block);
}

void ScriptBuilder::build(std::shared_ptr<Target> target)
{
    if (target->blocks().empty()) {
        for (auto block : impl->blocks)
            target->addBlock(block);

        for (auto block : impl->inputBlocks)
            target->addBlock(block);
    }

    std::vector<std::shared_ptr<Target>> targets = impl->engine->targets();

    if (std::find(targets.begin(), targets.end(), target) == targets.end()) {
        targets.push_back(target);
        impl->engine->setTargets({ target });
    }

    impl->engine->compile();
}
