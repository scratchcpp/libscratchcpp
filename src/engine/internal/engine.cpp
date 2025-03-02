// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/iextension.h>
#include <scratchcpp/script.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/keyevent.h>
#include <scratchcpp/sound.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/thread.h>
#include <cassert>
#include <iostream>

#include "engine.h"
#include "timer.h"
#include "clock.h"
#include "audio/iaudioengine.h"
#include "blocks/blocks.h"
#include "scratch/monitor_p.h"

using namespace libscratchcpp;

const std::unordered_map<Engine::HatType, bool> Engine::m_hatRestartExistingThreads = {
    { HatType::WhenTouchingObject, false }, { HatType::GreenFlag, true },   { HatType::BroadcastReceived, true }, { HatType::BackdropChanged, true },
    { HatType::CloneInit, false },          { HatType::KeyPressed, false }, { HatType::TargetClicked, true },     { HatType::WhenGreaterThan, false }
};

const std::unordered_map<Engine::HatType, bool> Engine::m_hatEdgeActivated = {
    { HatType::WhenTouchingObject, true }, { HatType::GreenFlag, false },  { HatType::BroadcastReceived, false }, { HatType::BackdropChanged, false },
    { HatType::CloneInit, false },         { HatType::KeyPressed, false }, { HatType::TargetClicked, false },     { HatType::WhenGreaterThan, true }
};

Engine::Engine() :
    m_defaultTimer(std::make_unique<Timer>()),
    m_timer(m_defaultTimer.get()),
    m_clock(Clock::instance().get()),
    m_audioEngine(IAudioEngine::instance())
{
}

Engine::~Engine()
{
    m_clones.clear();
    m_sortedDrawables.clear();
}

void Engine::clear()
{
    stop();

    for (auto monitor : m_monitors)
        m_monitorRemoved(monitor.get(), monitor->impl->iface);

    for (auto thread : m_threads) {
        if (!thread->isFinished())
            m_threadAboutToStop(thread.get());
    }

    m_targets.clear();
    m_broadcasts.clear();
    m_monitors.clear();
    m_extensions.clear();
    m_broadcastMap.clear();
    m_sortedDrawables.clear();
    m_threads.clear();
    m_threadsToStop.clear();
    m_scripts.clear();

    m_whenTouchingObjectHats.clear();
    m_greenFlagHats.clear();
    m_backdropChangeHats.clear();
    m_broadcastHats.clear();
    m_cloneInitHats.clear();
    m_whenKeyPressedHats.clear();
    m_whenTargetClickedHats.clear();
    m_whenGreaterThanHats.clear();

    m_scriptHatFields.clear();
    m_edgeActivatedHatValues.clear();

    m_running = false;
    m_frameActivity = false;

    m_unsupportedBlocks.clear();
}

// Resolves ID references and sets pointers of entities.
void Engine::resolveIds()
{
    for (auto target : m_targets) {
        std::cout << "Processing target " << target->name() << "..." << std::endl;
        const auto &blocks = target->blocks();
        for (auto block : blocks) {
            IExtension *ext = blockExtension(block->opcode());
            block->setNext(getBlock(block->nextId(), target.get()).get());
            block->setParent(getBlock(block->parentId(), target.get()).get());

            if (ext) {
                block->setCompileFunction(resolveBlockCompileFunc(ext, block->opcode()));
                block->setHatPredicateCompileFunction(resolveHatPredicateCompileFunc(ext, block->opcode()));
            }

            const auto &inputs = block->inputs();
            for (const auto &input : inputs) {
                input->setValueBlock(getBlock(input->valueBlockId(), target.get()).get());

                if (ext)
                    input->setInputId(resolveInput(ext, input->name()));

                InputValue *value = input->primaryValue();
                std::string id = value->valueId(); // no reference!
                value->setValuePtr(getEntity(id, target.get()));
                assert(input->secondaryValue()->type() != InputValue::Type::Variable && input->secondaryValue()->type() != InputValue::Type::List); // secondary values never have a variable or list
                input->secondaryValue()->setValuePtr(getEntity(input->secondaryValue()->valueId(), target.get()));

                // Add missing variables and lists
                if (!value->valuePtr()) {
                    if (value->type() == InputValue::Type::Variable) {
                        assert(!id.empty());
                        std::cout << "warning: variable " << value->value().toString() << " referenced by an input missing, creating in stage" << std::endl;
                        auto var = std::make_shared<Variable>(id, value->value().toString());
                        value->setValuePtr(var);
                        stage()->addVariable(var);
                    } else if (value->type() == InputValue::Type::List) {
                        assert(!id.empty());
                        std::cout << "warning: list " << value->value().toString() << " referenced by an input missing, creating in stage" << std::endl;
                        std::shared_ptr<List> list = std::make_shared<List>(id, value->value().toString());
                        value->setValuePtr(list);
                        stage()->addList(list);
                    }
                }
            }

            const auto &fields = block->fields();
            for (auto field : fields) {
                std::string id = field->valueId(); // no reference!
                field->setValuePtr(getEntity(id, target.get()));

                if (ext) {
                    field->setFieldId(resolveField(ext, field->name()));

                    if (!field->valuePtr())
                        field->setSpecialValueId(resolveFieldValue(ext, field->value().toString()));
                }

                // TODO: Move field information out of Engine
                if (!field->valuePtr()) {
                    if (field->name() == "VARIABLE") {
                        assert(!id.empty());
                        std::cout << "warning: variable " << field->value().toString() << " referenced by a field missing, creating in stage" << std::endl;
                        auto var = std::make_shared<Variable>(id, field->value().toString());
                        field->setValuePtr(var);
                        stage()->addVariable(var);
                    } else if (field->name() == "LIST") {
                        assert(!id.empty());
                        std::cout << "warning: list " << field->value().toString() << " referenced by a field missing, creating in stage" << std::endl;
                        std::shared_ptr<List> list = std::make_shared<List>(id, field->value().toString());
                        field->setValuePtr(list);
                        stage()->addList(list);
                    }
                }
            }

            block->updateInputMap();
            block->updateFieldMap();

            auto comment = getComment(block->commentId(), target.get());
            block->setComment(comment);

            if (comment) {
                comment->setBlock(block.get());
                assert(comment->blockId() == block->id());
            }
        }
    }

    for (auto monitor : m_monitors) {
        auto block = monitor->block();
        auto ext = blockExtension(block->opcode());

        if (ext)
            block->setCompileFunction(resolveBlockCompileFunc(ext, block->opcode()));

        monitor->setExtension(ext);

        const auto &fields = block->fields();
        Target *target;

        if (monitor->sprite())
            target = monitor->sprite();
        else
            target = stage();

        assert(target);

        for (auto field : fields) {
            field->setValuePtr(getEntity(field->valueId(), target));

            if (ext) {
                field->setFieldId(resolveField(ext, field->name()));

                if (!field->valuePtr())
                    field->setSpecialValueId(resolveFieldValue(ext, field->value().toString()));
            }

            // TODO: Move field information out of Engine
            if (field->name() == "VARIABLE") {
                std::string name = field->value().toString();
                int index = target->findVariable(name);
                std::shared_ptr<Variable> var;

                if (index == -1) {
                    var = std::make_shared<Variable>(monitor->id(), name);
                    index = target->addVariable(var);
                } else
                    var = target->variableAt(index);

                field->setValuePtr(target->variableAt(index));
                assert(var);
                var->setMonitor(monitor.get());
            } else if (field->name() == "LIST") {
                std::string name = field->value().toString();
                int index = target->findList(name);
                std::shared_ptr<List> list;

                if (index == -1) {
                    list = std::make_shared<List>(monitor->id(), name);
                    index = target->addList(list);
                } else
                    list = target->listAt(index);

                field->setValuePtr(target->listAt(index));
                assert(list);
                list->setMonitor(monitor.get());
            }
        }

        block->updateInputMap();
        block->updateFieldMap();
    }
}

