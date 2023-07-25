// SPDX-License-Identifier: Apache-2.0

#include "engine.h"
#include "../blocks/standardblocks.h"
#include "../scratchconfiguration.h"
#include "iblocksection.h"
#include "compiler.h"
#include "script.h"
#include <cassert>
#include <iostream>
#include <thread>

using namespace libscratchcpp;

/*! Constructs engine. */
Engine::Engine()
{
    srand(time(NULL));
}

/*! Clears the engine so that it can be used with another project. */
void Engine::clear()
{
    m_sections.clear();
    m_targets.clear();
    m_broadcasts.clear();
}

/*!
 * Resolves ID references and sets pointers of entities.
 * \note This function shouldn't normally be called because it's called from compile().
 */
void Engine::resolveIds()
{
    for (auto target : m_targets) {
        std::cout << "Processing target " << target->name() << "..." << std::endl;
        auto blocks = target->blocks();
        for (auto block : blocks) {
            auto section = blockSection(block->opcode());
            block->setNext(getBlock(block->nextId()));
            block->setParent(getBlock(block->parentId()));
            if (section)
                block->setCompileFunction(section->resolveBlockCompileFunc(block->opcode()));

            auto inputs = block->inputs();
            for (auto input : inputs) {
                input->setValueBlock(getBlock(input->valueBlockId()));
                if (section)
                    input->setInputId(section->resolveInput(input->name()));
                input->primaryValue()->setValuePtr(getEntity(input->primaryValue()->valueId()));
                input->secondaryValue()->setValuePtr(getEntity(input->primaryValue()->valueId()));
            }

            auto fields = block->fields();
            for (auto field : fields) {
                field->setValuePtr(getEntity(field->valueId()));
                if (section) {
                    field->setFieldId(section->resolveField(field->name()));
                    if (!field->valuePtr())
                        field->setSpecialValueId(section->resolveFieldValue(field->value().toString()));
                }
            }

            block->updateInputMap();
            block->updateFieldMap();
        }
    }
}

/*!
 * Compiles all scripts to bytecode.
 * \see Compiler
 */
void Engine::compile()
{
    // Resolve entities by ID
    resolveIds();

    // Compile scripts to bytecode
    for (auto target : m_targets) {
        std::cout << "Compiling scripts in target " << target->name() << "..." << std::endl;
        std::unordered_map<std::string, unsigned int *> procedureBytecodeMap;
        Compiler compiler(this);
        auto blocks = target->blocks();
        for (auto block : blocks) {
            if (block->topLevel()) {
                auto section = blockSection(block->opcode());
                if (section) {
                    auto script = std::make_shared<Script>(target.get(), this);
                    m_scripts[block] = script;

                    compiler.compile(block);

                    script->setFunctions(m_functions);
                    script->setBytecode(compiler.bytecode());
                    if (block->opcode() == "procedures_definition") {
                        auto b = block->inputAt(block->findInput("custom_block"))->valueBlock();
                        procedureBytecodeMap[b->mutationPrototype()->procCode()] = script->bytecode();
                    }
                } else
                    std::cout << "warning: unsupported top level block: " << block->opcode() << std::endl;
            }
        }

        const std::vector<std::string> &procedures = compiler.procedures();
        std::vector<unsigned int *> procedureBytecodes;
        for (const std::string &code : procedures)
            procedureBytecodes.push_back(procedureBytecodeMap[code]);

        for (auto block : blocks) {
            if (m_scripts.count(block) == 1) {
                m_scripts[block]->setProcedures(procedureBytecodes);
                m_scripts[block]->setConstValues(compiler.constValues());
                m_scripts[block]->setVariables(compiler.variablePtrs());
                m_scripts[block]->setLists(compiler.lists());
            }
        }
    }
}

/*!
 * Runs a single frame.\n
 * Use this if you want to use a custom event loop
 * in your project player.
 * \note Nothing will happen until start() is called.
 * \param[in] timeLimit The time limit for the frame (for atomic scripts). Set to 0 for no limit.
 */
void Engine::frame()
{
    for (int i = 0; i < m_runningScripts.size(); i++) {
        auto script = m_runningScripts[i];
        m_breakFrame = false;

        do {
            script->run();
            if (script->atEnd()) {
                for (auto &[key, value] : m_broadcastMap)
                    value.erase(std::remove(value.begin(), value.end(), script->script()), value.end());
                m_scriptsToRemove.push_back(script.get());
            }
        } while (!script->atEnd() && !m_breakFrame);
    }

    for (auto script : m_scriptsToRemove) {
        size_t index = -1;
        for (size_t i = 0; i < m_runningScripts.size(); i++) {
            if (m_runningScripts[i].get() == script) {
                index = i;
                break;
            }
        }
        assert(index != -1);
        m_runningScripts.erase(m_runningScripts.begin() + index);
    }
    m_scriptsToRemove.clear();
}

