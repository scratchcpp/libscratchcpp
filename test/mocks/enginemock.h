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

        MOCK_METHOD(void, frame, (), (override));
        MOCK_METHOD(void, start, (), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(void, startScript, (std::shared_ptr<Block>, std::shared_ptr<Target>), (override));
        MOCK_METHOD(void, broadcast, (unsigned int, VirtualMachine *, bool), (override));
        MOCK_METHOD(void, stopScript, (VirtualMachine *), (override));
        MOCK_METHOD(void, stopTarget, (Target *, VirtualMachine *), (override));
        MOCK_METHOD(void, run, (), (override));

        MOCK_METHOD(bool, broadcastRunning, (unsigned int, VirtualMachine *), (override));

        MOCK_METHOD(void, breakFrame, (), (override));
        MOCK_METHOD(bool, breakingCurrentFrame, (), (override));

        MOCK_METHOD(void, skipFrame, (), (override));
        MOCK_METHOD(void, lockFrame, (), (override));

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

        MOCK_METHOD(void, addBroadcastScript, (std::shared_ptr<Block>, std::shared_ptr<Broadcast>), (override));

        MOCK_METHOD(const std::vector<std::shared_ptr<Target>> &, targets, (), (const, override));
        MOCK_METHOD(void, setTargets, (const std::vector<std::shared_ptr<Target>> &), (override));
        MOCK_METHOD(Target *, targetAt, (int), (const, override));
        MOCK_METHOD(int, findTarget, (const std::string &), (const, override));

        MOCK_METHOD(std::vector<std::string> &, extensions, (), (const, override));
        MOCK_METHOD(void, setExtensions, (const std::vector<std::string> &), (override));

        MOCK_METHOD(const ScriptMap &, scripts, (), (const, override));
};

} // namespace libscratchcpp