void Engine::compile()
{
    // Resolve entities by ID
    resolveIds();

    // Compile scripts
    for (auto target : m_targets) {
        std::cout << "Compiling scripts in target " << target->name() << "..." << std::endl;
        auto ctx = Compiler::createContext(this, target.get());
        m_compilerContexts[target.get()] = ctx;
        Compiler compiler(ctx.get());
        const auto &blocks = target->blocks();

        for (auto block : blocks) {
            if (block->topLevel() && !block->isTopLevelReporter() && !block->shadow()) {
                auto ext = blockExtension(block->opcode());
                if (ext) {
                    auto script = std::make_shared<Script>(target.get(), block.get(), this);
                    m_scripts[block.get()] = script;
                    script->setCode(compiler.compile(block.get()));

                    if (block->hatPredicateCompileFunction())
                        script->setHatPredicateCode(compiler.compile(block.get(), Compiler::CodeType::HatPredicate));
                } else {
                    std::cout << "warning: unsupported top level block: " << block->opcode() << std::endl;
                    m_unsupportedBlocks.insert(block->opcode());
                }
            }
        }

        const auto &unsupportedBlocks = compiler.unsupportedBlocks();

        for (const std::string &opcode : unsupportedBlocks)
            m_unsupportedBlocks.insert(opcode);

        // Preoptimize to avoid lag when starting scripts for the first time
        std::cout << "Optimizing target " << target->name() << "..." << std::endl;
        compiler.preoptimize();
    }

    // Compile monitor blocks to bytecode
    std::cout << "Compiling stage monitors..." << std::endl;

    for (auto monitor : m_monitors)
        compileMonitor(monitor);
}

void Engine::start()
{
    stop();

    m_eventLoopMutex.lock();
    m_timer->reset();
    m_edgeActivatedHatValues.clear();
    m_running = true;

    // Start "when green flag clicked" scripts
    startHats(HatType::GreenFlag, {}, nullptr);

    m_eventLoopMutex.unlock();
}

void Engine::stop()
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L2057-L2081
    if (m_activeThread) {
        stopThread(m_activeThread.get());
        // NOTE: The project should continue running even after "stop all" is called and the remaining threads should be stepped once.
        // The remaining threads can even start new threads which will ignore the "stop all" call and will "restart" the project.
        // This is probably a bug in the Scratch VM, but let's keep it here to keep it compatible.
        m_threadsToStop = m_threads;

        // Remove threads owned by clones because clones are going to be deleted (#547)
        m_threads.erase(
            std::remove_if(
                m_threads.begin(),
                m_threads.end(),
                [](std::shared_ptr<Thread> thread) {
                    assert(thread);
                    Target *target = thread->target();
                    assert(target);

                    if (!target->isStage()) {
                        Sprite *sprite = static_cast<Sprite *>(target);

                        if (sprite->isClone())
                            return true;
                    }

                    return false;
                }),
            m_threads.end());
    } else {
        // If there isn't any active thread, it means the project was stopped from the outside
        // In this case all threads should be removed and the project should be considered stopped
        for (auto thread : m_threads) {
            if (!thread->isFinished())
                m_threadAboutToStop(thread.get());
        }

        m_threads.clear();
        m_running = false;
    }

    m_frameActivity = false;
    deleteClones();
    stopSounds();
    m_stopped();
}

Thread *Engine::startScript(Block *topLevelBlock, Target *target)
{
    return pushThread(topLevelBlock, target).get();
}

void Engine::broadcast(int index, Thread *sender, bool wait)
{
    if (index < 0 || index >= m_broadcasts.size())
        return;

    broadcastByPtr(m_broadcasts[index].get(), sender, wait);
}

void Engine::broadcastByPtr(Broadcast *broadcast, Thread *sender, bool wait)
{
    startHats(HatType::BroadcastReceived, { { HatField::BroadcastOption, broadcast } }, nullptr);
    addBroadcastPromise(broadcast, sender, wait);
}

void Engine::startBackdropScripts(Broadcast *broadcast, Thread *sender, bool wait)
{
    startHats(HatType::BackdropChanged, { { HatField::Backdrop, broadcast->name() } }, nullptr);
    addBroadcastPromise(broadcast, sender, wait);
}

void Engine::stopScript(Thread *vm)
{
    stopThread(vm);
}

void Engine::stopTarget(Target *target, Thread *exceptScript)
{
    std::vector<Thread *> threads;

    for (auto thread : m_threads) {
        if ((thread->target() == target) && (thread.get() != exceptScript))
            threads.push_back(thread.get());
    }

    for (auto thread : threads)
        stopThread(thread);
}

void Engine::initClone(std::shared_ptr<Sprite> clone)
{
    if (!clone || ((m_cloneLimit >= 0) && (m_clones.size() >= m_cloneLimit)))
        return;

    Target *root = clone->cloneSprite();
    assert(root);

    if (!root)
        return;

#ifndef NDEBUG
    // Since we're initializing the clone, it shouldn't have any running scripts
    for (auto thread : m_threads)
        assert(thread->target() != clone.get() || thread->isFinished());
#endif

    startHats(HatType::CloneInit, {}, clone.get());

    assert(std::find(m_clones.begin(), m_clones.end(), clone) == m_clones.end());
    assert(std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), clone.get()) == m_sortedDrawables.end());
    m_clones.insert(clone);
    m_sortedDrawables.push_back(clone.get()); // execution order needs to be updated after this
    m_sortedDrawables.push_back(clone->bubble());
}

void Engine::deinitClone(std::shared_ptr<Sprite> clone)
{
    m_clones.erase(clone);
    m_sortedDrawables.erase(std::remove(m_sortedDrawables.begin(), m_sortedDrawables.end(), clone->bubble()), m_sortedDrawables.end());
    m_sortedDrawables.erase(std::remove(m_sortedDrawables.begin(), m_sortedDrawables.end(), clone.get()), m_sortedDrawables.end());
}

void Engine::stopSounds()
{
    for (auto target : m_targets) {
        const auto &sounds = target->sounds();

        for (auto sound : sounds)
            sound->stop();
    }
}

double Engine::globalVolume() const
{
    return m_audioEngine->volume() * 100;
}

void Engine::setGlobalVolume(double volume)
{
    m_audioEngine->setVolume(volume / 100);
}

