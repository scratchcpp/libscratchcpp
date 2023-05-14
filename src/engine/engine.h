// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include "../scratch/broadcast.h"
#include "../scratch/target.h"
#include "global.h"
#include "virtualmachine.h"
#include <unordered_map>
#include <memory>
#include <chrono>

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT IBlockSection;

/*!
 * \brief The Engine class provides an API for running Scratch projects.
 *
 * This class is can be used to load Scratch projects using custom methods.\n
 * If you want to load a project from a standard Scratch project file, use the ScratchProject class.
 */
class LIBSCRATCHCPP_EXPORT Engine
{
    public:
        Engine();
        Engine(const Engine &) = delete;

        void clear();
        void compile();

        void frame();
        void start();
        void stop();
        void startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target);
        void broadcast(unsigned int index);
        void stopScript(VirtualMachine *vm);
        void stopTarget(Target *target, VirtualMachine *exceptScript);
        void run();

        void breakFrame();
        void stayOnCurrentBlock();
        void breakAtomicScript();
        bool isAtomic();

        void registerSection(std::shared_ptr<IBlockSection> section);
        unsigned int functionIndex(BlockFunc f);
        std::shared_ptr<IBlockSection> blockSection(const std::string &opcode) const;

        std::vector<std::shared_ptr<Broadcast>> broadcasts() const;
        void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts);
        std::shared_ptr<Broadcast> broadcastAt(int index) const;
        int findBroadcast(const std::string &broadcastName) const;
        int findBroadcastById(const std::string &broadcastId) const;

        void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, std::shared_ptr<Broadcast> broadcast);

        std::vector<std::shared_ptr<Target>> targets() const;
        void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets);
        Target *targetAt(int index) const;
        int findTarget(const std::string &targetName) const;

        std::vector<std::string> extensions() const;
        void setExtensions(const std::vector<std::string> &newExtensions);

    private:
        std::shared_ptr<Block> getBlock(std::string id);
        std::shared_ptr<Variable> getVariable(std::string id);
        std::shared_ptr<List> getList(std::string id);
        std::shared_ptr<Broadcast> getBroadcast(std::string id);
        std::shared_ptr<IEntity> getEntity(std::string id);
        std::vector<std::shared_ptr<IBlockSection>> m_sections;
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::unordered_map<unsigned int, std::vector<VirtualMachine *>> m_broadcastMap;
        std::vector<std::string> m_extensions;
        std::vector<VirtualMachine *> m_runningScripts;
        std::vector<unsigned int *> m_scriptPositions;
        std::vector<VirtualMachine *> m_scriptsToRemove;
        std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<VirtualMachine>> m_scripts;
        std::vector<BlockFunc> m_functions;

        bool m_breakFrame = false;
        bool m_stayOnCurrentBlock = false;
        bool m_atomic = true;
        int scriptCount = 0;
};

} // namespace libscratchcpp
