// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/iblocksection.h>
#include <scratchcpp/script.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/keyevent.h>
#include <cassert>
#include <iostream>
#include <thread>

#include "engine.h"
#include "blocksectioncontainer.h"
#include "timer.h"
#include "../../blocks/standardblocks.h"

using namespace libscratchcpp;

Engine::Engine() :
    m_defaultTimer(std::make_unique<Timer>()),
    m_timer(m_defaultTimer.get())
{
}

void Engine::clear()
{
    m_sections.clear();
    m_targets.clear();
    m_broadcasts.clear();
}

// Resolves ID references and sets pointers of entities.
void Engine::resolveIds()
{
    for (auto target : m_targets) {
        std::cout << "Processing target " << target->name() << "..." << std::endl;
        auto blocks = target->blocks();
        for (auto block : blocks) {
            auto container = blockSectionContainer(block->opcode());
            block->setNext(getBlock(block->nextId()));
            block->setParent(getBlock(block->parentId()));
            if (container)
                block->setCompileFunction(container->resolveBlockCompileFunc(block->opcode()));

            auto inputs = block->inputs();
            for (auto input : inputs) {
                input->setValueBlock(getBlock(input->valueBlockId()));
                if (container)
                    input->setInputId(container->resolveInput(input->name()));
                input->primaryValue()->setValuePtr(getEntity(input->primaryValue()->valueId()));
                input->secondaryValue()->setValuePtr(getEntity(input->primaryValue()->valueId()));
            }

            auto fields = block->fields();
            for (auto field : fields) {
                field->setValuePtr(getEntity(field->valueId()));
                if (container) {
                    field->setFieldId(container->resolveField(field->name()));
                    if (!field->valuePtr())
                        field->setSpecialValueId(container->resolveFieldValue(field->value().toString()));
                }
            }

            block->updateInputMap();
            block->updateFieldMap();
        }
    }
}

void Engine::compile()
{
    // Resolve entities by ID
    resolveIds();

    // Compile scripts to bytecode
    for (auto target : m_targets) {
        std::cout << "Compiling scripts in target " << target->name() << "..." << std::endl;
        std::unordered_map<std::string, unsigned int *> procedureBytecodeMap;
        Compiler compiler(this, target.get());
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
                m_scripts[block]->setVariables(compiler.variables());
                m_scripts[block]->setLists(compiler.lists());
            }
        }
    }
}