void Engine::updateMonitors()
{
    // Execute the "script" of each visible monitor
    for (auto monitor : m_monitors) {
        if (monitor->visible()) {
            auto script = monitor->script();

            if (script) {
                auto thread = script->start();
                ValueData value = thread->runReporter();
                monitor->updateValue(Value(value));
                value_free(&value);
            }
        }
    }
}

void Engine::step()
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L2087C6-L2155
    updateFrameDuration();

    // Clean up threads that were told to stop during or since the last step
    m_threads.erase(std::remove_if(m_threads.begin(), m_threads.end(), [](std::shared_ptr<Thread> thread) { return thread->isFinished(); }), m_threads.end());

    // Find all edge-activated hats, and add them to threads to be evaluated
    for (auto const &[hatType, edgeActivated] : m_hatEdgeActivated) {
        if (edgeActivated) {
            auto newThreads = startHats(hatType, {}, nullptr);

            // Process edge-triggered hats (must happen here because of Scratch 2 compatibility)
            // Processing the hats means running their predicates (if they didn't change their return value from false to true, remove the threads)
            for (auto thread : newThreads) {
                bool oldValue = false;
                auto hatBlock = thread->script()->topBlock();
                assert(hatBlock);

                Target *target = hatBlock->target();
                assert(target);
                auto it = m_edgeActivatedHatValues.find(hatBlock);

                if (it == m_edgeActivatedHatValues.cend()) {
                    m_edgeActivatedHatValues[hatBlock] = {};
                } else {
                    auto &map = it->second;
                    auto it = map.find(target);

                    if (it != map.cend())
                        oldValue = it->second;
                }

                bool newValue = thread->script()->runHatPredicate(hatBlock->target());
                bool edgeWasActivated = !oldValue && newValue; // changed from false true
                m_edgeActivatedHatValues[hatBlock][target] = newValue;

                if (!edgeWasActivated)
                    stopThread(thread.get());
            }
        }
    }

    // Check running threads (must be done here)
    m_frameActivity = !m_threads.empty();

    // Resolve stopped broadcast scripts
    std::vector<Broadcast *> resolved;
    std::vector<Thread *> resolvedThreads;

    for (const auto &[broadcast, senderThread] : m_broadcastSenders) {
        std::unordered_map<Broadcast *, std::vector<Script *>> *broadcastMap = nullptr;

        if (broadcast->isBackdropBroadcast()) {
            // This broadcast belongs to a backdrop
            assert(m_broadcastMap.find(broadcast) == m_broadcastMap.cend());
            broadcastMap = &m_backdropBroadcastMap;
        } else {
            // This is a regular broadcast
            assert(m_backdropBroadcastMap.find(broadcast) == m_backdropBroadcastMap.cend());
            broadcastMap = &m_broadcastMap;
        }

        bool found = false;

        if (broadcastMap->find(broadcast) != broadcastMap->cend()) {
            const auto &scripts = (*broadcastMap)[broadcast];

            for (auto script : scripts) {
                if (std::find_if(m_threads.begin(), m_threads.end(), [script](std::shared_ptr<Thread> thread) { return thread->script() == script; }) != m_threads.end()) {
                    found = true;
                    break;
                }
            }
        }

        if (found) {
            // If a broadcast with the same name but different case
            // was considered stopped before, restore the promise.
            if (std::find(resolvedThreads.begin(), resolvedThreads.end(), senderThread) != resolvedThreads.end()) {
                senderThread->promise();
                resolvedThreads.erase(std::remove(resolvedThreads.begin(), resolvedThreads.end(), senderThread), resolvedThreads.end());
            }
        } else {
            Thread *th = senderThread;

            if (std::find_if(m_threads.begin(), m_threads.end(), [th](std::shared_ptr<Thread> thread) { return thread.get() == th; }) != m_threads.end()) {
                auto promise = th->promise();

                if (promise)
                    promise->resolve();
            }

            resolved.push_back(broadcast);
            resolvedThreads.push_back(th);
        }
    }

    for (Broadcast *broadcast : resolved)
        m_broadcastSenders.erase(broadcast);

    m_redrawRequested = false;

    // Step threads
    stepThreads();

    // Render
    m_aboutToRedraw();
}

void Engine::run()
{
    start();
    eventLoop(true);
}

void Engine::runEventLoop()
{
    eventLoop();
}

void Engine::stopEventLoop()
{
    m_stopEventLoopMutex.lock();
    m_stopEventLoop = true;
    m_stopEventLoopMutex.unlock();
}

sigslot::signal<> &Engine::aboutToRender()
{
    return m_aboutToRedraw;
}

sigslot::signal<Thread *> &Engine::threadAboutToStop()
{
    return m_threadAboutToStop;
}

sigslot::signal<> &Engine::stopped()
{
    return m_stopped;
}

std::vector<std::shared_ptr<Thread>> Engine::stepThreads()
{
    // https://github.com/scratchfoundation/scratch-vm/blob/develop/src/engine/sequencer.js#L70-L173
    const double WORK_TIME = 0.75 * m_frameDuration.count(); // 75% of frame duration
    assert(WORK_TIME > 0);
    auto stepStart = m_clock->currentSteadyTime();

    size_t numActiveThreads = 1; // greater than zero
    std::vector<std::shared_ptr<Thread>> doneThreads;

    auto elapsedTime = [this, &stepStart]() {
        std::chrono::steady_clock::time_point currentTime = m_clock->currentSteadyTime();
        std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - stepStart);
        return elapsedTime.count();
    };

    while (!m_threads.empty() && numActiveThreads > 0 && elapsedTime() < WORK_TIME && (m_turboModeEnabled || !m_redrawRequested)) {
        numActiveThreads = 0;

        // Attempt to run each thread one time
        for (int i = 0; i < m_threads.size(); i++) {
            assert(i >= 0 && i < m_threads.size());
            m_activeThread = m_threads[i];

            // Check if the thread is done so it is not executed
            if (m_activeThread->isFinished()) {
                // Finished with this thread
                continue;
            }

            stepThread(m_activeThread);

            if (!m_activeThread->isFinished())
                numActiveThreads++;
        }

        // Remove threads in m_threadsToStop
        for (auto thread : m_threadsToStop) {
            if (!thread->isFinished())
                m_threadAboutToStop(thread.get());

            m_threads.erase(std::remove(m_threads.begin(), m_threads.end(), thread), m_threads.end());
        }

        m_threadsToStop.clear();

        // Remove inactive threads (and add them to doneThreads)
        m_threads.erase(
            std::remove_if(
                m_threads.begin(),
                m_threads.end(),
                [&doneThreads](std::shared_ptr<Thread> thread) {
                    if (thread->isFinished()) {
                        doneThreads.push_back(thread);
                        return true;
                    } else
                        return false;
                }),
            m_threads.end());
    }

    if (m_threads.empty())
        m_running = false;

    m_activeThread = nullptr;
    return doneThreads;
}

void Engine::stepThread(std::shared_ptr<Thread> thread)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/develop/src/engine/sequencer.js#L179-L276
    thread->run();
}

