// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iengine.h>
#include <scratchcpp/target.h>
#include <scratchcpp/itimer.h>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <set>
#include <variant>

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
        VirtualMachine *startScript(std::shared_ptr<Block> topLevelBlock, Target *target) override;
        void broadcast(unsigned int index) override;
        void broadcastByPtr(Broadcast *broadcast) override;
        void startBackdropScripts(Broadcast *broadcast) override;
        void stopScript(VirtualMachine *vm) override;
        void stopTarget(Target *target, VirtualMachine *exceptScript) override;
        void initClone(std::shared_ptr<Sprite> clone) override;
        void deinitClone(std::shared_ptr<Sprite> clone) override;
        void stopSounds() override;

        void updateMonitors() override;
        void step() override;
        void run() override;
        void runEventLoop() override;
        void stopEventLoop() override;

        void setRedrawHandler(const std::function<void()> &handler) override;

        bool isRunning() const override;

        double fps() const override;
        void setFps(double fps) override;

        bool turboModeEnabled() const override;
        void setTurboModeEnabled(bool turboMode) override;

        bool keyPressed(const std::string &name) const override;
        void setKeyState(const std::string &name, bool pressed) override;
        void setKeyState(const KeyEvent &event, bool pressed) override;
        void setAnyKeyPressed(bool pressed) override;

        void mouseWheelUp() override;
        void mouseWheelDown() override;

        double mouseX() const override;
        void setMouseX(double x) override;

        double mouseY() const override;
        void setMouseY(double y) override;

        bool mousePressed() const override;
        void setMousePressed(bool pressed) override;

        void clickTarget(Target *target) override;

        unsigned int stageWidth() const override;
        void setStageWidth(unsigned int width) override;

        unsigned int stageHeight() const override;
        void setStageHeight(unsigned int height) override;

        int cloneLimit() const override;
        void setCloneLimit(int limit) override;

        int cloneCount() const override;

        bool spriteFencingEnabled() const override;
        void setSpriteFencingEnabled(bool enable) override;

        bool broadcastRunning(unsigned int index) override;
        bool broadcastByPtrRunning(Broadcast *broadcast) override;

        void requestRedraw() override;

        ITimer *timer() const override;
        void setTimer(ITimer *timer);

        void registerSection(std::shared_ptr<IBlockSection> section) override;
        std::vector<std::shared_ptr<IBlockSection>> registeredSections() const;
        unsigned int functionIndex(BlockFunc f) override;

        void addCompileFunction(IBlockSection *section, const std::string &opcode, BlockComp f) override;
        void addMonitorNameFunction(IBlockSection *section, const std::string &opcode, MonitorNameFunc f) override;
        void addMonitorChangeFunction(IBlockSection *section, const std::string &opcode, MonitorChangeFunc f) override;
        void addHatBlock(IBlockSection *section, const std::string &opcode) override;
        void addInput(IBlockSection *section, const std::string &name, int id) override;
        void addField(IBlockSection *section, const std::string &name, int id) override;
        void addFieldValue(IBlockSection *section, const std::string &value, int id) override;

        const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const override;
        void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts) override;
        std::shared_ptr<Broadcast> broadcastAt(int index) const override;
        int findBroadcast(const std::string &broadcastName) const override;
        int findBroadcastById(const std::string &broadcastId) const override;

        void addGreenFlagScript(std::shared_ptr<Block> hatBlock) override;
        void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, int fieldId, Broadcast *broadcast) override;
        void addBackdropChangeScript(std::shared_ptr<Block> hatBlock, int fieldId) override;
        void addCloneInitScript(std::shared_ptr<Block> hatBlock) override;
        void addKeyPressScript(std::shared_ptr<Block> hatBlock, int fieldId) override;
        void addTargetClickScript(std::shared_ptr<Block> hatBlock) override;

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

        const std::vector<std::shared_ptr<Monitor>> &monitors() const override;
        void setMonitors(const std::vector<std::shared_ptr<Monitor>> &newMonitors) override;
        void setAddMonitorHandler(const std::function<void(Monitor *)> &handler) override;
        void setRemoveMonitorHandler(const std::function<void(Monitor *, IMonitorHandler *)> &handler) override;

        const std::function<void(const std::string &)> &questionAsked() const override;
        void setQuestionAsked(const std::function<void(const std::string &)> &f) override;

        const std::function<void(const std::string &)> &questionAnswered() const override;
        void setQuestionAnswered(const std::function<void(const std::string &)> &f) override;

        const std::vector<std::string> &extensions() const override;
        void setExtensions(const std::vector<std::string> &newExtensions) override;

        const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const override;

        BlockSectionContainer *blockSectionContainer(const std::string &opcode) const;
        BlockSectionContainer *blockSectionContainer(IBlockSection *section) const;

        IClock *m_clock = nullptr;

    private:
        enum class HatType
        {
            GreenFlag,
            BroadcastReceived,
            BackdropChanged,
            CloneInit,
            KeyPressed,
            TargetClicked
        };

        enum class HatField
        {
            BroadcastOption,
            Backdrop,
            KeyOption
        };

        std::vector<std::shared_ptr<VirtualMachine>> stepThreads();
        void stepThread(std::shared_ptr<VirtualMachine> thread);
        void eventLoop(bool untilProjectStops = false);
        void finalize();
        void deleteClones();
        void removeExecutableClones();
        void createMissingMonitors();
        void addVarOrListMonitor(std::shared_ptr<Monitor> monitor, Target *target);
        std::shared_ptr<Block> getBlock(const std::string &id);
        std::shared_ptr<Variable> getVariable(const std::string &id);
        std::shared_ptr<List> getList(const std::string &id);
        std::shared_ptr<Broadcast> getBroadcast(const std::string &id);
        std::shared_ptr<Comment> getComment(const std::string &id);
        std::shared_ptr<Entity> getEntity(const std::string &id);
        std::shared_ptr<IBlockSection> blockSection(const std::string &opcode) const;

        void addHatToMap(std::unordered_map<Target *, std::vector<Script *>> &map, Script *script);
        void addHatField(Script *script, HatField field, int fieldId);
        const std::vector<libscratchcpp::Script *> &getHats(Target *target, HatType type);

        void updateSpriteLayerOrder();

        void updateFrameDuration();
        void addRunningScript(std::shared_ptr<VirtualMachine> vm);

        std::shared_ptr<VirtualMachine> pushThread(std::shared_ptr<Block> block, Target *target);
        void stopThread(VirtualMachine *thread);
        std::shared_ptr<VirtualMachine> restartThread(std::shared_ptr<VirtualMachine> thread);

        template<typename F>
        void allScriptsByOpcodeDo(HatType hatType, F &&f, Target *optTarget);

        std::vector<std::shared_ptr<VirtualMachine>>
        startHats(HatType hatType, const std::unordered_map<HatField, std::variant<std::string, const char *, Entity *>> &optMatchFields, Target *optTarget);

        static const std::unordered_map<HatType, bool> m_hatRestartExistingThreads; // used to check whether a hat should restart existing threads

        std::unordered_map<std::shared_ptr<IBlockSection>, std::unique_ptr<BlockSectionContainer>> m_sections;
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::unordered_map<Broadcast *, std::vector<Script *>> m_broadcastMap;
        std::vector<std::shared_ptr<Monitor>> m_monitors;
        std::vector<std::string> m_extensions;
        std::vector<Target *> m_executableTargets; // sorted by layer (reverse order of execution)
        std::vector<std::shared_ptr<VirtualMachine>> m_threads;
        std::vector<std::shared_ptr<VirtualMachine>> m_threadsToStop;
        std::shared_ptr<VirtualMachine> m_activeThread;
        std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> m_scripts;
        std::vector<BlockFunc> m_functions;
        std::recursive_mutex m_eventLoopMutex;

        std::unordered_map<Target *, std::vector<Script *>> m_greenFlagHats;
        std::unordered_map<Target *, std::vector<Script *>> m_backdropChangeHats;
        std::unordered_map<Target *, std::vector<Script *>> m_broadcastHats;
        std::unordered_map<Target *, std::vector<Script *>> m_cloneInitHats;
        std::unordered_map<Target *, std::vector<Script *>> m_whenKeyPressedHats;
        std::unordered_map<Target *, std::vector<Script *>> m_whenTargetClickedHats;

        std::unordered_map<Script *, std::unordered_map<HatField, int>> m_scriptHatFields; // HatField, field ID from the block implementation

        std::unique_ptr<ITimer> m_defaultTimer;
        ITimer *m_timer = nullptr;
        double m_fps = 30;                         // default FPS
        std::chrono::milliseconds m_frameDuration; // will be computed in step()
        bool m_turboModeEnabled = false;
        std::unordered_map<std::string, bool> m_keyMap; // holds key states
        bool m_anyKeyPressed = false;
        double m_mouseX = 0;
        double m_mouseY = 0;
        bool m_mousePressed = false;
        unsigned int m_stageWidth = 480;
        unsigned int m_stageHeight = 360;
        int m_cloneLimit = 300;
        std::set<std::shared_ptr<Sprite>> m_clones;
        bool m_spriteFencingEnabled = true;

        bool m_running = false;
        bool m_redrawRequested = false;
        std::function<void()> m_redrawHandler = nullptr;
        bool m_stopEventLoop = false;
        std::mutex m_stopEventLoopMutex;

        std::function<void(Monitor *)> m_addMonitorHandler = nullptr;
        std::function<void(Monitor *, IMonitorHandler *)> m_removeMonitorHandler = nullptr;
        std::function<void(const std::string &)> m_questionAsked = nullptr;
        std::function<void(const std::string &)> m_questionAnswered = nullptr;
};

} // namespace libscratchcpp