void Engine::frame()
{
    m_lockFrame = false;

    for (int i = 0; i < m_runningScripts.size(); i++) {
        auto script = m_runningScripts[i];
        m_breakFrame = false;

        do {
            script->run();
            if (script->atEnd() && m_running) {
                for (auto &[key, value] : m_runningBroadcastMap) {
                    size_t index = 0;

                    for (const auto &pair : value) {
                        if (pair.second == script.get()) {
                            value.erase(value.begin() + index);
                            break;
                        }

                        index++;
                    }
                }

                if (std::find(m_scriptsToRemove.begin(), m_scriptsToRemove.end(), script.get()) == m_scriptsToRemove.end())
                    m_scriptsToRemove.push_back(script.get());
            }
        } while (!script->atEnd() && !m_breakFrame);
    }

    assert(m_running || m_scriptsToRemove.empty());

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

void Engine::start()
{
    m_timer->reset();
    m_running = true;

    for (auto target : m_targets) {
        auto gfBlocks = target->greenFlagBlocks();
        for (auto block : gfBlocks)
            startScript(block, target);
    }
}

void Engine::stop()
{
    m_runningScripts.clear();
    m_scriptsToRemove.clear();
    m_running = false;
}

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

void Engine::broadcast(unsigned int index, VirtualMachine *sourceScript, bool wait)
{
    if (index < 0 || index >= m_broadcasts.size())
        return;

    broadcastByPtr(m_broadcasts[index].get(), sourceScript, wait);
}

void Engine::broadcastByPtr(Broadcast *broadcast, VirtualMachine *sourceScript, bool wait)
{
    const std::vector<Script *> &scripts = m_broadcastMap[broadcast];

    for (auto script : scripts) {
        std::vector<VirtualMachine *> runningBroadcastScripts;

        for (auto vm : m_runningScripts) {
            if (vm->script() == script) {
                runningBroadcastScripts.push_back(vm.get());
            }
        }

        // Reset running scripts
        for (VirtualMachine *vm : runningBroadcastScripts) {
            vm->reset();

            // Remove the script from scripts to remove because it's going to run again
            m_scriptsToRemove.erase(std::remove(m_scriptsToRemove.begin(), m_scriptsToRemove.end(), vm), m_scriptsToRemove.end());
            assert(std::find(m_scriptsToRemove.begin(), m_scriptsToRemove.end(), vm) == m_scriptsToRemove.end());

            auto &scripts = m_runningBroadcastMap[broadcast];

            for (auto &pair : scripts) {
                if (pair.second->script() == script)
                    pair.first = sourceScript;
            }

            if (script == sourceScript->script())
                sourceScript->stop(false, !wait);
            else
                sourceScript->stop(true, false);
        }

        // Start scripts which are not running
        Target *root = script->target();
        std::vector<Target *> targets = { root };

        if (!root->isStage()) {
            Sprite *sprite = dynamic_cast<Sprite *>(root);
            assert(sprite);
            auto children = sprite->allChildren();

            for (auto child : children)
                targets.push_back(child.get());
        }

        for (Target *target : targets) {
            auto it = std::find_if(runningBroadcastScripts.begin(), runningBroadcastScripts.end(), [target](VirtualMachine *vm) { return vm->target() == target; });

            if (it == runningBroadcastScripts.end()) {
                auto vm = script->start(target);
                m_runningScripts.push_back(vm);
                m_runningBroadcastMap[broadcast].push_back({ sourceScript, vm.get() });
            }
        }
    }
}

void Engine::stopScript(VirtualMachine *vm)
{
    assert(vm);
    if (std::find(m_scriptsToRemove.begin(), m_scriptsToRemove.end(), vm) == m_scriptsToRemove.end())
        m_scriptsToRemove.push_back(vm);
}

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

void Engine::initClone(Sprite *clone)
{
    if (!clone)
        return;

    Sprite *source = clone->cloneParent();
    Target *root = clone->cloneRoot();
    assert(source);
    assert(root);

    if (!source || !root)
        return;

    auto it = m_cloneInitScriptsMap.find(root);

    if (it != m_cloneInitScriptsMap.cend()) {
        const auto &scripts = it->second;

#ifndef NDEBUG
        // Since we're initializing the clone, it shouldn't have any running scripts
        for (const auto script : m_runningScripts)
            assert((script->target() != clone) || (std::find(m_scriptsToRemove.begin(), m_scriptsToRemove.end(), script.get()) != m_scriptsToRemove.end()));
#endif

        for (auto script : scripts) {
            auto vm = script->start(clone);
            m_runningScripts.push_back(vm);
        }
    }
}

void Engine::run()
{
    updateFrameDuration();
    start();

    while (true) {
        auto lastFrameTime = std::chrono::steady_clock::now();
        m_skipFrame = false;

        // Execute the frame
        frame();
        if (m_runningScripts.size() <= 0)
            break;

        // Sleep until the time for the next frame
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
        auto sleepTime = m_frameDuration - elapsedTime;
        bool timeOut = sleepTime <= std::chrono::milliseconds::zero();

        if (!timeOut && !m_skipFrame)
            std::this_thread::sleep_for(sleepTime);

        if ((m_skipFrame && timeOut) || !m_skipFrame) {
            // TODO: Repaint here
        }

        lastFrameTime = currentTime;
    }

    stop();
}

bool Engine::isRunning() const
{
    return m_running;
}

double Engine::fps() const
{
    return m_fps;
}

void Engine::setFps(double fps)
{
    m_fps = fps;
    updateFrameDuration();
}

bool Engine::keyPressed(const std::string &name) const
{
    if (name == "any") {
        if (m_anyKeyPressed)
            return true;

        for (const auto &[key, value] : m_keyMap) {
            if (value)
                return true;
        }

        return false;
    }

    KeyEvent event(name);
    auto it = m_keyMap.find(event.name());

    if (it == m_keyMap.cend())
        return false;
    else
        return it->second;
}

void Engine::setKeyState(const std::string &name, bool pressed)
{
    KeyEvent event(name);
    m_keyMap[event.name()] = pressed;
}

void Engine::setAnyKeyPressed(bool pressed)
{
    m_anyKeyPressed = pressed;
}

double Engine::mouseX() const
{
    return m_mouseX;
}

void Engine::setMouseX(double x)
{
    m_mouseX = x;
}

double Engine::mouseY() const
{
    return m_mouseY;
}

void Engine::setMouseY(double y)
{
    m_mouseY = y;
}

bool Engine::mousePressed() const
{
    return m_mousePressed;
}

void Engine::setMousePressed(bool pressed)
{
    m_mousePressed = pressed;
}

unsigned int Engine::stageWidth() const
{
    return m_stageWidth;
}

void Engine::setStageWidth(unsigned int width)
{
    m_stageWidth = width;
}

unsigned int Engine::stageHeight() const
{
    return m_stageHeight;
}

void Engine::setStageHeight(unsigned int height)
{
    m_stageHeight = height;
}

bool Engine::broadcastRunning(unsigned int index, VirtualMachine *sourceScript)
{
    if (index < 0 || index >= m_broadcasts.size())
        return false;

    return broadcastByPtrRunning(m_broadcasts[index].get(), sourceScript);
}

bool Engine::broadcastByPtrRunning(Broadcast *broadcast, VirtualMachine *sourceScript)
{
    auto it = m_runningBroadcastMap.find(broadcast);

    if (it == m_runningBroadcastMap.cend())
        return false;

    const auto &scripts = it->second;

    for (const auto &pair : scripts) {
        if (pair.first == sourceScript)
            return true;
    }

    return false;
}

void Engine::breakFrame()
{
    m_breakFrame = true;
}

bool Engine::breakingCurrentFrame()
{
    return m_breakFrame;
}

void Engine::skipFrame()
{
    if (!m_lockFrame) {
        breakFrame();
        m_skipFrame = true;
    }
}

void Engine::lockFrame()
{
    m_skipFrame = false;
    m_lockFrame = true;
}

ITimer *Engine::timer() const
{
    return m_timer;
}

void Engine::setTimer(ITimer *timer)
{
    m_timer = timer;
}

void Engine::registerSection(std::shared_ptr<IBlockSection> section)
{
    if (section) {
        if (m_sections.find(section) != m_sections.cend()) {
            std::cerr << "Warning: block section \"" << section->name() << "\" is already registered" << std::endl;
            return;
        }

        m_sections[section] = std::make_unique<BlockSectionContainer>();
        section->registerBlocks(this);
    }
}

std::vector<std::shared_ptr<IBlockSection>> Engine::registeredSections() const
{
    std::vector<std::shared_ptr<IBlockSection>> ret;

    for (const auto &[key, value] : m_sections)
        ret.push_back(key);

    return ret;
}

unsigned int Engine::functionIndex(BlockFunc f)
{
    auto it = std::find(m_functions.begin(), m_functions.end(), f);
    if (it != m_functions.end())
        return it - m_functions.begin();
    m_functions.push_back(f);
    return m_functions.size() - 1;
}

void Engine::addCompileFunction(IBlockSection *section, const std::string &opcode, BlockComp f)
{
    auto container = blockSectionContainer(section);

    if (container)
        container->addCompileFunction(opcode, f);
}

void Engine::addHatBlock(IBlockSection *section, const std::string &opcode)
{
    auto container = blockSectionContainer(section);

    if (container)
        container->addHatBlock(opcode);
}

void Engine::addInput(IBlockSection *section, const std::string &name, int id)
{
    auto container = blockSectionContainer(section);

    if (container)
        container->addInput(name, id);
}

void Engine::addField(IBlockSection *section, const std::string &name, int id)
{
    auto container = blockSectionContainer(section);

    if (container)
        container->addField(name, id);
}

void Engine::addFieldValue(IBlockSection *section, const std::string &value, int id)
{
    auto container = blockSectionContainer(section);

    if (container)
        container->addFieldValue(value, id);
}

const std::vector<std::shared_ptr<Broadcast>> &Engine::broadcasts() const
{
    return m_broadcasts;
}

void Engine::setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts)
{
    m_broadcasts = broadcasts;
}

