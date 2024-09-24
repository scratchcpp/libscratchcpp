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

namespace libscratchcpp
{

class Entity;
class IClock;
class IAudioEngine;
class Thread;

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
        Thread *startScript(std::shared_ptr<Block> topLevelBlock, Target *target) override;
        void broadcast(int index, Thread *sender, bool wait) override;
        void broadcastByPtr(Broadcast *broadcast, Thread *sender, bool wait) override;
        void startBackdropScripts(Broadcast *broadcast, Thread *sender, bool wait) override;
        void stopScript(Thread *vm) override;
        void stopTarget(Target *target, Thread *exceptScript) override;
        void initClone(std::shared_ptr<Sprite> clone) override;
        void deinitClone(std::shared_ptr<Sprite> clone) override;

        void stopSounds() override;
        virtual double globalVolume() const override;
        virtual void setGlobalVolume(double volume) override;

        void updateMonitors() override;
        void step() override;
        void run() override;
        void runEventLoop() override;
        void stopEventLoop() override;

        sigslot::signal<> &aboutToRender() override;
        sigslot::signal<Thread *> &threadAboutToStop() override;
        sigslot::signal<> &stopped() override;

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

        void requestRedraw() override;

        ITimer *timer() const override;
        void setTimer(ITimer *timer);

        unsigned int functionIndex(BlockFunc f) override;
        const std::vector<BlockFunc> &blockFunctions() const override;

        void addCompileFunction(IExtension *extension, const std::string &opcode, BlockComp f) override;
        void addHatPredicateCompileFunction(IExtension *extension, const std::string &opcode, HatPredicateCompileFunc f) override;
        void addMonitorNameFunction(IExtension *extension, const std::string &opcode, MonitorNameFunc f) override;
        void addMonitorChangeFunction(IExtension *extension, const std::string &opcode, MonitorChangeFunc f) override;
        void addHatBlock(IExtension *extension, const std::string &opcode) override;
        void addInput(IExtension *extension, const std::string &name, int id) override;
        void addField(IExtension *extension, const std::string &name, int id) override;
        void addFieldValue(IExtension *extension, const std::string &value, int id) override;