/*!
 * Calls all "when green flag clicked" blocks.
 * \note Nothing will happen until run() or frame() is called.
 */
void Engine::start()
{
    for (auto target : m_targets) {
        auto gfBlocks = target->greenFlagBlocks();
        for (auto block : gfBlocks)
            startScript(block, target);
    }
}

/*! Stops all scripts. */
void Engine::stop()
{
    m_runningScripts.clear();
}

/*! Starts a script with the given top level block as the given Target (a sprite or the stage). */
void Engine::startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target)
{
    if (!topLevelBlock) {
        std::cout << "warning: starting a script with a null top level block (nothing will happen)" << std::endl;
        return;
    }

    if (!target) {
        std::cout << "error: scripts must be started by a target";
        assert(false);
        return;
    }

    if (topLevelBlock->next()) {
        auto script = m_scripts[topLevelBlock];
        m_runningScripts.push_back(script->start());
    }
}

/*! Starts the script of the broadcast with the given index. */
void libscratchcpp::Engine::broadcast(unsigned int index, VirtualMachine *sourceScript)
{
    const std::vector<Script *> &scripts = m_broadcastMap[index];
    for (auto script : scripts) {
        size_t index = -1;
        for (size_t i = 0; i < m_runningScripts.size(); i++) {
            if (m_runningScripts[i]->script() == script) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            // Reset the script if it's already running
            m_runningScripts[index]->reset();
            if (script == sourceScript->script())
                sourceScript->stop(false, true);
        } else {
            m_runningScripts.push_back(script->start());
        }
    }
}

/*! Stops the given script. */
void Engine::stopScript(VirtualMachine *vm)
{
    assert(vm);
    m_scriptsToRemove.push_back(vm);
}

/*!
 * Stops all scripts in the given target.
 * \param[in] target The Target to stop.
 * \param[in] exceptScript Sets this parameter to stop all scripts except the given script.
 */
void Engine::stopTarget(Target *target, VirtualMachine *exceptScript)
{
    std::vector<VirtualMachine *> scripts;
    for (auto script : m_runningScripts) {
        if ((script->target() == target) && (script.get() != exceptScript))
            scripts.push_back(script.get());
    }

    for (auto script : scripts)
        stopScript(script);
}

/*!
 * Runs the event loop and calls "when green flag clicked" blocks.
 * \note This function returns when all scripts finish.\n
 * If you need to implement something advanced, such as a GUI with the
 * green flag button, use frame().
 */
void Engine::run()
{
    auto frameDuration = std::chrono::milliseconds(33);
    start();

    while (true) {
        auto lastFrameTime = std::chrono::steady_clock::now();
        // Execute the frame
        frame();
        if (m_runningScripts.size() <= 0)
            break;

        // Sleep until the time for the next frame
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
        auto sleepTime = frameDuration - elapsedTime;

        if (sleepTime > std::chrono::milliseconds::zero())
            std::this_thread::sleep_for(sleepTime);

        lastFrameTime = currentTime;
    }
}

/*! Returns true if there are any running script of the broadcast with the given index. */
bool Engine::broadcastRunning(unsigned int index)
{
    return !m_broadcastMap[index].empty();
}

/*!
 * Call this from a block implementation to force a "screen refresh".
 * \note This has no effect in "run without screen refresh" custom blocks.
 */
void Engine::breakFrame()
{
    m_breakFrame = true;
}

/*! Returns true if breakFrame() was called. */
bool libscratchcpp::Engine::breakingCurrentFrame()
{
    return m_breakFrame;
}

/*! Registers the given block section. */
void Engine::registerSection(std::shared_ptr<IBlockSection> section)
{
    if (section)
        m_sections.push_back(section);
}

/*! Returns the index of the given block function. */
unsigned int Engine::functionIndex(BlockFunc f)
{
    auto it = std::find(m_functions.begin(), m_functions.end(), f);
    if (it != m_functions.end())
        return it - m_functions.begin();
    m_functions.push_back(f);
    return m_functions.size() - 1;
}

/*! Resolves the block and returns the block section in which it has been registered. */
std::shared_ptr<IBlockSection> Engine::blockSection(const std::string &opcode) const
{
    for (auto section : m_sections) {
        auto block = section->resolveBlockCompileFunc(opcode);
        if (block)
            return section;
    }

    return nullptr;
}

/*! Returns the list of broadcasts. */
std::vector<std::shared_ptr<Broadcast>> Engine::broadcasts() const
{
    return m_broadcasts;
}

/*! Sets the list of broadcasts. */
void Engine::setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts)
{
    m_broadcasts = broadcasts;
}