void Engine::eventLoop(bool untilProjectStops)
{
    m_stopEventLoop = false;

    while (true) {
        auto tickStart = m_clock->currentSteadyTime();
        m_eventLoopMutex.lock();
        step();
        updateMonitors();

        // Stop the event loop if the project has finished running (and untilProjectStops is set to true)
        if (untilProjectStops && m_threads.empty()) {
            m_eventLoopMutex.unlock();
            break;
        }

        // Stop the event loop if stopEventLoop() was called
        m_stopEventLoopMutex.lock();

        if (m_stopEventLoop) {
            m_stopEventLoopMutex.unlock();
            m_eventLoopMutex.unlock();
            break;
        }

        m_stopEventLoopMutex.unlock();

        std::chrono::steady_clock::time_point currentTime = m_clock->currentSteadyTime();
        std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - tickStart);
        std::chrono::milliseconds sleepTime = m_frameDuration - elapsedTime;
        m_eventLoopMutex.unlock();

        // If there's any time left, sleep
        if (sleepTime > std::chrono::milliseconds::zero())
            m_clock->sleep(sleepTime);
    }

    m_eventLoopMutex.lock();
    m_threads.clear();
    m_running = false;
    m_frameActivity = false;
    m_redrawRequested = false;
    m_eventLoopMutex.unlock();
}

bool Engine::isRunning() const
{
    return m_running || m_frameActivity;
}

double Engine::fps() const
{
    return m_fps;
}

void Engine::setFps(double fps)
{
    if (fps > 0) {
        m_fps = std::min(fps, 250.0);
        updateFrameDuration();
    }
}

bool Engine::turboModeEnabled() const
{
    return m_turboModeEnabled;
}

void Engine::setTurboModeEnabled(bool turboMode)
{
    m_turboModeEnabled = turboMode;
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
    setKeyState(event, pressed);
}

void Engine::setKeyState(const KeyEvent &event, bool pressed)
{
    m_keyMap[event.name()] = pressed;

    // Start "when key pressed" scripts
    if (pressed) {
        startHats(HatType::KeyPressed, { { HatField::KeyOption, event.name() } }, nullptr);
        startHats(HatType::KeyPressed, { { HatField::KeyOption, "any" } }, nullptr);
    }
}

void Engine::setAnyKeyPressed(bool pressed)
{
    m_anyKeyPressed = pressed;

    // Start "when key pressed" scripts
    if (pressed)
        startHats(HatType::KeyPressed, { { HatField::KeyOption, "any" } }, nullptr);
}

void Engine::mouseWheelUp()
{
    // Start "when up arrow pressed" scripts
    startHats(HatType::KeyPressed, { { HatField::KeyOption, "up arrow" } }, nullptr);
}

void Engine::mouseWheelDown()
{
    // Start "when down arrow pressed" scripts
    startHats(HatType::KeyPressed, { { HatField::KeyOption, "down arrow" } }, nullptr);
}

double Engine::mouseX() const
{
    return m_mouseX;
}

void Engine::setMouseX(double x)
{
    m_mouseX = std::round(x);
}

double Engine::mouseY() const
{
    return m_mouseY;
}

void Engine::setMouseY(double y)
{
    m_mouseY = std::round(y);
}

bool Engine::mousePressed() const
{
    return m_mousePressed;
}

void Engine::setMousePressed(bool pressed)
{
    m_mousePressed = pressed;
}