std::shared_ptr<Broadcast> Engine::broadcastAt(int index) const
{
    if (index < 0 || index >= m_broadcasts.size())
        return nullptr;

    return m_broadcasts[index];
}

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

void Engine::addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, Broadcast *broadcast)
{
    if (m_broadcastMap.count(broadcast) == 1) {
        std::vector<Script *> &scripts = m_broadcastMap[broadcast];
        // TODO: Do not allow adding existing scripts
        scripts.push_back(m_scripts[whenReceivedBlock].get());
    } else {
        m_broadcastMap[broadcast] = { m_scripts[whenReceivedBlock].get() };

        // Create a vector of running scripts for this broadcast
        // so we don't need to check if it's there
        m_runningBroadcastMap[broadcast] = {};
    }
}

void Engine::addCloneInitScript(std::shared_ptr<Block> hatBlock)
{
    Target *target = hatBlock->target();
    Script *script = m_scripts[hatBlock].get();
    auto it = m_cloneInitScriptsMap.find(target);

    if (it == m_cloneInitScriptsMap.cend())
        m_cloneInitScriptsMap[target] = { script };
    else {
        auto &scripts = it->second;

        if (std::find(scripts.begin(), scripts.end(), script) == scripts.cend())
            scripts.push_back(script);
    }
}

