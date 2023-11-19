// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iengine.h>
#include <scratchcpp/target.h>
#include <scratchcpp/itimer.h>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "blocksectioncontainer.h"

namespace libscratchcpp
{

class Entity;
class IClock;

class Engine : public IEngine
{
    public:
        Engine();
        Engine(const Engine &) = delete;
        ~Engine();

        void clear() override;
        void resolveIds();
        void compile() override;

        void start() override;
        void stop() override;
        void startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target) override;
        void broadcast(unsigned int index, VirtualMachine *sourceScript, bool wait = false) override;
        void broadcastByPtr(Broadcast *broadcast, VirtualMachine *sourceScript, bool wait = false) override;
        void stopScript(VirtualMachine *vm) override;
        void stopTarget(Target *target, VirtualMachine *exceptScript) override;
        void initClone(libscratchcpp::Sprite *clone) override;
        void deinitClone(libscratchcpp::Sprite *clone) override;
        void run() override;
        void runEventLoop() override;

        bool isRunning() const override;

        double fps() const override;
        void setFps(double fps) override;

        bool turboModeEnabled() const override;
        void setTurboModeEnabled(bool turboMode) override;

        bool keyPressed(const std::string &name) const override;
        void setKeyState(const std::string &name, bool pressed) override;
        void setAnyKeyPressed(bool pressed) override;

        double mouseX() const override;
        void setMouseX(double x) override;

        double mouseY() const override;
        void setMouseY(double y) override;

        bool mousePressed() const override;
        void setMousePressed(bool pressed) override;

        unsigned int stageWidth() const override;
        void setStageWidth(unsigned int width) override;

        unsigned int stageHeight() const override;
        void setStageHeight(unsigned int height) override;

        int cloneLimit() const override;
        void setCloneLimit(int limit) override;

        int cloneCount() const override;

        bool spriteFencingEnabled() const override;
        void setSpriteFencingEnabled(bool enable) override;

        bool broadcastRunning(unsigned int index, VirtualMachine *sourceScript) override;
        bool broadcastByPtrRunning(Broadcast *broadcast, VirtualMachine *sourceScript) override;

        void requestRedraw() override;

        ITimer *timer() const override;
        void setTimer(ITimer *timer);

        void registerSection(std::shared_ptr<IBlockSection> section) override;
        std::vector<std::shared_ptr<IBlockSection>> registeredSections() const;
        unsigned int functionIndex(BlockFunc f) override;

        void addCompileFunction(IBlockSection *section, const std::string &opcode, BlockComp f) override;
        void addHatBlock(IBlockSection *section, const std::string &opcode) override;
        void addInput(IBlockSection *section, const std::string &name, int id) override;
        void addField(IBlockSection *section, const std::string &name, int id) override;
        void addFieldValue(IBlockSection *section, const std::string &value, int id) override;

        const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const override;
        void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts) override;
        std::shared_ptr<Broadcast> broadcastAt(int index) const override;
        int findBroadcast(const std::string &broadcastName) const override;
        int findBroadcastById(const std::string &broadcastId) const override;

        void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, Broadcast *broadcast) override;
        void addCloneInitScript(std::shared_ptr<Block> hatBlock) override;
        void addKeyPressScript(std::shared_ptr<Block> hatBlock, std::string keyName) override;

        const std::vector<std::shared_ptr<Target>> &targets() const override;
        void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets) override;
        Target *targetAt(int index) const override;
        int findTarget(const std::string &targetName) const override;

        void moveSpriteToFront(Sprite *sprite) override;
        void moveSpriteToBack(Sprite *sprite) override;
        void moveSpriteForwardLayers(Sprite *sprite, int layers) override;
        void moveSpriteBackwardLayers(Sprite *sprite, int layers) override;
        void moveSpriteBehindOther(Sprite *sprite, Sprite *other) override;

        Stage *stage() const override;

        const std::vector<std::string> &extensions() const override;
        void setExtensions(const std::vector<std::string> &newExtensions) override;

        const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const override;

        BlockSectionContainer *blockSectionContainer(const std::string &opcode) const;
        BlockSectionContainer *blockSectionContainer(IBlockSection *section) const;

        IClock *m_clock = nullptr;

    private:
        using TargetScriptMap = std::unordered_map<Target *, std::vector<std::shared_ptr<VirtualMachine>>>;

        void eventLoop(bool untilProjectStops = false);
        void runScripts(const TargetScriptMap &scriptMap, TargetScriptMap &globalScriptMap);
        void finalize();
        void deleteClones();
        void removeExecutableClones();
        std::shared_ptr<Block> getBlock(const std::string &id);
        std::shared_ptr<Variable> getVariable(const std::string &id);
        std::shared_ptr<List> getList(const std::string &id);
        std::shared_ptr<Broadcast> getBroadcast(const std::string &id);
        std::shared_ptr<Entity> getEntity(const std::string &id);
        std::shared_ptr<IBlockSection> blockSection(const std::string &opcode) const;

        void updateSpriteLayerOrder();

        void updateFrameDuration();
        void addRunningScript(std::shared_ptr<VirtualMachine> vm);
        void startWhenKeyPressedScripts(const std::vector<Script *> &scripts);

        std::unordered_map<std::shared_ptr<IBlockSection>, std::unique_ptr<BlockSectionContainer>> m_sections;
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::unordered_map<Broadcast *, std::vector<Script *>> m_broadcastMap;
        std::unordered_map<Broadcast *, std::vector<std::pair<VirtualMachine *, VirtualMachine *>>> m_runningBroadcastMap; // source script, "when received" script
        std::unordered_map<Target *, std::vector<Script *>> m_cloneInitScriptsMap;                                         // target (no clones), "when I start as a clone" scripts
        std::unordered_map<std::string, std::vector<Script *>> m_whenKeyPressedScripts;                                    // key name, "when key pressed" scripts
        std::vector<std::string> m_extensions;
        std::vector<Target *> m_executableTargets; // sorted by layer (reverse order of execution)
        std::unordered_map<Target *, std::vector<std::shared_ptr<VirtualMachine>>> m_runningScripts;
        std::unordered_map<Target *, std::vector<std::shared_ptr<VirtualMachine>>> m_newScripts;
        std::vector<VirtualMachine *> m_scriptsToRemove;
        std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> m_scripts;
        std::vector<BlockFunc> m_functions;

        std::unique_ptr<ITimer> m_defaultTimer;
        ITimer *m_timer = nullptr;
        double m_fps = 30;                         // default FPS
        std::chrono::milliseconds m_frameDuration; // will be computed in eventLoop()
        bool m_turboModeEnabled = false;
        std::unordered_map<std::string, bool> m_keyMap; // holds key states
        bool m_anyKeyPressed = false;
        double m_mouseX = 0;
        double m_mouseY = 0;
        bool m_mousePressed = false;
        unsigned int m_stageWidth = 480;
        unsigned int m_stageHeight = 360;
        int m_cloneLimit = 300;
        std::vector<Sprite *> m_clones;
        bool m_spriteFencingEnabled = true;

        bool m_running = false;
        bool m_redrawRequested = false;
};

} // namespace libscratchcpp