void Engine::clickTarget(Target *target)
{
    assert(target);

    if (target) {
        if (!target->isStage() && !static_cast<Sprite *>(target)->visible())
            return;

        startHats(HatType::TargetClicked, {}, target);
    }
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

int Engine::cloneLimit() const
{
    return m_cloneLimit;
}

void Engine::setCloneLimit(int limit)
{
    m_cloneLimit = limit < 0 ? -1 : limit;
}

int Engine::cloneCount() const
{
    return m_clones.size();
}

bool Engine::spriteFencingEnabled() const
{
    return m_spriteFencingEnabled;
}

void Engine::setSpriteFencingEnabled(bool enable)
{
    m_spriteFencingEnabled = enable;
}

void Engine::requestRedraw()
{
    m_redrawRequested = true;
}

ITimer *Engine::timer() const
{
    return m_timer;
}

void Engine::setTimer(ITimer *timer)
{
    m_timer = timer;
}

void Engine::addCompileFunction(IExtension *extension, const std::string &opcode, BlockComp f)
{
    if (m_compileFunctions.find(extension) == m_compileFunctions.cend())
        m_compileFunctions[extension] = {};

    m_compileFunctions[extension][opcode] = f;
}

void Engine::addHatPredicateCompileFunction(IExtension *extension, const std::string &opcode, HatPredicateCompileFunc f)
{
    if (m_hatPredicateCompileFunctions.find(extension) == m_hatPredicateCompileFunctions.cend())
        m_hatPredicateCompileFunctions[extension] = {};

    m_hatPredicateCompileFunctions[extension][opcode] = f;
}

void Engine::addMonitorNameFunction(IExtension *extension, const std::string &opcode, MonitorNameFunc f)
{
    if (m_monitorNameFunctions.find(extension) == m_monitorNameFunctions.cend())
        m_monitorNameFunctions[extension] = {};

    m_monitorNameFunctions[extension][opcode] = f;
}

void Engine::addMonitorChangeFunction(IExtension *extension, const std::string &opcode, MonitorChangeFunc f)
{
    if (m_monitorChangeFunctions.find(extension) == m_monitorChangeFunctions.cend())
        m_monitorChangeFunctions[extension] = {};

    m_monitorChangeFunctions[extension][opcode] = f;
}

void Engine::addHatBlock(IExtension *extension, const std::string &opcode)
{
    if (m_compileFunctions.find(extension) == m_compileFunctions.cend())
        m_compileFunctions[extension] = {};

    m_compileFunctions[extension][opcode] = [](Compiler *compiler) -> CompilerValue * { return nullptr; };
}

void Engine::addInput(IExtension *extension, const std::string &name, int id)
{
    if (m_inputs.find(extension) == m_inputs.cend())
        m_inputs[extension] = {};

    m_inputs[extension][name] = id;
}

void Engine::addField(IExtension *extension, const std::string &name, int id)
{
    if (m_fields.find(extension) == m_fields.cend())
        m_fields[extension] = {};

    m_fields[extension][name] = id;
}

void Engine::addFieldValue(IExtension *extension, const std::string &value, int id)
{
    if (m_fieldValues.find(extension) == m_fieldValues.cend())
        m_fieldValues[extension] = {};

    m_fieldValues[extension][value] = id;
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

std::vector<int> Engine::findBroadcasts(const std::string &broadcastName) const
{
    std::vector<int> ret;
    std::string lowercaseName = broadcastName;
    std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);

    for (unsigned int i = 0; i < m_broadcasts.size(); i++) {
        std::string name = m_broadcasts[i]->name();
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        if (name == lowercaseName)
            ret.push_back(i);
    }

    return ret;
}

int Engine::findBroadcastById(const std::string &broadcastId) const
{
    auto it = std::find_if(m_broadcasts.begin(), m_broadcasts.end(), [broadcastId](std::shared_ptr<Broadcast> broadcast) { return broadcast->id() == broadcastId; });

    if (it == m_broadcasts.end())
        return -1;
    else
        return it - m_broadcasts.begin();
}

void Engine::addWhenTouchingObjectScript(Block *hatBlock)
{
    addHatToMap(m_whenTouchingObjectHats, m_scripts[hatBlock].get());
}

void Engine::addGreenFlagScript(Block *hatBlock)
{
    std::cout << "passed block: " << hatBlock << std::endl;
    addHatToMap(m_greenFlagHats, m_scripts[hatBlock].get());
}

void Engine::addBroadcastScript(Block *whenReceivedBlock, Field *field, Broadcast *broadcast)
{
    assert(!broadcast->isBackdropBroadcast());
    Script *script = m_scripts[whenReceivedBlock].get();
    auto it = m_broadcastMap.find(broadcast);

    if (it != m_broadcastMap.cend()) {
        auto &scripts = it->second;
        auto scriptIt = std::find(scripts.begin(), scripts.end(), script);

        if (scriptIt == scripts.end())
            scripts.push_back(script);
    } else
        m_broadcastMap[broadcast] = { script };

    addHatToMap(m_broadcastHats, script);
    addHatField(script, HatField::BroadcastOption, field);
}

void Engine::addBackdropChangeScript(Block *hatBlock, Field *field)
{
    Stage *stage = this->stage();

    if (!stage)
        return;

    if (!field) {
        assert(false);
        return;
    }

    const std::string &backdropName = field->value().toString();
    auto backdrop = stage->costumeAt(stage->findCostume(backdropName));

    if (!backdrop)
        return;

    Broadcast *broadcast = backdrop->broadcast();
    assert(broadcast->isBackdropBroadcast());

    Script *script = m_scripts[hatBlock].get();
    auto it = m_backdropBroadcastMap.find(broadcast);

    if (it != m_backdropBroadcastMap.cend()) {
        auto &scripts = it->second;
        auto scriptIt = std::find(scripts.begin(), scripts.end(), script);

        if (scriptIt == scripts.end())
            scripts.push_back(script);
    } else
        m_backdropBroadcastMap[broadcast] = { script };

    addHatToMap(m_backdropChangeHats, script);
    addHatField(script, HatField::Backdrop, field);
}

void Engine::addCloneInitScript(Block *hatBlock)
{
    addHatToMap(m_cloneInitHats, m_scripts[hatBlock].get());
}

void Engine::addKeyPressScript(Block *hatBlock, Field *field)
{
    Script *script = m_scripts[hatBlock].get();
    addHatToMap(m_whenKeyPressedHats, script);
    addHatField(script, HatField::KeyOption, field);
}

void Engine::addTargetClickScript(Block *hatBlock)
{
    addHatToMap(m_whenTargetClickedHats, m_scripts[hatBlock].get());
}

void Engine::addWhenGreaterThanScript(Block *hatBlock)
{
    Script *script = m_scripts[hatBlock].get();
    addHatToMap(m_whenGreaterThanHats, script);
}

const std::vector<std::shared_ptr<Target>> &Engine::targets() const
{
    return m_targets;
}

void Engine::setTargets(const std::vector<std::shared_ptr<Target>> &newTargets)
{
    m_targets = newTargets;
    m_sortedDrawables.clear();

    for (auto target : m_targets) {
        m_sortedDrawables.push_back(target.get());

        // Set engine in the target
        target->setEngine(this);
        auto blocks = target->blocks();

        for (auto block : blocks) {
            // Set engine and target in the block
            block->setEngine(this);
            block->setTarget(target.get());
        }
    }

    // Sort the targets by layer order
    std::sort(m_sortedDrawables.begin(), m_sortedDrawables.end(), [](Drawable *d1, Drawable *d2) { return d1->layerOrder() < d2->layerOrder(); });

    // Add text bubbles (layers are irrelevant until text is displayed)
    for (auto target : m_targets) {
        target->bubble()->setLayerOrder(m_sortedDrawables.size());
        m_sortedDrawables.push_back(target->bubble());
    }
}

Target *Engine::targetAt(int index) const
{
    if (index < 0 || index >= m_targets.size())
        return nullptr;

    return m_targets[index].get();
}

void Engine::getVisibleTargets(std::vector<Target *> &dst) const
{
    dst.clear();

    for (auto target : m_targets) {
        assert(target);

        if (target->isStage())
            dst.push_back(target.get());
        else {
            Sprite *sprite = static_cast<Sprite *>(target.get());

            if (sprite->visible())
                dst.push_back(target.get());

            const auto &clones = sprite->clones();

            for (auto clone : clones) {
                if (clone->visible())
                    dst.push_back(clone.get());
            }
        }
    }

    std::sort(dst.begin(), dst.end(), [](Target *t1, Target *t2) { return t1->layerOrder() > t2->layerOrder(); });
}

int Engine::findTarget(const std::string &targetName) const
{
    auto it = std::find_if(m_targets.begin(), m_targets.end(), [targetName](std::shared_ptr<Target> target) {
        return ((target->isStage() && targetName == "_stage_") || (!target->isStage() && target->name() == targetName));
    });

    if (it == m_targets.end())
        return -1;
    else
        return it - m_targets.begin();
}

void Engine::moveDrawableToFront(Drawable *drawable)
{
    if (!drawable || m_sortedDrawables.size() <= 2)
        return;

    if (drawable->isTarget() && static_cast<Target *>(drawable)->isStage()) {
        std::cout << "warning: cannot move stage to front" << std::endl;
        assert(false);
        return;
    }

    auto it = std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), drawable);

    if (it != m_sortedDrawables.end()) {
        std::rotate(it, it + 1, m_sortedDrawables.end());
        updateDrawableLayerOrder();
    }
}

void Engine::moveDrawableToBack(Drawable *drawable)
{
    if (!drawable || m_sortedDrawables.size() <= 2)
        return;

    auto it = std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), drawable);

    if (it != m_sortedDrawables.end()) {
        std::rotate(m_sortedDrawables.begin() + 1, it, it + 1); // stage is always the first
        updateDrawableLayerOrder();
    }
}

void Engine::moveDrawableForwardLayers(Drawable *drawable, int layers)
{
    if (!drawable || layers == 0)
        return;

    if (layers > 0 && drawable->isTarget() && static_cast<Target *>(drawable)->isStage()) {
        std::cout << "warning: cannot move stage forward" << std::endl;
        assert(false);
        return;
    }

    auto it = std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), drawable);

    if (it == m_sortedDrawables.end())
        return;

    auto target = it + layers;

    if (target <= m_sortedDrawables.begin()) {
        moveDrawableToBack(drawable);
        return;
    }

    if (target >= m_sortedDrawables.end()) {
        moveDrawableToFront(drawable);
        return;
    }

    if (layers > 0)
        std::rotate(it, it + 1, target + 1);
    else
        std::rotate(target, it, it + 1);

    updateDrawableLayerOrder();
}

void Engine::moveDrawableBackwardLayers(Drawable *drawable, int layers)
{
    moveDrawableForwardLayers(drawable, -layers);
}

