// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iengine.h>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "../libscratchcpp_global.h"
#include "../scratch/broadcast.h"
#include "../scratch/target.h"
#include "global.h"
#include "virtualmachine.h"
#include "blocksectioncontainer.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT Script;

class LIBSCRATCHCPP_EXPORT Engine : public IEngine
{
    public:
        Engine();
        Engine(const Engine &) = delete;

        void clear() override;
        void resolveIds();
        void compile() override;

        void frame() override;
        void start() override;
        void stop() override;
        void startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target);
        void broadcast(unsigned int index, VirtualMachine *sourceScript);
        void stopScript(VirtualMachine *vm);
        void stopTarget(Target *target, VirtualMachine *exceptScript);
        void run() override;

        bool broadcastRunning(unsigned int index) override;

        void breakFrame() override;
        bool breakingCurrentFrame() override;

        void registerSection(std::shared_ptr<IBlockSection> section) override;
        unsigned int functionIndex(BlockFunc f);

        void addCompileFunction(IBlockSection *section, const std::string &opcode, BlockComp f) override;
        void addInput(IBlockSection *section, const std::string &name, int id) override;
        void addField(IBlockSection *section, const std::string &name, int id) override;
        void addFieldValue(IBlockSection *section, const std::string &value, int id) override;
        void addHatBlock(IBlockSection *section, const std::string &opcode) override;

        const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const;
        void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts);
        std::shared_ptr<Broadcast> broadcastAt(int index) const;
        int findBroadcast(const std::string &broadcastName) const;
        int findBroadcastById(const std::string &broadcastId) const;

        void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, std::shared_ptr<Broadcast> broadcast);

        const std::vector<std::shared_ptr<Target>> &targets() const;
        void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets);
        Target *targetAt(int index) const;
        int findTarget(const std::string &targetName) const;

        const std::vector<std::string> &extensions() const override;
        void setExtensions(const std::vector<std::string> &newExtensions) override;

        const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const;

    private:
        std::shared_ptr<Block> getBlock(const std::string &id);
        std::shared_ptr<Variable> getVariable(const std::string &id);
        std::shared_ptr<List> getList(const std::string &id);
        std::shared_ptr<Broadcast> getBroadcast(const std::string &id);
        std::shared_ptr<IEntity> getEntity(const std::string &id);
        std::shared_ptr<IBlockSection> blockSection(const std::string &opcode) const;
        BlockSectionContainer *blockSectionContainer(const std::string &opcode) const;
        BlockSectionContainer *blockSectionContainer(IBlockSection *section) const;

        std::unordered_map<std::shared_ptr<IBlockSection>, std::unique_ptr<BlockSectionContainer>> m_sections;
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::unordered_map<unsigned int, std::vector<Script *>> m_broadcastMap;
        std::vector<std::string> m_extensions;
        std::vector<std::shared_ptr<VirtualMachine>> m_runningScripts;
        std::vector<VirtualMachine *> m_scriptsToRemove;
        std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> m_scripts;
        std::vector<BlockFunc> m_functions;

        bool m_breakFrame = false;
};

} // namespace libscratchcpp
