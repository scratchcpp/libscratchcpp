#pragma once

#include <scratchcpp/iengine.h>
#include <gmock/gmock.h>

namespace libscratchcpp
{

using ScriptMap = std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>>;

class EngineMock : public IEngine
{
    public:
        MOCK_METHOD(void, clear, (), (override));
        MOCK_METHOD(void, compile, (), (override));

        MOCK_METHOD(void, start, (), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(void, startScript, (std::shared_ptr<Block>, std::shared_ptr<Target>), (override));
        MOCK_METHOD(void, broadcast, (unsigned int, VirtualMachine *, bool), (override));
        MOCK_METHOD(void, broadcastByPtr, (Broadcast *, VirtualMachine *, bool), (override));
        MOCK_METHOD(void, stopScript, (VirtualMachine *), (override));
        MOCK_METHOD(void, stopTarget, (Target *, VirtualMachine *), (override));
        MOCK_METHOD(void, initClone, (Sprite *), (override));
        MOCK_METHOD(void, deinitClone, (Sprite *), (override));
        MOCK_METHOD(void, run, (), (override));
        MOCK_METHOD(void, runEventLoop, (), (override));

        MOCK_METHOD(void, setRedrawHandler, (const std::function<void()> &), (override));

        MOCK_METHOD(bool, isRunning, (), (const, override));

        MOCK_METHOD(double, fps, (), (const, override));
        MOCK_METHOD(void, setFps, (double fps), (override));

        MOCK_METHOD(bool, turboModeEnabled, (), (const, override));
        MOCK_METHOD(void, setTurboModeEnabled, (bool), (override));

        MOCK_METHOD(bool, keyPressed, (const std::string &), (const, override));
        MOCK_METHOD(void, setKeyState, (const std::string &, bool), (override));
        MOCK_METHOD(void, setAnyKeyPressed, (bool), (override));

        MOCK_METHOD(double, mouseX, (), (const, override));
        MOCK_METHOD(void, setMouseX, (double x), (override));

        MOCK_METHOD(double, mouseY, (), (const, override));
        MOCK_METHOD(void, setMouseY, (double y), (override));

        MOCK_METHOD(bool, mousePressed, (), (const, override));
        MOCK_METHOD(void, setMousePressed, (bool), (override));

        MOCK_METHOD(unsigned int, stageWidth, (), (const, override));
        MOCK_METHOD(void, setStageWidth, (unsigned int), (override));

        MOCK_METHOD(unsigned int, stageHeight, (), (const, override));
        MOCK_METHOD(void, setStageHeight, (unsigned int), (override));

        MOCK_METHOD(int, cloneLimit, (), (const, override));
        MOCK_METHOD(void, setCloneLimit, (int), (override));

        MOCK_METHOD(int, cloneCount, (), (const, override));

        MOCK_METHOD(bool, spriteFencingEnabled, (), (const, override));
        MOCK_METHOD(void, setSpriteFencingEnabled, (bool), (override));

        MOCK_METHOD(bool, broadcastRunning, (unsigned int, VirtualMachine *), (override));
        MOCK_METHOD(bool, broadcastByPtrRunning, (Broadcast *, VirtualMachine *), (override));

        MOCK_METHOD(void, requestRedraw, (), (override));

        MOCK_METHOD(ITimer *, timer, (), (const, override));

        MOCK_METHOD(void, registerSection, (std::shared_ptr<IBlockSection>), (override));
        MOCK_METHOD(unsigned int, functionIndex, (BlockFunc), (override));

        MOCK_METHOD(void, addCompileFunction, (IBlockSection *, const std::string &, BlockComp), (override));
        MOCK_METHOD(void, addHatBlock, (IBlockSection *, const std::string &), (override));
        MOCK_METHOD(void, addInput, (IBlockSection *, const std::string &, int), (override));
        MOCK_METHOD(void, addField, (IBlockSection *, const std::string &, int), (override));
        MOCK_METHOD(void, addFieldValue, (IBlockSection *, const std::string &, int), (override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Broadcast>> &, broadcasts, (), (const, override));
        MOCK_METHOD(void, setBroadcasts, (const std::vector<std::shared_ptr<Broadcast>> &), (override));
        MOCK_METHOD(std::shared_ptr<Broadcast>, broadcastAt, (int), (const, override));
        MOCK_METHOD(int, findBroadcast, (const std::string &), (const, override));
        MOCK_METHOD(int, findBroadcastById, (const std::string &), (const, override));

        MOCK_METHOD(void, addBroadcastScript, (std::shared_ptr<Block>, Broadcast *), (override));
        MOCK_METHOD(void, addCloneInitScript, (std::shared_ptr<Block>), (override));
        MOCK_METHOD(void, addKeyPressScript, (std::shared_ptr<Block>, std::string), (override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Target>> &, targets, (), (const, override));
        MOCK_METHOD(void, setTargets, (const std::vector<std::shared_ptr<Target>> &), (override));
        MOCK_METHOD(Target *, targetAt, (int), (const, override));
        MOCK_METHOD(int, findTarget, (const std::string &), (const, override));

        MOCK_METHOD(void, moveSpriteToFront, (Sprite * sprite), (override));
        MOCK_METHOD(void, moveSpriteToBack, (Sprite * sprite), (override));
        MOCK_METHOD(void, moveSpriteForwardLayers, (Sprite * sprite, int layers), (override));
        MOCK_METHOD(void, moveSpriteBackwardLayers, (Sprite * sprite, int layers), (override));
        MOCK_METHOD(void, moveSpriteBehindOther, (Sprite * sprite, Sprite *other), (override));

        MOCK_METHOD(Stage *, stage, (), (const, override));

        MOCK_METHOD(std::vector<std::string> &, extensions, (), (const, override));
        MOCK_METHOD(void, setExtensions, (const std::vector<std::string> &), (override));

        MOCK_METHOD(const ScriptMap &, scripts, (), (const, override));
};

} // namespace libscratchcpp