void Engine::moveDrawableBehindOther(Drawable *drawable, Drawable *other)
{
    if (drawable == other)
        return;

    if (drawable->isTarget() && static_cast<Target *>(drawable)->isStage()) {
        std::cout << "warning: cannot move stage behind drawable" << std::endl;
        assert(false);
        return;
    }

    auto itSprite = std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), drawable);
    auto itOther = std::find(m_sortedDrawables.begin(), m_sortedDrawables.end(), other);

    if ((itSprite == m_sortedDrawables.end()) || (itOther == m_sortedDrawables.end()))
        return;

    auto target = itOther - 1; // behind

    if (target < itSprite)
        target++;

    if (target <= m_sortedDrawables.begin()) {
        moveDrawableToBack(drawable);
        return;
    }

    if (target >= m_sortedDrawables.end()) {
        moveDrawableToFront(drawable);
        return;
    }

    if (target > itSprite)
        std::rotate(itSprite, itSprite + 1, target + 1);
    else
        std::rotate(target, itSprite, itSprite + 1);

    updateDrawableLayerOrder();
}

Stage *Engine::stage() const
{
    auto it = std::find_if(m_targets.begin(), m_targets.end(), [](std::shared_ptr<Target> target) { return target && target->isStage(); });

    if (it == m_targets.end())
        return nullptr;
    else
        return static_cast<Stage *>((*it).get());
}

const std::vector<std::shared_ptr<Monitor>> &Engine::monitors() const
{
    return m_monitors;
}

void Engine::setMonitors(const std::vector<std::shared_ptr<Monitor>> &newMonitors)
{
    m_monitors.clear();

    for (auto monitor : newMonitors) {
        m_monitors.push_back(monitor);
        m_monitorAdded(monitor.get());
    }
}

Monitor *Engine::createVariableMonitor(std::shared_ptr<Variable> var, const std::string &opcode, const std::string &varFieldName, int varFieldId, BlockComp compileFunction)
{
    if (var->monitor())
        return var->monitor();
    else {
        auto monitor = std::make_shared<Monitor>(var->id(), opcode);
        auto field = std::make_shared<Field>(varFieldName, var->name(), var);
        field->setFieldId(varFieldId);
        monitor->block()->addField(field);
        monitor->block()->setCompileFunction(compileFunction);

        addVarOrListMonitor(monitor, var->target());
        var->setMonitor(monitor.get());
        compileMonitor(monitor);
        return monitor.get();
    }
}

Monitor *Engine::createListMonitor(std::shared_ptr<List> list, const std::string &opcode, const std::string &listFieldName, int listFieldId, BlockComp compileFunction)
{
    if (list->monitor())
        return list->monitor();
    else {
        auto monitor = std::make_shared<Monitor>(list->id(), opcode);
        auto field = std::make_shared<Field>(listFieldName, list->name(), list);
        field->setFieldId(listFieldId);
        monitor->block()->addField(field);
        monitor->block()->setCompileFunction(compileFunction);
        monitor->setMode(Monitor::Mode::List);

        addVarOrListMonitor(monitor, list->target());
        list->setMonitor(monitor.get());
        compileMonitor(monitor);
        return monitor.get();
    }
}

sigslot::signal<Monitor *> &Engine::monitorAdded()
{
    return m_monitorAdded;
}

sigslot::signal<Monitor *, IMonitorHandler *> &Engine::monitorRemoved()
{
    return m_monitorRemoved;
}

sigslot::signal<const std::string &> &Engine::questionAsked()
{
    return m_questionAsked;
}

sigslot::signal<> &Engine::questionAborted()
{
    return m_questionAborted;
}

sigslot::signal<const std::string &> &Engine::questionAnswered()
{
    return m_questionAnswered;
}

const std::vector<std::string> &Engine::extensions() const
{
    return m_extensions;
}

void Engine::setExtensions(const std::vector<std::string> &newExtensions)
{
    clearExtensionData();
    m_extensions = newExtensions;

    // Register blocks of default extensions
    const auto &defaultExtensions = Blocks::extensions();

    for (auto ext : defaultExtensions) {
        ext->registerBlocks(this);
        ext->onInit(this);
    }

    // Register blocks of custom extensions
    for (auto ext : m_extensions) {
        IExtension *ptr = ScratchConfiguration::getExtension(ext);
        if (ptr) {
            ptr->registerBlocks(this);
            ptr->onInit(this);
        } else
            std::cerr << "Unsupported extension: " << ext << std::endl;
    }
}

const std::unordered_map<Block *, std::shared_ptr<Script>> &Engine::scripts() const
{
    return m_scripts;
}

// Returns the block with the given ID.
std::shared_ptr<Block> Engine::getBlock(const std::string &id, Target *target)
{
    if (id.empty())
        return nullptr;

    int index;

    if (target) {
        index = target->findBlock(id);

        if (index != -1)
            return target->blockAt(index);
    }

    for (auto t : m_targets) {
        index = t->findBlock(id);

        if (index != -1)
            return t->blockAt(index);
    }

    return nullptr;
}

// Returns the variable with the given ID.
std::shared_ptr<Variable> Engine::getVariable(const std::string &id, Target *target)
{
    if (id.empty())
        return nullptr;

    Stage *stage = this->stage();
    int index;

    // Check stage
    if (stage) {
        index = stage->findVariableById(id);

        if (index != -1)
            return stage->variableAt(index);
    }

    // Check currently compiled target
    if (target != stage) {
        index = target->findVariableById(id);

        if (index != -1)
            return target->variableAt(index);
    }

    // Fall back to checking all the other targets
    for (auto t : m_targets) {
        if (t.get() != stage && t.get() != target) {
            int index = t->findVariableById(id);

            if (index != -1)
                return t->variableAt(index);
        }
    }

    return nullptr;
}