/*! Returns the broadcast at index. */
std::shared_ptr<Broadcast> Engine::broadcastAt(int index) const
{
    return m_broadcasts[index];
}

/*! Returns the index of the broadcast with the given name. */
int Engine::findBroadcast(const std::string &broadcastName) const
{
    int i = 0;
    for (auto broadcast : m_broadcasts) {
        if (broadcast->name() == broadcastName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the index of the broadcast with the given ID. */
int Engine::findBroadcastById(const std::string &broadcastId) const
{
    int i = 0;
    for (auto broadcast : m_broadcasts) {
        if (broadcast->id() == broadcastId)
            return i;
        i++;
    }
    return -1;
}

/*! Registers the broadcast script. */
void libscratchcpp::Engine::addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, std::shared_ptr<Broadcast> broadcast)
{
    auto id = findBroadcast(broadcast->name());
    if (m_broadcastMap.count(id) == 1) {
        std::vector<Script *> &scripts = m_broadcastMap[id];
        scripts.push_back(m_scripts[whenReceivedBlock].get());
    } else
        m_broadcastMap[id] = { m_scripts[whenReceivedBlock].get() };
}

/*! Returns the list of targets. */
std::vector<std::shared_ptr<Target>> Engine::targets() const
{
    return m_targets;
}

/*! Sets the list of targets. */
void Engine::setTargets(const std::vector<std::shared_ptr<Target>> &newTargets)
{
    m_targets = newTargets;

    // Set engine and target in all blocks
    for (auto target : m_targets) {
        auto blocks = target->blocks();
        for (auto block : blocks) {
            block->setEngine(this);
            block->setTarget(target.get());
        }
    }
}

/*! Returns the target at index. */
Target *Engine::targetAt(int index) const
{
    return m_targets[index].get();
}

/*! Returns the index of the target with the given name. */
int Engine::findTarget(const std::string &targetName) const
{
    int i = 0;
    for (auto target : m_targets) {
        if (target->name() == targetName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the list of extension names. */
std::vector<std::string> Engine::extensions() const
{
    return m_extensions;
}

/*! Sets the list of extension names. */
void Engine::setExtensions(const std::vector<std::string> &newExtensions)
{
    m_sections.clear();
    m_extensions = newExtensions;

    // Register standard block sections
    ScratchConfiguration::getExtension<StandardBlocks>()->registerSections(this);

    // Register block sections of extensions
    for (auto ext : m_extensions) {
        IExtension *ptr = ScratchConfiguration::getExtension(ext);
        if (ptr)
            ptr->registerSections(this);
        else
            std::cerr << "Unsupported extension: " << ext << std::endl;
    }
}

/*! Returns the map of scripts (each top level block has a Script object). */
const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &Engine::scripts() const
{
    return m_scripts;
}

/*! Returns the block with the given ID. */
std::shared_ptr<Block> Engine::getBlock(std::string id)
{
    if (id.empty())
        return nullptr;

    for (auto target : m_targets) {
        int index = target->findBlock(id);
        if (index != -1)
            return target->blockAt(index);
    }

    return nullptr;
}

/*! Returns the variable with the given ID. */
std::shared_ptr<Variable> Engine::getVariable(std::string id)
{
    if (id.empty())
        return nullptr;

    for (auto target : m_targets) {
        int index = target->findVariableById(id);
        if (index != -1)
            return target->variableAt(index);
    }

    return nullptr;
}

/*! Returns the Scratch list with the given ID. */
std::shared_ptr<List> Engine::getList(std::string id)
{
    if (id.empty())
        return nullptr;

    for (auto target : m_targets) {
        int index = target->findListById(id);
        if (index != -1)
            return target->listAt(index);
    }

    return nullptr;
}

/*! Returns the broadcast with the given ID. */
std::shared_ptr<Broadcast> Engine::getBroadcast(std::string id)
{
    if (id.empty())
        return nullptr;

    int index = findBroadcastById(id);
    if (index != -1)
        return broadcastAt(index);

    return nullptr;
}

/*! Returns the entity with the given ID. \see IEntity */
std::shared_ptr<IEntity> Engine::getEntity(std::string id)
{
    // Blocks
    auto block = getBlock(id);
    if (block)
        return std::static_pointer_cast<IEntity>(block);

    // Variables
    auto variable = getVariable(id);
    if (variable)
        return std::static_pointer_cast<IEntity>(variable);

    // Lists
    auto list = getList(id);
    if (list)
        return std::static_pointer_cast<IEntity>(list);

    // Broadcasts
    auto broadcast = getBroadcast(id);
    if (broadcast)
        return std::static_pointer_cast<IEntity>(broadcast);

    return nullptr;
}
