#pragma once

#include <scratchcpp/iengine.h>
#include <gmock/gmock.h>

namespace libscratchcpp
{

using ScriptMap = std::unordered_map<Block *, std::shared_ptr<Script>>;

class EngineMock : public IEngine
{
    public:
        MOCK_METHOD(void, clear, (), (override));
        MOCK_METHOD(void, compile, (), (override));

        MOCK_METHOD(void, start, (), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(Thread *, startScript, (Block *, Target *), (override));
        MOCK_METHOD(void, broadcast, (int, Thread *, bool), (override));
        MOCK_METHOD(void, broadcastByPtr, (Broadcast *, Thread *, bool), (override));
        MOCK_METHOD(void, startBackdropScripts, (Broadcast *, Thread *, bool), (override));
        MOCK_METHOD(void, stopScript, (Thread *), (override));
        MOCK_METHOD(void, stopTarget, (Target *, Thread *), (override));
        MOCK_METHOD(void, initClone, (std::shared_ptr<Sprite>), (override));
        MOCK_METHOD(void, deinitClone, (std::shared_ptr<Sprite>), (override));

        MOCK_METHOD(void, stopSounds, (), (override));
        MOCK_METHOD(double, globalVolume, (), (const, override));
        MOCK_METHOD(void, setGlobalVolume, (double), (override));

        MOCK_METHOD(void, updateMonitors, (), (override));
        MOCK_METHOD(void, step, (), (override));
        MOCK_METHOD(void, run, (), (override));
        MOCK_METHOD(void, runEventLoop, (), (override));
        MOCK_METHOD(void, stopEventLoop, (), (override));

        MOCK_METHOD(sigslot::signal<> &, aboutToRender, (), (override));
        MOCK_METHOD(sigslot::signal<Thread *> &, threadAboutToStop, (), (override));
        MOCK_METHOD(sigslot::signal<> &, stopped, (), (override));

        MOCK_METHOD(bool, isRunning, (), (const, override));

        MOCK_METHOD(double, fps, (), (const, override));
        MOCK_METHOD(void, setFps, (double fps), (override));

        MOCK_METHOD(bool, turboModeEnabled, (), (const, override));
        MOCK_METHOD(void, setTurboModeEnabled, (bool), (override));

        MOCK_METHOD(bool, keyPressed, (const std::string &), (const, override));
        MOCK_METHOD(void, setKeyState, (const std::string &, bool), (override));
        MOCK_METHOD(void, setKeyState, (const KeyEvent &, bool), (override));
        MOCK_METHOD(void, setAnyKeyPressed, (bool), (override));

        MOCK_METHOD(void, mouseWheelUp, (), (override));
        MOCK_METHOD(void, mouseWheelDown, (), (override));

        MOCK_METHOD(double, mouseX, (), (const, override));
        MOCK_METHOD(void, setMouseX, (double x), (override));

        MOCK_METHOD(double, mouseY, (), (const, override));
        MOCK_METHOD(void, setMouseY, (double y), (override));

        MOCK_METHOD(bool, mousePressed, (), (const, override));
        MOCK_METHOD(void, setMousePressed, (bool), (override));

        MOCK_METHOD(void, clickTarget, (Target * target), (override));

        MOCK_METHOD(unsigned int, stageWidth, (), (const, override));
        MOCK_METHOD(void, setStageWidth, (unsigned int), (override));

        MOCK_METHOD(unsigned int, stageHeight, (), (const, override));
        MOCK_METHOD(void, setStageHeight, (unsigned int), (override));

        MOCK_METHOD(int, cloneLimit, (), (const, override));
        MOCK_METHOD(void, setCloneLimit, (int), (override));

        MOCK_METHOD(int, cloneCount, (), (const, override));

        MOCK_METHOD(bool, spriteFencingEnabled, (), (const, override));
        MOCK_METHOD(void, setSpriteFencingEnabled, (bool), (override));

        MOCK_METHOD(void, requestRedraw, (), (override));

        MOCK_METHOD(ITimer *, timer, (), (const, override));

        MOCK_METHOD(void, addCompileFunction, (IExtension *, const std::string &, BlockComp), (override));
        MOCK_METHOD(void, addHatPredicateCompileFunction, (IExtension *, const std::string &, HatPredicateCompileFunc), (override));
        MOCK_METHOD(void, addMonitorNameFunction, (IExtension *, const std::string &, MonitorNameFunc), (override));
        MOCK_METHOD(void, addMonitorChangeFunction, (IExtension *, const std::string &, MonitorChangeFunc), (override));
        MOCK_METHOD(void, addHatBlock, (IExtension *, const std::string &), (override));
        MOCK_METHOD(void, addInput, (IExtension *, const std::string &, int), (override));
        MOCK_METHOD(void, addField, (IExtension *, const std::string &, int), (override));
        MOCK_METHOD(void, addFieldValue, (IExtension *, const std::string &, int), (override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Broadcast>> &, broadcasts, (), (const, override));
        MOCK_METHOD(void, setBroadcasts, (const std::vector<std::shared_ptr<Broadcast>> &), (override));
        MOCK_METHOD(std::shared_ptr<Broadcast>, broadcastAt, (int), (const, override));
        MOCK_METHOD(std::vector<int>, findBroadcasts, (const std::string &), (const, override));
        MOCK_METHOD(int, findBroadcastById, (const std::string &), (const, override));

        MOCK_METHOD(void, addWhenTouchingObjectScript, (Block *), (override));
        MOCK_METHOD(void, addGreenFlagScript, (Block *), (override));
        MOCK_METHOD(void, addBroadcastScript, (Block *, Field *, Broadcast *), (override));
        MOCK_METHOD(void, addBackdropChangeScript, (Block *, Field *), (override));
        MOCK_METHOD(void, addCloneInitScript, (Block *), (override));
        MOCK_METHOD(void, addKeyPressScript, (Block *, Field *), (override));
        MOCK_METHOD(void, addTargetClickScript, (Block *), (override));
        MOCK_METHOD(void, addWhenGreaterThanScript, (Block *), (override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Target>> &, targets, (), (const, override));
        MOCK_METHOD(void, setTargets, (const std::vector<std::shared_ptr<Target>> &), (override));
        MOCK_METHOD(Target *, targetAt, (int), (const, override));
        MOCK_METHOD(void, getVisibleTargets, (std::vector<Target *> &), (const, override));
        MOCK_METHOD(int, findTarget, (const std::string &), (const, override));

        MOCK_METHOD(void, moveDrawableToFront, (Drawable *), (override));
        MOCK_METHOD(void, moveDrawableToBack, (Drawable *), (override));
        MOCK_METHOD(void, moveDrawableForwardLayers, (Drawable *, int), (override));
        MOCK_METHOD(void, moveDrawableBackwardLayers, (Drawable *, int), (override));
        MOCK_METHOD(void, moveDrawableBehindOther, (Drawable *, Drawable *), (override));

        MOCK_METHOD(Stage *, stage, (), (const, override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Monitor>> &, monitors, (), (const, override));
        MOCK_METHOD(void, setMonitors, (const std::vector<std::shared_ptr<Monitor>> &), (override));
        MOCK_METHOD(Monitor *, createVariableMonitor, (std::shared_ptr<Variable>, const std::string &, const std::string &), (override));
        MOCK_METHOD(Monitor *, createListMonitor, (std::shared_ptr<List>, const std::string &, const std::string &), (override));
        MOCK_METHOD(sigslot::signal<Monitor *> &, monitorAdded, (), (override));
        MOCK_METHOD((sigslot::signal<Monitor *, IMonitorHandler *> &), monitorRemoved, (), (override));

        MOCK_METHOD(sigslot::signal<const std::string &> &, questionAsked, (), (override));
        MOCK_METHOD(sigslot::signal<> &, questionAborted, (), (override));
        MOCK_METHOD(sigslot::signal<const std::string &> &, questionAnswered, (), (override));

        MOCK_METHOD(std::vector<std::string> &, extensions, (), (const, override));
        MOCK_METHOD(void, setExtensions, (const std::vector<std::string> &), (override));

        MOCK_METHOD(const ScriptMap &, scripts, (), (const, override));

        MOCK_METHOD(const std::string &, userAgent, (), (const, override));
        MOCK_METHOD(void, setUserAgent, (const std::string &), (override));

        MOCK_METHOD(const std::unordered_set<std::string> &, unsupportedBlocks, (), (const, override));
};

} // namespace libscratchcpp