        const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const override;
        void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts) override;
        std::shared_ptr<Broadcast> broadcastAt(int index) const override;
        std::vector<int> findBroadcasts(const std::string &broadcastName) const override;
        int findBroadcastById(const std::string &broadcastId) const override;

        void addWhenTouchingObjectScript(std::shared_ptr<Block> hatBlock) override;
        void addGreenFlagScript(std::shared_ptr<Block> hatBlock) override;
        void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, Field *field, Broadcast *broadcast) override;
        void addBackdropChangeScript(std::shared_ptr<Block> hatBlock, Field *field) override;
        void addCloneInitScript(std::shared_ptr<Block> hatBlock) override;
        void addKeyPressScript(std::shared_ptr<Block> hatBlock, Field *field) override;
        void addTargetClickScript(std::shared_ptr<Block> hatBlock) override;
        void addWhenGreaterThanScript(std::shared_ptr<Block> hatBlock) override;

        const std::vector<std::shared_ptr<Target>> &targets() const override;
        void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets) override;
        Target *targetAt(int index) const override;
        void getVisibleTargets(std::vector<Target *> &dst) const override;
        int findTarget(const std::string &targetName) const override;

        void moveSpriteToFront(Sprite *sprite) override;
        void moveSpriteToBack(Sprite *sprite) override;
        void moveSpriteForwardLayers(Sprite *sprite, int layers) override;
        void moveSpriteBackwardLayers(Sprite *sprite, int layers) override;
        void moveSpriteBehindOther(Sprite *sprite, Sprite *other) override;

        Stage *stage() const override;

        const std::vector<std::shared_ptr<Monitor>> &monitors() const override;
        void setMonitors(const std::vector<std::shared_ptr<Monitor>> &newMonitors) override;
        Monitor *createVariableMonitor(std::shared_ptr<Variable> var, const std::string &opcode, const std::string &varFieldName, int varFieldId, BlockComp compileFunction) override;
        Monitor *createListMonitor(std::shared_ptr<List> list, const std::string &opcode, const std::string &listFieldName, int listFieldId, BlockComp compileFunction) override;
        sigslot::signal<Monitor *> &monitorAdded() override;
        sigslot::signal<Monitor *, IMonitorHandler *> &monitorRemoved() override;

        sigslot::signal<const std::string &> &questionAsked() override;
        sigslot::signal<> &questionAborted() override;
        sigslot::signal<const std::string &> &questionAnswered() override;

        const std::vector<std::string> &extensions() const override;
        void setExtensions(const std::vector<std::string> &newExtensions) override;

        const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const override;

        const std::string &userAgent() const override;
        void setUserAgent(const std::string &agent) override;

        const std::unordered_set<std::string> &unsupportedBlocks() const override;

        IClock *m_clock = nullptr;
        IAudioEngine *m_audioEngine = nullptr;

    private:
        enum class HatType
        {
            WhenTouchingObject,
            GreenFlag,
            BroadcastReceived,
            BackdropChanged,
            CloneInit,
            KeyPressed,
            TargetClicked,
            WhenGreaterThan
        };

        enum class HatField
        {
            BroadcastOption,
            Backdrop,
            KeyOption,
            WhenGreaterThanMenu
        };

        void clearExtensionData();
        IExtension *blockExtension(const std::string &opcode) const;
        BlockComp resolveBlockCompileFunc(IExtension *extension, const std::string &opcode) const;
        HatPredicateCompileFunc resolveHatPredicateCompileFunc(IExtension *extension, const std::string &opcode) const;
        MonitorNameFunc resolveMonitorNameFunc(IExtension *extension, const std::string &opcode) const;
        MonitorChangeFunc resolveMonitorChangeFunc(IExtension *extension, const std::string &opcode) const;
        int resolveInput(IExtension *extension, const std::string &name) const;
        int resolveField(IExtension *extension, const std::string &name) const;
        int resolveFieldValue(IExtension *extension, const std::string &value) const;

        void compileMonitor(std::shared_ptr<Monitor> monitor);

        std::vector<std::shared_ptr<Thread>> stepThreads();
        void stepThread(std::shared_ptr<Thread> thread);
        void eventLoop(bool untilProjectStops = false);
        void finalize();
        void deleteClones();
        void removeExecutableClones();
        void addVarOrListMonitor(std::shared_ptr<Monitor> monitor, Target *target);
        std::shared_ptr<Block> getBlock(const std::string &id, Target *target);
        std::shared_ptr<Variable> getVariable(const std::string &id, Target *target);
        std::shared_ptr<List> getList(const std::string &id, Target *target);
        std::shared_ptr<Broadcast> getBroadcast(const std::string &id);
        std::shared_ptr<Comment> getComment(const std::string &id, Target *target);
        std::shared_ptr<Entity> getEntity(const std::string &id, Target *target);

        void addHatToMap(std::unordered_map<Target *, std::vector<Script *>> &map, Script *script);
        void addHatField(Script *script, HatField hatField, Field *targetField);
        const std::vector<libscratchcpp::Script *> &getHats(Target *target, HatType type);

        void updateDrawableLayerOrder();

        void updateFrameDuration();
        void addRunningScript(std::shared_ptr<Thread> thread);

        void addBroadcastPromise(Broadcast *broadcast, Thread *sender, bool wait);

        std::shared_ptr<Thread> pushThread(std::shared_ptr<Block> block, Target *target);
        void stopThread(Thread *thread);
        std::shared_ptr<Thread> restartThread(std::shared_ptr<Thread> thread);

        template<typename F>
        void allScriptsByOpcodeDo(HatType hatType, F &&f, Target *optTarget);

        std::vector<std::shared_ptr<Thread>> startHats(HatType hatType, const std::unordered_map<HatField, std::variant<std::string, const char *, Entity *>> &optMatchFields, Target *optTarget);

        static const std::unordered_map<HatType, bool> m_hatRestartExistingThreads; // used to check whether a hat should restart existing threads
        static const std::unordered_map<HatType, bool> m_hatEdgeActivated;          // used to check whether a hat is edge-activated (runs when a predicate becomes true)

        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::unordered_map<Broadcast *, std::vector<Script *>> m_broadcastMap;
        std::unordered_map<Broadcast *, std::vector<Script *>> m_backdropBroadcastMap;
        std::unordered_map<Broadcast *, Thread *> m_broadcastSenders; // used for resolving broadcast promises
        std::vector<std::shared_ptr<Monitor>> m_monitors;
        std::vector<std::string> m_extensions;
        std::vector<Drawable *> m_sortedDrawables; // sorted by layer (reverse order of execution)
        std::vector<std::shared_ptr<Thread>> m_threads;
        std::vector<std::shared_ptr<Thread>> m_threadsToStop;
        std::shared_ptr<Thread> m_activeThread;
        std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> m_scripts;
        std::vector<BlockFunc> m_functions;
        std::recursive_mutex m_eventLoopMutex;
        std::string m_userAgent;

        std::unordered_map<IExtension *, std::unordered_map<std::string, BlockComp>> m_compileFunctions;
        std::unordered_map<IExtension *, std::unordered_map<std::string, HatPredicateCompileFunc>> m_hatPredicateCompileFunctions;
        std::unordered_map<IExtension *, std::unordered_map<std::string, MonitorNameFunc>> m_monitorNameFunctions;
        std::unordered_map<IExtension *, std::unordered_map<std::string, MonitorChangeFunc>> m_monitorChangeFunctions;
        std::unordered_map<IExtension *, std::unordered_map<std::string, int>> m_inputs;
        std::unordered_map<IExtension *, std::unordered_map<std::string, int>> m_fields;
        std::unordered_map<IExtension *, std::unordered_map<std::string, int>> m_fieldValues;

        std::unordered_map<Target *, std::vector<Script *>> m_whenTouchingObjectHats;
        std::unordered_map<Target *, std::vector<Script *>> m_greenFlagHats;
        std::unordered_map<Target *, std::vector<Script *>> m_backdropChangeHats;
        std::unordered_map<Target *, std::vector<Script *>> m_broadcastHats;
        std::unordered_map<Target *, std::vector<Script *>> m_cloneInitHats;
        std::unordered_map<Target *, std::vector<Script *>> m_whenKeyPressedHats;
        std::unordered_map<Target *, std::vector<Script *>> m_whenTargetClickedHats;
        std::unordered_map<Target *, std::vector<Script *>> m_whenGreaterThanHats;

        std::unordered_map<Script *, std::unordered_map<HatField, Field *>> m_scriptHatFields; // HatField, field from the block implementation

        std::unordered_map<Block *, std::unordered_map<Target *, bool>> m_edgeActivatedHatValues; // (block, target, last value) edge-activated hats only run after the value changes from false to true

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
        bool m_frameActivity = false;
        bool m_redrawRequested = false;
        sigslot::signal<> m_aboutToRedraw;
        sigslot::signal<Thread *> m_threadAboutToStop;
        sigslot::signal<> m_stopped;
        bool m_stopEventLoop = false;
        std::mutex m_stopEventLoopMutex;

        sigslot::signal<Monitor *> m_monitorAdded;
        sigslot::signal<Monitor *, IMonitorHandler *> m_monitorRemoved;
        sigslot::signal<const std::string &> m_questionAsked;
        sigslot::signal<> m_questionAborted;
        sigslot::signal<const std::string &> m_questionAnswered;

        std::unordered_set<std::string> m_unsupportedBlocks;
};

} // namespace libscratchcpp