// Returns the Scratch list with the given ID.
std::shared_ptr<List> Engine::getList(const std::string &id, Target *target)
{
    if (id.empty())
        return nullptr;

    Stage *stage = this->stage();
    int index;

    // Check stage
    if (stage) {
        index = stage->findListById(id);

        if (index != -1)
            return stage->listAt(index);
    }

    // Check currently compiled target
    if (target != stage) {
        index = target->findListById(id);

        if (index != -1)
            return target->listAt(index);
    }

    // Fall back to checking all the other targets
    for (auto t : m_targets) {
        if (t.get() != stage && t.get() != target) {
            int index = t->findListById(id);

            if (index != -1)
                return t->listAt(index);
        }
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

// Returns the comment with the given ID.
std::shared_ptr<Comment> Engine::getComment(const std::string &id, Target *target)
{
    if (id.empty())
        return nullptr;

    int index;

    if (target) {
        index = target->findComment(id);

        if (index != -1)
            return target->commentAt(index);
    }

    for (auto t : m_targets) {
        index = t->findComment(id);

        if (index != -1)
            return t->commentAt(index);
    }

    return nullptr;
}

// Returns the entity with the given ID. \see IEntity
std::shared_ptr<Entity> Engine::getEntity(const std::string &id, Target *target)
{
    // Variables
    auto variable = getVariable(id, target);
    if (variable)
        return std::static_pointer_cast<Entity>(variable);

    // Lists
    auto list = getList(id, target);
    if (list)
        return std::static_pointer_cast<Entity>(list);

    // Broadcasts
    auto broadcast = getBroadcast(id);
    if (broadcast)
        return std::static_pointer_cast<Entity>(broadcast);

    // Blocks
    auto block = getBlock(id, target);
    if (block)
        return std::static_pointer_cast<Entity>(block);

    // Comments
    auto comment = getComment(id, target);
    if (comment)
        return std::static_pointer_cast<Entity>(comment);

    return nullptr;
}

void Engine::addHatToMap(std::unordered_map<Target *, std::vector<Script *>> &map, Script *script)
{
    if (!script)
        return;

    assert(script->target());
    Target *target = script->target();
    auto it = map.find(target);

    if (it != map.cend()) {
        auto &scripts = it->second;
        auto scriptIt = std::find(scripts.begin(), scripts.end(), script);

        if (scriptIt == scripts.end())
            scripts.push_back(script);
    } else
        map[target] = { script };
}

void Engine::addHatField(Script *script, HatField hatField, Field *targetField)
{
    auto it = m_scriptHatFields.find(script);

    if (it == m_scriptHatFields.cend())
        m_scriptHatFields[script] = { { hatField, targetField } };
    else {
        auto &fieldMap = it->second;
        fieldMap[hatField] = targetField;
    }
}

const std::vector<Script *> &Engine::getHats(Target *target, HatType type)
{
    assert(target);

    // Get root if this is a clone
    if (!target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);

        if (sprite->isClone())
            target = sprite->cloneSprite();
    }

    switch (type) {
        case HatType::WhenTouchingObject:
            return m_whenTouchingObjectHats[target];

        case HatType::GreenFlag:
            return m_greenFlagHats[target];

        case HatType::BroadcastReceived:
            return m_broadcastHats[target];

        case HatType::BackdropChanged:
            return m_backdropChangeHats[target];

        case HatType::CloneInit:
            return m_cloneInitHats[target];

        case HatType::KeyPressed:
            return m_whenKeyPressedHats[target];

        case HatType::TargetClicked:
            return m_whenTargetClickedHats[target];

        case HatType::WhenGreaterThan:
            return m_whenGreaterThanHats[target];

        default: {
            static const std::vector<Script *> empty = {};
            return empty;
        }
    }
}

void Engine::updateDrawableLayerOrder()
{
    assert(m_sortedDrawables.empty() || (m_sortedDrawables[0]->isTarget() && static_cast<Target *>(m_sortedDrawables[0])->isStage()));

    for (size_t i = 1; i < m_sortedDrawables.size(); i++) // i = 1 to skip the stage
        m_sortedDrawables[i]->setLayerOrder(i);
}

const std::string &Engine::userAgent() const
{
    return m_userAgent;
}

void Engine::setUserAgent(const std::string &agent)
{
    m_userAgent = agent;
}

const std::unordered_set<std::string> &Engine::unsupportedBlocks() const
{
    return m_unsupportedBlocks;
}

void Engine::clearExtensionData()
{
    m_compileFunctions.clear();
    m_hatPredicateCompileFunctions.clear();
    m_monitorNameFunctions.clear();
    m_monitorChangeFunctions.clear();
    m_inputs.clear();
    m_fields.clear();
    m_fieldValues.clear();
}

IExtension *Engine::blockExtension(const std::string &opcode) const
{
    for (const auto &[ext, data] : m_compileFunctions) {
        auto it = data.find(opcode);

        if (it != data.cend())
            return ext;
    }

    return nullptr;
}

BlockComp Engine::resolveBlockCompileFunc(IExtension *extension, const std::string &opcode) const
{
    if (!extension)
        return nullptr;

    auto it = m_compileFunctions.find(extension);

    if (it != m_compileFunctions.cend()) {
        auto dataIt = it->second.find(opcode);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return nullptr;
}

HatPredicateCompileFunc Engine::resolveHatPredicateCompileFunc(IExtension *extension, const std::string &opcode) const
{
    if (!extension)
        return nullptr;

    auto it = m_hatPredicateCompileFunctions.find(extension);

    if (it != m_hatPredicateCompileFunctions.cend()) {
        auto dataIt = it->second.find(opcode);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return nullptr;
}

MonitorNameFunc Engine::resolveMonitorNameFunc(IExtension *extension, const std::string &opcode) const
{
    if (!extension)
        return nullptr;

    auto it = m_monitorNameFunctions.find(extension);

    if (it != m_monitorNameFunctions.cend()) {
        auto dataIt = it->second.find(opcode);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return nullptr;
}

MonitorChangeFunc Engine::resolveMonitorChangeFunc(IExtension *extension, const std::string &opcode) const
{
    if (!extension)
        return nullptr;

    auto it = m_monitorChangeFunctions.find(extension);

    if (it != m_monitorChangeFunctions.cend()) {
        auto dataIt = it->second.find(opcode);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return nullptr;
}

int Engine::resolveInput(IExtension *extension, const std::string &name) const
{
    if (!extension)
        return -1;

    auto it = m_inputs.find(extension);

    if (it != m_inputs.cend()) {
        auto dataIt = it->second.find(name);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return -1;
}

int Engine::resolveField(IExtension *extension, const std::string &name) const
{
    if (!extension)
        return -1;

    auto it = m_fields.find(extension);

    if (it != m_fields.cend()) {
        auto dataIt = it->second.find(name);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return -1;
}

int Engine::resolveFieldValue(IExtension *extension, const std::string &value) const
{
    if (!extension)
        return -1;

    auto it = m_fieldValues.find(extension);

    if (it != m_fieldValues.cend()) {
        auto dataIt = it->second.find(value);

        if (dataIt != it->second.cend())
            return dataIt->second;
    }

    return -1;
}

void Engine::compileMonitor(std::shared_ptr<Monitor> monitor)
{
    Target *target = monitor->sprite() ? static_cast<Target *>(monitor->sprite()) : stage();
    auto block = monitor->block();
    auto ext = blockExtension(block->opcode());

    if (ext) {
        auto ctx = Compiler::createContext(this, target);
        Compiler compiler(ctx.get());
        MonitorNameFunc nameFunc = resolveMonitorNameFunc(ext, block->opcode());

        if (nameFunc)
            monitor->setName(nameFunc(block.get()));

        MonitorChangeFunc changeFunc = resolveMonitorChangeFunc(ext, block->opcode());
        monitor->setValueChangeFunction(changeFunc);

        auto script = std::make_shared<Script>(target, block.get(), this);
        monitor->setScript(script);
        auto code = compiler.compile(block.get(), Compiler::CodeType::Reporter);
        script->setCode(code);
        m_monitorCompilerContexts[monitor.get()] = ctx;

        const auto &unsupportedBlocks = compiler.unsupportedBlocks();

        for (const std::string &opcode : unsupportedBlocks)
            m_unsupportedBlocks.insert(opcode);

        // Preoptimize to avoid lag when updating monitors for the first time
        compiler.preoptimize();
    } else {
        std::cout << "warning: unsupported monitor block: " << block->opcode() << std::endl;
        m_unsupportedBlocks.insert(block->opcode());
    }
}

void Engine::deleteClones()
{
    m_eventLoopMutex.lock();
    removeExecutableClones();
    m_clones.clear();

    for (auto target : m_targets) {
        if (!target->isStage()) {
            Sprite *sprite = static_cast<Sprite *>(target.get());
            std::vector<std::shared_ptr<Sprite>> clones = sprite->clones();

            for (auto clone : clones) {
                assert(clone);
                clone->deleteClone();
            }
        }
    }

    m_eventLoopMutex.unlock();
}

void Engine::removeExecutableClones()
{
    // Remove clones from sorted drawables
    for (std::shared_ptr<Sprite> clone : m_clones)
        m_sortedDrawables.erase(std::remove(m_sortedDrawables.begin(), m_sortedDrawables.end(), clone.get()), m_sortedDrawables.end());
}

void Engine::addVarOrListMonitor(std::shared_ptr<Monitor> monitor, Target *target)
{
    if (!target->isStage())
        monitor->setSprite(static_cast<Sprite *>(target));

    IExtension *ext = blockExtension(monitor->opcode());
    monitor->setExtension(ext);

    if (ext) {
        MonitorNameFunc nameFunc = resolveMonitorNameFunc(ext, monitor->opcode());

        if (nameFunc)
            monitor->setName(nameFunc(monitor->block().get()));

        MonitorChangeFunc changeFunc = resolveMonitorChangeFunc(ext, monitor->opcode());
        monitor->setValueChangeFunction(changeFunc);
    }

    m_monitors.push_back(monitor);
    m_monitorAdded(monitor.get());

    monitor->setVisible(false);
}

void Engine::updateFrameDuration()
{
    m_frameDuration = std::chrono::milliseconds(static_cast<long>(1000 / m_fps));
}

void Engine::addRunningScript(std::shared_ptr<Thread> thread)
{
    m_threads.push_back(thread);
}

void Engine::addBroadcastPromise(Broadcast *broadcast, Thread *sender, bool wait)
{
    assert(broadcast);
    assert(sender);

    // Resolve broadcast promise if it's already running
    auto it = m_broadcastSenders.find(broadcast);

    if (it != m_broadcastSenders.cend() && std::find_if(m_threads.begin(), m_threads.end(), [&it](std::shared_ptr<Thread> thread) { return thread.get() == it->second; }) != m_threads.end()) {
        auto promise = it->second->promise();

        if (promise)
            promise->resolve();
    }

    if (wait)
        m_broadcastSenders[broadcast] = sender;
}

std::shared_ptr<Thread> Engine::pushThread(Block *block, Target *target)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L1649-L1661
    if (!block) {
        std::cerr << "error: tried to start a script with a null block" << std::endl;
        assert(false);
        return nullptr;
    }

    if (!target) {
        std::cerr << "error: scripts must be started by a target" << std::endl;
        assert(false);
        return nullptr;
    }

    auto script = m_scripts[block];
    std::shared_ptr<Thread> thread = script->start(target);
    addRunningScript(thread);
    return thread;
}

void Engine::stopThread(Thread *thread)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L1667-L1672
    assert(thread);

    if (!thread->isFinished())
        m_threadAboutToStop(thread);

    thread->kill();
}

std::shared_ptr<Thread> Engine::restartThread(std::shared_ptr<Thread> thread)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L1681C30-L1694
    std::shared_ptr<Thread> newThread = thread->script()->start(thread->target());
    auto it = std::find(m_threads.begin(), m_threads.end(), thread);

    if (it != m_threads.end()) {
        if (!thread->isFinished())
            m_threadAboutToStop(thread.get());

        auto i = it - m_threads.begin();
        m_threads[i] = newThread;
        return newThread;
    }

    addRunningScript(thread);
    return thread;
}

template<typename F>
void Engine::allScriptsByOpcodeDo(HatType hatType, F &&f, Target *optTarget)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L1797-L1809
    std::vector<Drawable *> *drawablesPtr = &m_sortedDrawables;

    if (optTarget)
        drawablesPtr = new std::vector<Drawable *>({ optTarget });

    const std::vector<Drawable *> &drawables = *drawablesPtr;

    for (int t = drawables.size() - 1; t >= 0; t--) {
        Drawable *drawable = drawables[t];

        if (drawable->isTarget()) {
            Target *target = static_cast<Target *>(drawable);
            const auto &scripts = getHats(target, hatType);

            for (size_t j = 0; j < scripts.size(); j++)
                f(scripts[j], target);
        }
    }

    if (optTarget)
        delete drawablesPtr;
}

std::vector<std::shared_ptr<Thread>> Engine::startHats(HatType hatType, const std::unordered_map<HatField, std::variant<std::string, const char *, Entity *>> &optMatchFields, Target *optTarget)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/f1aa92fad79af17d9dd1c41eeeadca099339a9f1/src/engine/runtime.js#L1818-L1889
    std::vector<std::shared_ptr<Thread>> newThreads;

    allScriptsByOpcodeDo(
        hatType,
        [this, hatType, &optMatchFields, &newThreads](Script *script, Target *target) {
            if (!optMatchFields.empty()) {
                // Get the field map for this script
                auto fieldMapIt = m_scriptHatFields.find(script);
                assert(fieldMapIt != m_scriptHatFields.cend());

                if (fieldMapIt != m_scriptHatFields.cend()) {
                    const auto &fieldMap = fieldMapIt->second;

                    // Match any requested fields
                    for (const auto &[fieldId, fieldValue] : optMatchFields) {
                        auto fieldIt = fieldMap.find(fieldId);
                        assert(fieldIt != fieldMap.cend());

                        if (fieldIt != fieldMap.cend()) {
                            assert(fieldIt->second);

                            if (std::holds_alternative<std::string>(fieldValue)) {
                                if (fieldIt->second->value().toString() != std::get<std::string>(fieldValue)) {
                                    // Field mismatch
                                    return;
                                }
                            } else if (std::holds_alternative<const char *>(fieldValue)) {
                                if (fieldIt->second->value().toString() != std::string(std::get<const char *>(fieldValue))) {
                                    // Field mismatch
                                    return;
                                }
                            } else {
                                if (fieldIt->second->valuePtr().get() != std::get<Entity *>(fieldValue)) {
                                    // Field mismatch
                                    return;
                                }
                            }
                        }
                    }
                }
            }

            if (m_hatRestartExistingThreads.at(hatType)) {
                // Restart existing threads
                for (auto thread : m_threads) {
                    if (thread->target() == target && thread->script() == script) {
                        newThreads.push_back(restartThread(thread));
                        return;
                    }
                }
            } else {
                // Give up if any threads with the top block are running
                for (auto thread : m_threads) {
                    if (thread->target() == target && thread->script() == script && !thread->isFinished()) {
                        // Some thread is already running
                        return;
                    }
                }
            }

            // Start the thread with this top block
            newThreads.push_back(pushThread(script->topBlock(), target));
        },
        optTarget);

    return newThreads;
}
