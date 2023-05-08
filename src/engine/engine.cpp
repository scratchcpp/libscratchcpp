// SPDX-License-Identifier: Apache-2.0

#include "engine.h"
#include "../blocks/standardblocks.h"
#include "../scratchconfiguration.h"
#include "../scratch/runningscript.h"
#include "iblocksection.h"
#include "compiler.h"
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
 * Compiles all scripts to bytecode.
 * \see Compiler
 */
void Engine::compile()
{
    // Resolve entities by ID
    for (auto target : m_targets) {
        auto blocks = target->blocks();
        for (auto block : blocks) {
            auto section = blockSection(block->opcode());
            block->setNext(getBlock(block->nextId()));
            block->setParent(getBlock(block->parentId()));
            if (section) {
                block->setImplementation(section->resolveBlock(block->opcode()));
                block->setCompileFunction(section->resolveBlockCompileFunc(block->opcode()));
            }

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

    // Compile scripts to bytecode
    for (auto target : m_targets) {
        std::vector<Variable *> variables;
        std::vector<List *> lists;
        std::vector<InputValue *> constInputValues;
        auto blocks = target->blocks();
        for (auto block : blocks) {
            if (block->topLevel()) {
                auto section = blockSection(block->opcode());
                Compiler compiler(this, block);
                compiler.setConstInputValues(constInputValues);
                compiler.setVariables(variables);
                compiler.setLists(lists);
                compiler.compile();
                variables = compiler.variables();
                lists = compiler.lists();
                constInputValues = compiler.constInputValues();
                auto vm = std::make_shared<VirtualMachine>();
                vm->setFunctions(m_functions);
                vm->setConstValues(compiler.constValues());
                vm->setVariables(compiler.variablePtrs());
                vm->setLists(lists);
                vm->setBytecode(compiler.bytecode());
                m_scripts[block] = vm;
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
void Engine::frame(std::chrono::milliseconds timeLimit)
{
    auto frameStartTime = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<RunningScript>> scriptsToRemove;
    auto scripts = m_runningScripts;
    for (auto script : scripts) {
        std::shared_ptr<Block> block;
        m_breakFrame = false;
        m_atomic = true;

        do {
            block = script->currentBlock();
            if (!script->currentBlock()) {
                std::cout << "warning: encountered a null block in one of the scripts" << std::endl;
                scriptsToRemove.push_back(script);
                continue;
            }

            // Call the implementation of the block (if it has an implementation)
            block->run(script.get());

            // Move to next block
            if (m_stayOnCurrentBlock) {
                m_stayOnCurrentBlock = false;
                block = script->currentBlock();
            } else {
                script->moveToNextBlock();
                block = script->currentBlock();
                if (!block) {
                    // There isn't any block to move to
                    // This means the script has finished
                    scriptsToRemove.push_back(script);
                }
            }

            if (timeLimit.count() > 0) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - frameStartTime);
                if (duration > timeLimit)
                    break;
            }
        } while (block != nullptr && !m_breakFrame);
    }

    for (auto script : scriptsToRemove) {
        m_runningScripts.erase(std::remove(m_runningScripts.begin(), m_runningScripts.end(), script), m_runningScripts.end());
        scriptCount--;
    }
}

/*!
 * Calls all "when green flag clicked" blocks.
 * \note Nothing will happen until run() or frame() is called.
 */
void Engine::start()
{
    scriptCount = 0;
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
    scriptCount = 0;
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
        auto script = std::make_shared<RunningScript>(topLevelBlock, target, this);
        m_runningScripts.push_back(script);
        script->moveToNextBlock();
        scriptCount++;
    }
}

/*! Stops the given script. */
void Engine::stopScript(RunningScript *script)
{
    assert(script);
    int i = 0;
    for (auto s : m_runningScripts) {
        if (s.get() == script) {
            m_runningScripts.erase(m_runningScripts.begin() + i);
            break;
        }
        i++;
    }
    scriptCount--;
}

/*!
 * Stops all scripts in the given target.
 * \param[in] target The Target to stop.
 * \param[in] exceptScript Sets this parameter to stop all scripts except the given script.
 */
void Engine::stopTarget(Target *target, RunningScript *exceptScript)
{
    std::vector<RunningScript *> scripts;
    for (auto script : m_runningScripts) {
        RunningScript *s = script.get();
        if ((script->target().get() == target) && (s != exceptScript))
            scripts.push_back(s);
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
        frame(frameDuration);
        if (scriptCount <= 0)
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

/*!
 * Call this from a block implementation to force a "screen refresh".
 * \note This has no effect in "run without screen refresh" custom blocks.
 */
void Engine::breakFrame()
{
    m_breakFrame = true;
}

/*! Call this from a block implementation to prevent the engine from moving to next block. */
void libscratchcpp::Engine::stayOnCurrentBlock()
{
    m_stayOnCurrentBlock = true;
}

/*!
 * Call this from a block implementation to force screen refresh after the current loop period (if the block is in a loop).\n
 * This should for example called from some motion and looks blocks.
 */
void Engine::breakAtomicScript()
{
    m_atomic = false;
}

/*! Returns true if the current script is atomic (e. g. there's a loop running without screen refresh). */
bool Engine::isAtomic()
{
    return m_atomic;
}

/*! Registers the given block section. */
void Engine::registerSection(std::shared_ptr<IBlockSection> section)
{
    if (section)
        m_sections.push_back(section);
}

/*! Returns a pointer to the implementation function of the given block opcode. */
BlockImpl Engine::resolveBlock(const std::string &opcode) const
{
    for (auto section : m_sections) {
        auto block = section->resolveBlock(opcode);
        if (block)
            return block;
    }

    return nullptr;
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
        auto block = section->resolveBlock(opcode);
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
Broadcast *Engine::broadcastAt(int index) const
{
    return m_broadcasts[index].get();
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

    return nullptr;
}