void Engine::addKeyPressScript(std::shared_ptr<Block> hatBlock, std::string keyName)
{
    std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::tolower);
    Script *script = m_scripts[hatBlock].get();
    auto it = m_whenKeyPressedScripts.find(keyName);

    if (it == m_whenKeyPressedScripts.cend())
        m_whenKeyPressedScripts[keyName] = { script };
    else {
        auto &scripts = it->second;

        if (std::find(scripts.begin(), scripts.end(), script) == scripts.cend())
            scripts.push_back(script);
    }
}

const std::vector<std::shared_ptr<Target>> &Engine::targets() const
{
    return m_targets;
}

void Engine::setTargets(const std::vector<std::shared_ptr<Target>> &newTargets)
{
    m_targets = newTargets;
    m_variableOwners.clear();
    m_listOwners.clear();

    for (auto target : m_targets) {
        // Set engine in the target
        target->setEngine(this);
        auto blocks = target->blocks();

        for (auto block : blocks) {
            // Set engine and target in the block
            block->setEngine(this);
            block->setTarget(target.get());
        }

        // Add variables to owner map
        const auto &variables = target->variables();

        for (auto variable : variables)
            m_variableOwners[variable.get()] = target.get();

        // Add lists to owner map
        const auto &lists = target->lists();

        for (auto list : lists)
            m_listOwners[list.get()] = target.get();
    }
}

Target *Engine::targetAt(int index) const
{
    if (index < 0 || index >= m_targets.size())
        return nullptr;

    return m_targets[index].get();
}

int Engine::findTarget(const std::string &targetName) const
{
    int i = 0;
    for (auto target : m_targets) {
        if ((target->isStage() && targetName == "_stage_") || (!target->isStage() && target->name() == targetName))
            return i;
        i++;
    }
    return -1;
}

Stage *Engine::stage() const
{
    auto it = std::find_if(m_targets.begin(), m_targets.end(), [](std::shared_ptr<Target> target) { return target && target->isStage(); });

    if (it == m_targets.end())
        return nullptr;
    else
        return dynamic_cast<Stage *>((*it).get());
}

Target *Engine::variableOwner(Variable *variable) const
{
    auto it = m_variableOwners.find(variable);

    if (it == m_variableOwners.cend())
        return nullptr;

    return it->second;
}

Target *Engine::listOwner(List *list) const
{
    auto it = m_listOwners.find(list);

    if (it == m_listOwners.cend())
        return nullptr;

    return it->second;
}

const std::vector<std::string> &Engine::extensions() const
{
    return m_extensions;
}

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

const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &Engine::scripts() const
{
    return m_scripts;
}

// Returns the block with the given ID.
std::shared_ptr<Block> Engine::getBlock(const std::string &id)
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

// Returns the variable with the given ID.
std::shared_ptr<Variable> Engine::getVariable(const std::string &id)
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

// Returns the Scratch list with the given ID.
std::shared_ptr<List> Engine::getList(const std::string &id)
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

// Returns the broadcast with the given ID.
std::shared_ptr<Broadcast> Engine::getBroadcast(const std::string &id)
{
    if (id.empty())
        return nullptr;

    int index = findBroadcastById(id);
    if (index != -1)
        return broadcastAt(index);

    return nullptr;
}

// Returns the entity with the given ID. \see IEntity
std::shared_ptr<Entity> Engine::getEntity(const std::string &id)
{
    // Blocks
    auto block = getBlock(id);
    if (block)
        return std::static_pointer_cast<Entity>(block);

    // Variables
    auto variable = getVariable(id);
    if (variable)
        return std::static_pointer_cast<Entity>(variable);

    // Lists
    auto list = getList(id);
    if (list)
        return std::static_pointer_cast<Entity>(list);

    // Broadcasts
    auto broadcast = getBroadcast(id);
    if (broadcast)
        return std::static_pointer_cast<Entity>(broadcast);

    return nullptr;
}

std::shared_ptr<IBlockSection> Engine::blockSection(const std::string &opcode) const
{
    for (const auto &pair : m_sections) {
        auto block = pair.second->resolveBlockCompileFunc(opcode);
        if (block)
            return pair.first;
    }

    return nullptr;
}

BlockSectionContainer *Engine::blockSectionContainer(const std::string &opcode) const
{
    for (const auto &pair : m_sections) {
        auto block = pair.second->resolveBlockCompileFunc(opcode);
        if (block)
            return pair.second.get();
    }

    return nullptr;
}

BlockSectionContainer *Engine::blockSectionContainer(IBlockSection *section) const
{
    if (!section)
        return nullptr;

    for (const auto &pair : m_sections) {
        if (pair.first.get() == section)
            return pair.second.get();
    }

    return nullptr;
}

void Engine::updateFrameDuration()
{
    m_frameDuration = std::chrono::milliseconds(static_cast<long>(1000 / m_fps));
}
