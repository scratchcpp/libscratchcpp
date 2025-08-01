#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/keyevent.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/field.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratch/sound_p.h>
#include <timermock.h>
#include <clockmock.h>
#include <audioenginemock.h>
#include <audioinputmock.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>
#include <audioloudnessmock.h>
#include <monitorhandlermock.h>
#include <extensionmock.h>
#include <thread>

#include "../common.h"
#include "engine/internal/engine.h"
#include "engine/internal/clock.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::SaveArg;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

// NOTE: resolveIds() and compile() are tested in load_project_test

class RedrawMock
{
    public:
        MOCK_METHOD(void, redraw, ());
};

class StopMock
{
    public:
        MOCK_METHOD(void, threadRemoved, (Thread *));
        MOCK_METHOD(void, stopped, ());
};

class AddRemoveMonitorMock
{
    public:
        MOCK_METHOD(void, monitorAdded, (Monitor *));
        MOCK_METHOD(void, monitorRemoved, (Monitor *, IMonitorHandler *));
};

TEST(EngineTest, Clock)
{
    Engine engine;
    ASSERT_EQ(engine.m_clock, Clock::instance().get());
}

TEST(EngineTest, Clear)
{
    Engine engine;

    auto target1 = std::make_shared<Sprite>();
    auto target2 = std::make_shared<Sprite>();
    engine.setTargets({ target1, target2 });

    auto broadcast1 = std::make_shared<Broadcast>("", "");
    auto broadcast2 = std::make_shared<Broadcast>("", "");
    engine.setBroadcasts({ broadcast1, broadcast2 });

    auto extension = std::make_shared<ExtensionMock>();
    EXPECT_CALL(*extension, name()).WillRepeatedly(Return("ClearTest"));
    EXPECT_CALL(*extension, registerBlocks);
    EXPECT_CALL(*extension, onInit);
    ScratchConfiguration::registerExtension(extension);
    engine.setExtensions({ "ClearTest" });

    auto monitor1 = std::make_shared<Monitor>("", "");
    auto monitor2 = std::make_shared<Monitor>("", "");
    auto monitor3 = std::make_shared<Monitor>("", "");
    auto monitor4 = std::make_shared<Monitor>("", "");

    MonitorHandlerMock iface1, iface3, iface4;
    EXPECT_CALL(iface1, init);
    EXPECT_CALL(iface3, init);
    EXPECT_CALL(iface4, init);
    monitor1->setInterface(&iface1);
    monitor3->setInterface(&iface3);
    monitor4->setInterface(&iface4);

    engine.clear();
    ASSERT_TRUE(engine.targets().empty());
    ASSERT_TRUE(engine.broadcasts().empty());
    ASSERT_TRUE(engine.monitors().empty());
    ASSERT_TRUE(engine.extensions().empty());

    AddRemoveMonitorMock removeMonitorMock;
    auto handler = std::bind(&AddRemoveMonitorMock::monitorRemoved, &removeMonitorMock, std::placeholders::_1, std::placeholders::_2);
    engine.monitorRemoved().connect(handler);
    engine.setMonitors({ monitor1, monitor2, monitor3, monitor4 });

    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor1.get(), &iface1));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor2.get(), nullptr));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor3.get(), &iface3));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor4.get(), &iface4));
    engine.clear();
    ASSERT_TRUE(engine.monitors().empty());

    ScratchConfiguration::removeExtension(extension);
}

TEST(EngineTest, ClearThreadAboutToStopSignal)
{
    Project p("3_threads.sb3");
    ASSERT_TRUE(p.load());
    auto engine = p.engine();

    engine->start();
    engine->step();

    StopMock stopMock;
    EXPECT_CALL(stopMock, threadRemoved(_)).Times(3).WillRepeatedly(WithArgs<0>(Invoke([](Thread *thread) {
        ASSERT_TRUE(thread);
        ASSERT_FALSE(thread->isFinished());
    })));

    engine->threadAboutToStop().connect(&StopMock::threadRemoved, &stopMock);
    engine->clear();
}

TEST(EngineTest, StopThreadAboutToStopSignal)
{
    Project p("3_threads.sb3");
    ASSERT_TRUE(p.load());
    auto engine = p.engine();

    engine->start();
    engine->step();

    StopMock stopMock;
    EXPECT_CALL(stopMock, threadRemoved(_)).Times(3).WillRepeatedly(WithArgs<0>(Invoke([](Thread *vm) {
        ASSERT_TRUE(vm);
        ASSERT_FALSE(vm->isFinished());
    })));

    engine->threadAboutToStop().connect(&StopMock::threadRemoved, &stopMock);
    engine->stop();
}

TEST(EngineTest, StopSignal)
{
    StopMock stopMock;

    {
        Project p("3_threads.sb3");
        ASSERT_TRUE(p.load());
        auto engine = p.engine();

        engine->stopped().connect(&StopMock::stopped, &stopMock);
        EXPECT_CALL(stopMock, stopped());
        engine->start();
        engine->step();

        EXPECT_CALL(stopMock, stopped());
        engine->stop();
    }

    {
        Project p("stop_all_bypass.sb3");
        ASSERT_TRUE(p.load());
        auto engine = p.engine();

        engine->stopped().connect(&StopMock::stopped, &stopMock);
        EXPECT_CALL(stopMock, stopped());
        engine->start();
        EXPECT_CALL(stopMock, stopped());
        engine->step();

        EXPECT_CALL(stopMock, stopped());
        engine->stop();
    }
}

TEST(EngineTest, CompileAndExecuteMonitors)
{
    Engine engine;
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    engine.setTargets({ stage, sprite });

    auto m1 = std::make_shared<Monitor>("a", "monitor_test1");
    auto m2 = std::make_shared<Monitor>("b", "monitor_test2");
    auto m3 = std::make_shared<Monitor>("c", "monitor_test3");
    m1->setVisible(false);
    m2->setSprite(sprite.get());
    engine.setMonitors({ m1, m2 });

    auto extension = std::make_shared<ExtensionMock>();
    EXPECT_CALL(*extension, name()).WillRepeatedly(Return("MonitorTest"));
    EXPECT_CALL(*extension, registerBlocks);
    EXPECT_CALL(*extension, onInit);
    ScratchConfiguration::registerExtension(extension);
    engine.setExtensions({ "MonitorTest" });
    engine.addCompileFunction(extension.get(), m1->opcode(), [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(5.4); });
    engine.addCompileFunction(extension.get(), m2->opcode(), [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue("test"); });

    engine.addMonitorNameFunction(extension.get(), m1->opcode(), [](Block *block) -> const std::string & {
        static const std::string testStr = "test";
        return testStr;
    });

    engine.addMonitorNameFunction(extension.get(), m2->opcode(), [](Block *block) -> const std::string & { return block->opcode(); });

    engine.addMonitorChangeFunction(extension.get(), m1->opcode(), [](Block *block, const Value &newValue) { std::cout << "change 1!" << std::endl; });
    engine.addMonitorChangeFunction(extension.get(), m2->opcode(), [](Block *block, const Value &newValue) { std::cout << "change 2!" << std::endl; });

    // Compile the monitor blocks
    engine.compile();
    auto script1 = m1->script();
    auto script2 = m2->script();
    auto script3 = m3->script();
    ASSERT_TRUE(script1 && script2 && !script3);

    ASSERT_EQ(script1->target(), stage.get());
    ASSERT_EQ(script1->topBlock(), m1->block().get());

    ASSERT_EQ(script2->target(), sprite.get());
    ASSERT_EQ(script2->topBlock(), m2->block().get());

    ASSERT_EQ(m1->extension(), extension.get());
    ASSERT_EQ(m2->extension(), extension.get());
    ASSERT_FALSE(m3->extension());

    ASSERT_EQ(m1->name(), "test");
    ASSERT_EQ(m2->name(), m2->opcode());
    ASSERT_TRUE(m3->name().empty());

    // Execute the monitor blocks
    MonitorHandlerMock iface1, iface2, iface3;
    EXPECT_CALL(iface1, init);
    EXPECT_CALL(iface2, init);
    EXPECT_CALL(iface3, init);
    m1->setInterface(&iface1);
    m2->setInterface(&iface2);
    m3->setInterface(&iface3);

    EXPECT_CALL(iface1, onValueChanged).Times(0);
    EXPECT_CALL(iface2, onValueChanged(_)).WillOnce(WithArgs<0>(Invoke([](const Value &value) { ASSERT_EQ(value.toString(), "test"); })));
    EXPECT_CALL(iface3, onValueChanged).Times(0);
    engine.updateMonitors();

    // Change the monitor values
    testing::internal::CaptureStdout();
    EXPECT_CALL(iface1, onValueChanged);
    m1->changeValue(0);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "change 1!\n");

    testing::internal::CaptureStdout();
    EXPECT_CALL(iface2, onValueChanged);
    m2->changeValue(0);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "change 2!\n");

    ScratchConfiguration::removeExtension(extension);
}

TEST(EngineTest, IsRunning)
{
    Engine engine;
    ASSERT_FALSE(engine.isRunning());

    engine.start();
    ASSERT_TRUE(engine.isRunning());

    engine.stop();
    ASSERT_FALSE(engine.isRunning());

    engine.start();
    engine.run();
    ASSERT_FALSE(engine.isRunning());
}

TEST(EngineTest, StopSounds)
{
    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
    EXPECT_CALL(*player1, isLoaded).WillOnce(Return(false));
    EXPECT_CALL(*player2, isLoaded).WillOnce(Return(false));
    EXPECT_CALL(*player3, isLoaded).WillOnce(Return(false));
    EXPECT_CALL(*player1, load).WillOnce(Return(true));
    EXPECT_CALL(*player2, load).WillOnce(Return(true));
    EXPECT_CALL(*player3, load).WillOnce(Return(true));
    EXPECT_CALL(*player1, setVolume).Times(2);
    EXPECT_CALL(*player2, setVolume).Times(2);
    EXPECT_CALL(*player3, setVolume).Times(2);
    Project p("sounds.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    EXPECT_CALL(*player1, stop());
    EXPECT_CALL(*player2, stop());
    EXPECT_CALL(*player3, stop());
    engine->stopSounds();

    SoundPrivate::audioOutput = nullptr;
}

TEST(EngineTest, GlobalVolume)
{
    Engine engine;
    ASSERT_EQ(engine.globalVolume(), 100);

    engine.setGlobalVolume(58.3);
    ASSERT_EQ(std::round(engine.globalVolume() * 100) / 100, 58.3);

    AudioEngineMock audioEngine;
    engine.m_audioEngine = &audioEngine;

    EXPECT_CALL(audioEngine, volume()).WillOnce(Return(0.275));
    ASSERT_EQ(engine.globalVolume(), 27.5);

    EXPECT_CALL(audioEngine, setVolume(0.9236));
    engine.setGlobalVolume(92.36);
}

TEST(EngineTest, Step)
{
    Project p("step.sb3");
    ASSERT_TRUE(p.load());

    p.start();

    auto engine = p.engine();
    engine->setFps(250);
    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "test1");
    auto test1 = GET_VAR(stage, "test1");

    ASSERT_VAR(stage, "test2");
    auto test2 = GET_VAR(stage, "test2");

    for (int i = 1; i <= 3; i++) {
        engine->step();
        ASSERT_EQ(test1->value().toInt(), i);
        ASSERT_EQ(test2->value().toInt(), i > 1 ? 3 : 1);
    }
}

TEST(EngineTest, EventLoop)
{
    Engine engine;

    std::thread th([&engine]() { engine.runEventLoop(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    engine.stopEventLoop();
    th.join(); // should return immediately
}

TEST(EngineTest, Fps)
{
    Engine engine;
    ASSERT_EQ(engine.fps(), 30);

    engine.setFps(60.25);
    ASSERT_EQ(engine.fps(), 60.25);

    engine.setFps(0);
    ASSERT_EQ(engine.fps(), 60.25);

    engine.setFps(-5);
    ASSERT_EQ(engine.fps(), 60.25);

    engine.setFps(250);
    ASSERT_EQ(engine.fps(), 250);

    engine.setFps(30);
    ASSERT_EQ(engine.fps(), 30);

    engine.setFps(300);
    ASSERT_EQ(engine.fps(), 250);
}

/*TEST(EngineTest, FpsProject)
{
    Project p("2_frames.sb3");
    ASSERT_TRUE(p.load());

    ClockMock clock;
    Engine *engine = dynamic_cast<Engine *>(p.engine().get());
    engine->m_clock = &clock;

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(50));
    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(70));
    std::chrono::steady_clock::time_point time3(std::chrono::milliseconds(75));
    std::chrono::steady_clock::time_point time4(std::chrono::milliseconds(83));
    std::chrono::steady_clock::time_point time5(std::chrono::milliseconds(116));
    std::chrono::steady_clock::time_point time6(std::chrono::milliseconds(156));
    EXPECT_CALL(clock, currentSteadyTime())
        .WillOnce(Return(time1)) // tick start (frame 1 of 2)
        .WillOnce(Return(time1)) // step start
        .WillOnce(Return(time2)) // step loop check
        .WillOnce(Return(time3)) // step loop check (break)
        .WillOnce(Return(time3)) // tick end (then sleep 8 ms)
        .WillOnce(Return(time4)) // tick start (frame 2 of 2)
        .WillOnce(Return(time4)) // step start
        .WillOnce(Return(time4)) // step loop check
        .WillOnce(Return(time5)) // step loop check (break)
        .WillOnce(Return(time5)) // tick end (without sleeping)
        .WillOnce(Return(time5)) // tick start
        .WillOnce(Return(time5)) // step start
        .WillOnce(Return(time5)) // step loop check
        ;
    EXPECT_CALL(clock, sleep(std::chrono::milliseconds(8)));
    p.run();

    engine->setFps(10);
    RedrawMock redrawMock;
    auto handler = std::bind(&RedrawMock::redraw, &redrawMock);
    engine->aboutToRender().connect(handler);
    std::chrono::steady_clock::time_point time7(std::chrono::milliseconds(100));
    std::chrono::steady_clock::time_point time8(std::chrono::milliseconds(200));
    std::chrono::steady_clock::time_point time9(std::chrono::milliseconds(300));
    EXPECT_CALL(clock, currentSteadyTime())
        .WillOnce(Return(time7)) // tick start (frame 1 of 2)
        .WillOnce(Return(time7)) // step start
        .WillOnce(Return(time7)) // step loop check
        .WillOnce(Return(time7)) // step loop check (break because of redraw request)
        .WillOnce(Return(time7)) // tick end (then sleep 100 ms)
        .WillOnce(Return(time8)) // tick start (frame 2 of 2)
        .WillOnce(Return(time8)) // step start
        .WillOnce(Return(time8)) // step loop check
        .WillOnce(Return(time9)) // step loop check (break)
        .WillOnce(Return(time9)) // tick end (without sleeping)
        .WillOnce(Return(time9)) // tick start
        .WillOnce(Return(time9)) // step start
        .WillOnce(Return(time9)) // step loop check
        ;
    EXPECT_CALL(clock, sleep(std::chrono::milliseconds(100)));
    EXPECT_CALL(redrawMock, redraw()).Times(3);
    p.run();

    engine->setTurboModeEnabled(true);
    EXPECT_CALL(clock, currentSteadyTime())
        .WillOnce(Return(time7)) // tick start
        .WillOnce(Return(time7)) // step start
        .WillOnce(Return(time7)) // step loop check
        .WillOnce(Return(time7)) // step loop check
        .WillOnce(Return(time7)) // step loop check
        ;
    EXPECT_CALL(clock, sleep).Times(0);
    EXPECT_CALL(redrawMock, redraw());
    p.run();
}*/

TEST(EngineTest, TurboModeEnabled)
{
    Engine engine;
    ASSERT_FALSE(engine.turboModeEnabled());

    engine.setTurboModeEnabled(true);
    ASSERT_TRUE(engine.turboModeEnabled());

    engine.setTurboModeEnabled(false);
    ASSERT_FALSE(engine.turboModeEnabled());
}

/*TEST(EngineTest, ExecutionOrder)
{
    Project p("execution_order.sb3");
    ASSERT_TRUE(p.load());

    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_LIST(stage, "order");
    auto list = GET_LIST(stage, "order");
    ASSERT_EQ(list->size(), 13);

    ASSERT_EQ(Value((*list)[0]).toString(), "Sprite2");
    ASSERT_EQ(Value((*list)[1]).toString(), "Sprite3");
    ASSERT_EQ(Value((*list)[2]).toString(), "Sprite1");
    ASSERT_EQ(Value((*list)[3]).toString(), "Stage");
    ASSERT_EQ(Value((*list)[4]).toString(), "Sprite1 1");
    ASSERT_EQ(Value((*list)[5]).toString(), "Sprite1 2");
    ASSERT_EQ(Value((*list)[6]).toString(), "Sprite1 3");
    ASSERT_EQ(Value((*list)[7]).toString(), "Sprite2 msg");
    ASSERT_EQ(Value((*list)[8]).toString(), "Sprite3 msg");
    ASSERT_EQ(Value((*list)[9]).toString(), "Sprite1 1 msg");
    ASSERT_EQ(Value((*list)[10]).toString(), "Sprite1 2 msg");
    ASSERT_EQ(Value((*list)[11]).toString(), "Sprite1 3 msg");
    ASSERT_EQ(Value((*list)[12]).toString(), "Stage msg");
}*/

TEST(EngineTest, KeyState)
{
    Engine engine;
    ASSERT_FALSE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_FALSE(engine.keyPressed("any"));

    // Key name
    engine.setKeyState("A", true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("up arrow", true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_TRUE(engine.keyPressed("up arrow"));
    ASSERT_FALSE(engine.keyPressed("U"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("up arrow", false);
    engine.setKeyState("UP arrow", true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("u"));
    ASSERT_TRUE(engine.keyPressed("U"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("up arrow", true);
    engine.setKeyState("b", true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_TRUE(engine.keyPressed("b"));
    ASSERT_TRUE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("a", false);
    ASSERT_FALSE(engine.keyPressed("a"));
    ASSERT_TRUE(engine.keyPressed("b"));
    ASSERT_TRUE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("up arrow", false);
    ASSERT_FALSE(engine.keyPressed("a"));
    ASSERT_TRUE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("b", false);
    ASSERT_FALSE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("u", false);
    ASSERT_FALSE(engine.keyPressed("any"));

    engine.setKeyState("32", true);
    ASSERT_TRUE(engine.keyPressed("32"));
    ASSERT_TRUE(engine.keyPressed("space"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setAnyKeyPressed(true);
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setAnyKeyPressed(false);
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState("space", false);
    engine.setKeyState("a", false);
    engine.setKeyState("b", false);
    ASSERT_FALSE(engine.keyPressed("any"));

    engine.setAnyKeyPressed(true);
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setAnyKeyPressed(false);
    ASSERT_FALSE(engine.keyPressed("any"));

    // Key object
    engine.setKeyState(KeyEvent("A"), true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_FALSE(engine.keyPressed("up arrow"));
    ASSERT_TRUE(engine.keyPressed("any"));

    engine.setKeyState(KeyEvent("up arrow"), true);
    ASSERT_TRUE(engine.keyPressed("a"));
    ASSERT_FALSE(engine.keyPressed("b"));
    ASSERT_TRUE(engine.keyPressed("up arrow"));
    ASSERT_FALSE(engine.keyPressed("U"));
    ASSERT_TRUE(engine.keyPressed("any"));
}

TEST(EngineTest, WhenKeyPressed)
{
    Project p("when_key_pressed.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    // no pressed key
    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // space
    engine->setKeyState("space", true);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    engine->setKeyState("space", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // right arrow
    engine->setKeyState("right arrow", true);
    engine->step();

    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 1);
    engine->setKeyState("right arrow", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // right arrow - key object
    engine->setKeyState(KeyEvent("right arrow"), true);
    engine->step();

    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    engine->setKeyState("right arrow", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 3);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // any key
    engine->setAnyKeyPressed(true);
    engine->step();

    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 4);
    engine->setAnyKeyPressed(false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 4);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // a
    engine->setKeyState("a", true);
    engine->step();

    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    engine->setKeyState("a", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 5);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 0);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // x
    engine->setKeyState("x", true);
    engine->step();

    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 1);
    engine->setKeyState("x", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 6);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 0);

    // 4
    engine->setKeyState("4", true);
    engine->step();

    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 1);
    engine->setKeyState("4", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 7);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 1);

    // multiple
    engine->setKeyState("space", true);
    engine->setKeyState("x", true);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 2);
    engine->setKeyState("space", false);
    engine->setKeyState("x", false);
    engine->step();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 8);
    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 1);
}

TEST(EngineTest, MouseWheel)
{
    Project p("mouse_wheel.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    // Initial state
    ASSERT_VAR(stage, "up");
    ASSERT_EQ(GET_VAR(stage, "up")->value().toInt(), 0);
    ASSERT_VAR(stage, "down");
    ASSERT_EQ(GET_VAR(stage, "down")->value().toInt(), 0);
    ASSERT_VAR(stage, "any");
    ASSERT_EQ(GET_VAR(stage, "any")->value().toInt(), 0);

    // Up
    engine->mouseWheelUp();
    ASSERT_FALSE(engine->keyPressed("up arrow"));
    engine->step();
    ASSERT_VAR(stage, "up");
    ASSERT_EQ(GET_VAR(stage, "up")->value().toInt(), 1);
    ASSERT_VAR(stage, "down");
    ASSERT_EQ(GET_VAR(stage, "down")->value().toInt(), 0);
    ASSERT_VAR(stage, "any");
    ASSERT_EQ(GET_VAR(stage, "any")->value().toInt(), 0);

    engine->mouseWheelUp();
    ASSERT_FALSE(engine->keyPressed("up arrow"));
    engine->step();
    ASSERT_VAR(stage, "up");
    ASSERT_EQ(GET_VAR(stage, "up")->value().toInt(), 2);
    ASSERT_VAR(stage, "down");
    ASSERT_EQ(GET_VAR(stage, "down")->value().toInt(), 0);
    ASSERT_VAR(stage, "any");
    ASSERT_EQ(GET_VAR(stage, "any")->value().toInt(), 0);

    // Down
    engine->mouseWheelDown();
    ASSERT_FALSE(engine->keyPressed("down arrow"));
    engine->step();
    ASSERT_VAR(stage, "up");
    ASSERT_EQ(GET_VAR(stage, "up")->value().toInt(), 2);
    ASSERT_VAR(stage, "down");
    ASSERT_EQ(GET_VAR(stage, "down")->value().toInt(), 1);
    ASSERT_VAR(stage, "any");
    ASSERT_EQ(GET_VAR(stage, "any")->value().toInt(), 0);

    engine->mouseWheelDown();
    ASSERT_FALSE(engine->keyPressed("down arrow"));
    engine->step();
    ASSERT_VAR(stage, "up");
    ASSERT_EQ(GET_VAR(stage, "up")->value().toInt(), 2);
    ASSERT_VAR(stage, "down");
    ASSERT_EQ(GET_VAR(stage, "down")->value().toInt(), 2);
    ASSERT_VAR(stage, "any");
    ASSERT_EQ(GET_VAR(stage, "any")->value().toInt(), 0);
}

TEST(EngineTest, MouseX)
{
    Engine engine;
    ASSERT_EQ(engine.mouseX(), 0);

    engine.setMouseX(-128.038);
    ASSERT_EQ(engine.mouseX(), -128);

    engine.setMouseX(35.7);
    ASSERT_EQ(engine.mouseX(), 36);
}

TEST(EngineTest, MouseY)
{
    Engine engine;
    ASSERT_EQ(engine.mouseY(), 0);

    engine.setMouseY(179.1258);
    ASSERT_EQ(engine.mouseY(), 179);

    engine.setMouseY(-12.98);
    ASSERT_EQ(engine.mouseY(), -13);
}

TEST(EngineTest, MousePressed)
{
    Engine engine;
    ASSERT_FALSE(engine.mousePressed());

    engine.setMousePressed(true);
    ASSERT_TRUE(engine.mousePressed());

    engine.setMousePressed(false);
    ASSERT_FALSE(engine.mousePressed());
}

TEST(EngineTest, StageWidth)
{
    Engine engine;
    ASSERT_EQ(engine.stageWidth(), 480);

    engine.setStageWidth(640);
    ASSERT_EQ(engine.stageWidth(), 640);
}

TEST(EngineTest, StageHeight)
{
    Engine engine;
    ASSERT_EQ(engine.stageHeight(), 360);

    engine.setStageHeight(515);
    ASSERT_EQ(engine.stageHeight(), 515);
}

TEST(EngineTest, SpriteFencingEnabled)
{
    Engine engine;
    ASSERT_TRUE(engine.spriteFencingEnabled());

    engine.setSpriteFencingEnabled(false);
    ASSERT_FALSE(engine.spriteFencingEnabled());

    engine.setSpriteFencingEnabled(true);
    ASSERT_TRUE(engine.spriteFencingEnabled());
}

TEST(EngineTest, Timer)
{
    Engine engine;
    ASSERT_TRUE(engine.timer());
    engine.timer()->reset(); // shouldn't crash

    TimerMock timer;
    engine.setTimer(&timer);
    ASSERT_EQ(engine.timer(), &timer);

    EXPECT_CALL(timer, reset()).Times(1);
    engine.start();

    EXPECT_CALL(timer, reset()).Times(0);
    engine.stop();

    EXPECT_CALL(timer, reset()).Times(1);
    engine.run();
}

TEST(EngineTest, Extensions)
{
    Engine engine;

    auto extension1 = std::make_shared<ExtensionMock>();
    EXPECT_CALL(*extension1, name()).WillRepeatedly(Return("ext1"));
    EXPECT_CALL(*extension1, registerBlocks(&engine));
    EXPECT_CALL(*extension1, onInit(&engine));
    ScratchConfiguration::registerExtension(extension1);

    auto extension2 = std::make_shared<ExtensionMock>();
    EXPECT_CALL(*extension2, name()).WillRepeatedly(Return("ext2"));
    EXPECT_CALL(*extension2, registerBlocks(&engine));
    EXPECT_CALL(*extension2, onInit(&engine));
    ScratchConfiguration::registerExtension(extension2);

    engine.setExtensions({ "ext1", "ext2" });
    ASSERT_EQ(engine.extensions(), std::vector<std::string>({ "ext1", "ext2" }));

    ScratchConfiguration::removeExtension(extension1);
    ScratchConfiguration::removeExtension(extension2);
}

TEST(EngineTest, Broadcasts)
{
    Engine engine;
    ASSERT_TRUE(engine.broadcasts().empty());

    auto b1 = std::make_shared<Broadcast>("a", "message1");
    auto b2 = std::make_shared<Broadcast>("b", "message2");
    auto b3 = std::make_shared<Broadcast>("c", "Test");
    auto b4 = std::make_shared<Broadcast>("d", "TesT");
    engine.setBroadcasts({ b1, b2, b3, b4 });

    ASSERT_EQ(engine.broadcasts(), std::vector<std::shared_ptr<Broadcast>>({ b1, b2, b3, b4 }));
    ASSERT_EQ(engine.broadcastAt(0), b1);
    ASSERT_EQ(engine.broadcastAt(1), b2);
    ASSERT_EQ(engine.broadcastAt(2), b3);
    ASSERT_EQ(engine.broadcastAt(3), b4);
    ASSERT_EQ(engine.broadcastAt(4), nullptr);
    ASSERT_EQ(engine.broadcastAt(-1), nullptr);

    ASSERT_TRUE(engine.findBroadcasts("invalid").empty());
    ASSERT_EQ(engine.findBroadcasts("message1"), std::vector<int>({ 0 }));
    ASSERT_EQ(engine.findBroadcasts("message2"), std::vector<int>({ 1 }));
    ASSERT_EQ(engine.findBroadcasts("Test"), std::vector<int>({ 2, 3 }));
    ASSERT_EQ(engine.findBroadcasts("MessAge2"), std::vector<int>({ 1 }));
    ASSERT_EQ(engine.findBroadcasts("tEst"), std::vector<int>({ 2, 3 }));

    ASSERT_EQ(engine.findBroadcastById("e"), -1);
    ASSERT_EQ(engine.findBroadcastById("a"), 0);
    ASSERT_EQ(engine.findBroadcastById("b"), 1);
    ASSERT_EQ(engine.findBroadcastById("c"), 2);
}

TEST(EngineTest, TargetClickScripts)
{
    Project p("target_click_scripts.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    // Initial state
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 0);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 0);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    // Sprite1
    Target *sprite = engine->targetAt(engine->findTarget("Sprite1"));
    ASSERT_TRUE(sprite);
    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 1);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 0);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 0);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    // Sprite2
    sprite = engine->targetAt(engine->findTarget("Sprite2"));
    ASSERT_TRUE(sprite);
    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 1);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 2);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    // Hidden sprite
    sprite = engine->targetAt(engine->findTarget("Hidden"));
    ASSERT_TRUE(sprite);
    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 2);
    ASSERT_VAR(stage, "hidden");
    ASSERT_EQ(GET_VAR(stage, "hidden")->value().toInt(), 0);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    engine->clickTarget(sprite);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 2);
    ASSERT_VAR(stage, "hidden");
    ASSERT_EQ(GET_VAR(stage, "hidden")->value().toInt(), 0);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 0);

    // Stage
    engine->clickTarget(stage);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 2);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 1);

    engine->clickTarget(stage);
    engine->step();
    ASSERT_VAR(stage, "1");
    ASSERT_EQ(GET_VAR(stage, "1")->value().toInt(), 2);
    ASSERT_VAR(stage, "2");
    ASSERT_EQ(GET_VAR(stage, "2")->value().toInt(), 2);
    ASSERT_VAR(stage, "stage");
    ASSERT_EQ(GET_VAR(stage, "stage")->value().toInt(), 2);
}

TEST(EngineTest, Targets)
{
    Engine engine;
    ASSERT_TRUE(engine.targets().empty());

    auto t1 = std::make_shared<Sprite>();
    t1->setName("Sprite1");
    t1->setVisible(true);
    t1->setLayerOrder(3);
    auto t2 = std::make_shared<Sprite>();
    auto block1 = std::make_shared<Block>("", "");
    auto block2 = std::make_shared<Block>("", "");
    t2->setName("Sprite2");
    t2->setVisible(false);
    t2->setLayerOrder(1);
    t2->addBlock(block1);
    t2->addBlock(block2);
    auto t3 = std::make_shared<Stage>();
    t3->setName("Stage");
    t3->setLayerOrder(0);
    engine.setTargets({ t1, t2, t3 });

    ASSERT_EQ(engine.targets(), std::vector<std::shared_ptr<Target>>({ t1, t2, t3 }));
    ASSERT_EQ(engine.targetAt(0), t1.get());
    ASSERT_EQ(engine.targetAt(1), t2.get());
    ASSERT_EQ(engine.targetAt(2), t3.get());
    ASSERT_EQ(engine.targetAt(3), nullptr);
    ASSERT_EQ(engine.targetAt(-1), nullptr);

    ASSERT_EQ(engine.findTarget("invalid"), -1);
    ASSERT_EQ(engine.findTarget("Sprite1"), 0);
    ASSERT_EQ(engine.findTarget("Sprite2"), 1);
    ASSERT_EQ(engine.findTarget("Stage"), -1);
    ASSERT_EQ(engine.findTarget("_stage_"), 2);

    auto t4 = std::make_shared<Sprite>();
    t4->setName("Stage");
    t4->setVisible(true);
    t4->setLayerOrder(2);
    engine.setTargets({ t1, t2, t4 });
    ASSERT_EQ(engine.findTarget("Stage"), 2);
    ASSERT_EQ(engine.findTarget("_stage_"), -1);

    std::vector<Target *> visibleTargets;
    engine.getVisibleTargets(visibleTargets);
    ASSERT_EQ(visibleTargets, std::vector<Target *>({ t1.get(), t4.get() }));

    engine.setTargets({ t1, t2, t3, t4 });
    ASSERT_EQ(engine.findTarget("Stage"), 3);
    ASSERT_EQ(engine.findTarget("_stage_"), 2);

    engine.getVisibleTargets(visibleTargets);
    ASSERT_EQ(visibleTargets, std::vector<Target *>({ t1.get(), t4.get(), t3.get() }));

    ASSERT_EQ(t1->engine(), &engine);
    ASSERT_EQ(t2->engine(), &engine);
    ASSERT_EQ(t3->engine(), &engine);

    ASSERT_EQ(block1->engine(), &engine);
    ASSERT_EQ(block2->engine(), &engine);
}

void createTargets(Engine *engine, std::vector<Sprite *> &sprites)
{
    auto stage = std::make_shared<Stage>();
    stage->setLayerOrder(0);
    auto sprite1 = std::make_shared<Sprite>();
    sprite1->setLayerOrder(1);
    auto sprite2 = std::make_shared<Sprite>();
    sprite2->setLayerOrder(5);
    auto sprite3 = std::make_shared<Sprite>();
    sprite3->setLayerOrder(3);
    auto sprite4 = std::make_shared<Sprite>();
    sprite4->setLayerOrder(4);
    auto sprite5 = std::make_shared<Sprite>();
    sprite5->setLayerOrder(2);

    engine->setTargets({ stage, sprite1, sprite2, sprite3, sprite4, sprite5 });
    sprites = { sprite1.get(), sprite2.get(), sprite3.get(), sprite4.get(), sprite5.get() };

    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);
}

TEST(EngineTest, MoveDrawableToFront)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveDrawableToFront(sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 11);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 6);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 10);

    for (int i = 0; i < 2; i++) {
        engine.moveDrawableToFront(sprites[0]);
        ASSERT_EQ(sprites[0]->layerOrder(), 11);
        ASSERT_EQ(sprites[1]->layerOrder(), 3);
        ASSERT_EQ(sprites[2]->layerOrder(), 10);
        ASSERT_EQ(sprites[3]->layerOrder(), 2);
        ASSERT_EQ(sprites[4]->layerOrder(), 1);
        ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 5);
        ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 6);
        ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 7);
        ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 8);
        ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 9);
    }

    engine.moveDrawableToFront(sprites[0]->bubble());
    ASSERT_EQ(sprites[0]->layerOrder(), 10);
    ASSERT_EQ(sprites[1]->layerOrder(), 3);
    ASSERT_EQ(sprites[2]->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->layerOrder(), 2);
    ASSERT_EQ(sprites[4]->layerOrder(), 1);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 11);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 6);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 8);

    auto stage = std::make_shared<Stage>();
    stage->setLayerOrder(0);
    auto sprite = std::make_shared<Sprite>();
    sprite->setLayerOrder(1);

    engine.setTargets({ stage, sprite });
    engine.moveDrawableToFront(sprite.get());
    ASSERT_EQ(sprite->layerOrder(), 3);
}

TEST(EngineTest, MoveDrawableToBack)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveDrawableToBack(sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    for (int i = 0; i < 1; i++) {
        engine.moveDrawableToBack(sprites[1]);
        ASSERT_EQ(sprites[0]->layerOrder(), 3);
        ASSERT_EQ(sprites[1]->layerOrder(), 1);
        ASSERT_EQ(sprites[2]->layerOrder(), 2);
        ASSERT_EQ(sprites[3]->layerOrder(), 5);
        ASSERT_EQ(sprites[4]->layerOrder(), 4);
        ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
        ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
        ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
        ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
        ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);
    }

    engine.moveDrawableToBack(sprites[2]->bubble());
    ASSERT_EQ(sprites[0]->layerOrder(), 4);
    ASSERT_EQ(sprites[1]->layerOrder(), 2);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 6);
    ASSERT_EQ(sprites[4]->layerOrder(), 5);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    auto stage = std::make_shared<Stage>();
    stage->setLayerOrder(0);
    auto sprite = std::make_shared<Sprite>();
    sprite->setLayerOrder(1);

    engine.setTargets({ stage, sprite });
    engine.moveDrawableToBack(sprite.get());
    ASSERT_EQ(sprite->layerOrder(), 1);
}

TEST(EngineTest, MoveDrawableForwardLayers)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    const auto &targets = engine.targets();

    for (auto target : targets)
        target->bubble()->setText("test");

    engine.moveDrawableForwardLayers(sprites[4], 2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableForwardLayers(sprites[4], 2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 6);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableForwardLayers(sprites[4], -3);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableForwardLayers(sprites[2], -3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableForwardLayers(sprites[0]->bubble(), 3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    sprites[1]->bubble()->setText("");
    sprites[3]->bubble()->setText("");
    engine.moveDrawableForwardLayers(sprites[2], 8);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 11);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 6);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 10);
}

TEST(EngineTest, MoveDrawableBackwardLayers)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    const auto &targets = engine.targets();

    for (auto target : targets)
        target->bubble()->setText("test");

    engine.moveDrawableBackwardLayers(sprites[4], -2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableBackwardLayers(sprites[4], -2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 6);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableBackwardLayers(sprites[4], 3);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableBackwardLayers(sprites[2], 3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    engine.moveDrawableBackwardLayers(sprites[0]->bubble(), -3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 10);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 11);

    sprites[1]->bubble()->setText("");
    sprites[3]->bubble()->setText("");
    engine.moveDrawableBackwardLayers(sprites[2], -8);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 11);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);
    ASSERT_EQ(sprites[0]->bubble()->layerOrder(), 9);
    ASSERT_EQ(sprites[1]->bubble()->layerOrder(), 6);
    ASSERT_EQ(sprites[2]->bubble()->layerOrder(), 7);
    ASSERT_EQ(sprites[3]->bubble()->layerOrder(), 8);
    ASSERT_EQ(sprites[4]->bubble()->layerOrder(), 10);
}

TEST(EngineTest, MoveDrawableBehindOther)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveDrawableBehindOther(sprites[4], sprites[3]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);

    engine.moveDrawableBehindOther(sprites[3], sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 2);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);

    engine.moveDrawableBehindOther(sprites[4], sprites[0]);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 4);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 1);
}

TEST(EngineTest, Stage)
{
    Engine engine;
    ASSERT_EQ(engine.stage(), nullptr);

    auto t1 = std::make_shared<Sprite>();
    t1->setName("Sprite1");
    engine.setTargets({ t1 });
    ASSERT_EQ(engine.stage(), nullptr);

    auto t2 = std::make_shared<Stage>();
    t2->setName("Stage");
    engine.setTargets({ t1, t2 });
    ASSERT_EQ(engine.stage(), t2.get());

    auto t3 = std::make_shared<Sprite>();
    t3->setName("Sprite2");
    engine.setTargets({ t1, t2, t3 });
    ASSERT_EQ(engine.stage(), t2.get());

    engine.setTargets({ t2, t3 });
    ASSERT_EQ(engine.stage(), t2.get());

    engine.setTargets({ t1, t3 });
    ASSERT_EQ(engine.stage(), nullptr);
}

TEST(EngineTest, Monitors)
{
    Engine engine;
    ASSERT_TRUE(engine.monitors().empty());

    auto m1 = std::make_shared<Monitor>("", "");
    auto m2 = std::make_shared<Monitor>("", "");
    auto m3 = std::make_shared<Monitor>("", "");

    engine.setMonitors({ m1, m2, m3 });
    ASSERT_EQ(engine.monitors(), std::vector<std::shared_ptr<Monitor>>({ m1, m2, m3 }));

    AddRemoveMonitorMock addMonitorMock;
    auto handler = std::bind(&AddRemoveMonitorMock::monitorAdded, &addMonitorMock, std::placeholders::_1);
    engine.monitorAdded().connect(handler);
    engine.setMonitors({});

    EXPECT_CALL(addMonitorMock, monitorAdded(m1.get()));
    EXPECT_CALL(addMonitorMock, monitorAdded(m2.get()));
    EXPECT_CALL(addMonitorMock, monitorAdded(m3.get()));
    engine.setMonitors({ m1, m2, m3 });
}

TEST(EngineTest, CreateMissingMonitors)
{
    static constexpr const char *VARIABLE_OPCODE = "data_variable";
    static constexpr const char *VARIABLE_FIELD_NAME = "VARIABLE";
    static constexpr int VARIABLE_FIELD_ID = 1;

    static constexpr const char *LIST_OPCODE = "data_listcontents";
    static constexpr const char *LIST_FIELD_NAME = "LIST";
    static constexpr int LIST_FIELD_ID = 2;

    auto var1 = std::make_shared<Variable>("a", "var1");
    auto var2 = std::make_shared<Variable>("b", "var2");
    auto var3 = std::make_shared<Variable>("c", "var3");
    auto var4 = std::make_shared<Variable>("d", "var4");
    auto var5 = std::make_shared<Variable>("e", "var5");
    auto list1 = std::make_shared<List>("f", "list1");
    auto list2 = std::make_shared<List>("g", "list2");
    auto target1 = std::make_shared<Stage>();
    auto target2 = std::make_shared<Sprite>();
    target1->addVariable(var1);
    target1->addVariable(var2);
    target1->addList(list1);
    target2->addVariable(var3);
    target2->addVariable(var4);
    target2->addVariable(var5);
    target2->addList(list2);

    auto m1 = std::make_shared<Monitor>(var1->id(), "data_variable");
    auto m2 = std::make_shared<Monitor>(var3->id(), "data_variable");
    auto m3 = std::make_shared<Monitor>(list2->id(), "data_listcontents");

    auto checkVariableMonitor = [](std::shared_ptr<Monitor> monitor, std::shared_ptr<Variable> var) {
        ASSERT_EQ(var->monitor(), monitor.get());

        auto block = monitor->block();
        ASSERT_EQ(monitor->id(), var->id());
        ASSERT_EQ(monitor->opcode(), "data_variable");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_TRUE(monitor->needsAutoPosition());
        ASSERT_FALSE(monitor->visible());
        ASSERT_EQ(block->fields().size(), 1);

        auto field = block->fieldAt(0);
        ASSERT_EQ(field->name(), VARIABLE_FIELD_NAME);
        ASSERT_EQ(field->fieldId(), VARIABLE_FIELD_ID);
        ASSERT_EQ(field->value(), var->name());
        ASSERT_EQ(field->valuePtr(), var);

        if (var->target()->isStage())
            ASSERT_EQ(monitor->sprite(), nullptr);
        else
            ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(var->target()));
    };

    auto checkListMonitor = [](std::shared_ptr<Monitor> monitor, std::shared_ptr<List> list) {
        ASSERT_EQ(list->monitor(), monitor.get());

        auto block = monitor->block();
        ASSERT_EQ(monitor->id(), list->id());
        ASSERT_EQ(monitor->opcode(), "data_listcontents");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::List);
        ASSERT_TRUE(monitor->needsAutoPosition());
        ASSERT_FALSE(monitor->visible());
        ASSERT_EQ(block->fields().size(), 1);

        auto field = block->fieldAt(0);
        ASSERT_EQ(field->name(), LIST_FIELD_NAME);
        ASSERT_EQ(field->fieldId(), LIST_FIELD_ID);
        ASSERT_EQ(field->value(), list->name());
        ASSERT_EQ(field->valuePtr(), list);

        if (list->target()->isStage())
            ASSERT_EQ(monitor->sprite(), nullptr);
        else
            ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(list->target()));
    };

    {
        Engine engine;
        engine.setTargets({ target1, target2 });
        engine.setMonitors({ m1, m2, m3 });

        const auto &monitors = engine.monitors();
        ASSERT_EQ(monitors.size(), 3);
        ASSERT_EQ(monitors[0], m1);
        ASSERT_EQ(monitors[1], m2);
        ASSERT_EQ(monitors[2], m3);

        engine.createVariableMonitor(var2, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);
        engine.createListMonitor(list1, LIST_OPCODE, LIST_FIELD_NAME, LIST_FIELD_ID, nullptr);
        engine.createVariableMonitor(var4, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);
        engine.createVariableMonitor(var5, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);

        ASSERT_EQ(monitors.size(), 7);
        checkVariableMonitor(monitors[3], var2);
        checkListMonitor(monitors[4], list1);
        checkVariableMonitor(monitors[5], var4);
        checkVariableMonitor(monitors[6], var5);
    }

    var2->setMonitor(nullptr);
    list1->setMonitor(nullptr);
    var4->setMonitor(nullptr);
    var5->setMonitor(nullptr);

    {
        Engine engine;
        AddRemoveMonitorMock addMonitorMock;
        auto handler = std::bind(&AddRemoveMonitorMock::monitorAdded, &addMonitorMock, std::placeholders::_1);
        engine.monitorAdded().connect(handler);

        EXPECT_CALL(addMonitorMock, monitorAdded(m1.get()));
        EXPECT_CALL(addMonitorMock, monitorAdded(m2.get()));
        EXPECT_CALL(addMonitorMock, monitorAdded(m3.get()));
        engine.setMonitors({ m1, m2, m3 });

        Monitor *m4, *m5, *m6, *m7;
        EXPECT_CALL(addMonitorMock, monitorAdded(_)).WillOnce(SaveArg<0>(&m4)).WillOnce(SaveArg<0>(&m5)).WillOnce(SaveArg<0>(&m6)).WillOnce(SaveArg<0>(&m7));

        engine.createVariableMonitor(var2, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);
        engine.createListMonitor(list1, LIST_OPCODE, LIST_FIELD_NAME, LIST_FIELD_ID, nullptr);
        engine.createVariableMonitor(var4, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);
        engine.createVariableMonitor(var5, VARIABLE_OPCODE, VARIABLE_FIELD_NAME, VARIABLE_FIELD_ID, nullptr);

        const auto &monitors = engine.monitors();
        ASSERT_EQ(monitors.size(), 7);
        ASSERT_EQ(monitors[3].get(), m4);
        ASSERT_EQ(monitors[4].get(), m5);
        ASSERT_EQ(monitors[5].get(), m6);
        ASSERT_EQ(monitors[6].get(), m7);
    }
}

void questionFunction(const std::string &)
{
}

/*TEST(EngineTest, Clones)
{
    Project p("clones.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "clone1");
    ASSERT_EQ(GET_VAR(stage, "clone1")->value().toInt(), 1);
    ASSERT_VAR(stage, "clone2");
    ASSERT_EQ(GET_VAR(stage, "clone2")->value().toInt(), 1);
    ASSERT_VAR(stage, "clone3");
    ASSERT_EQ(GET_VAR(stage, "clone3")->value().toInt(), 1);
    ASSERT_VAR(stage, "clone4");
    ASSERT_EQ(GET_VAR(stage, "clone4")->value().toInt(), 1);
    ASSERT_VAR(stage, "clone5");
    ASSERT_EQ(GET_VAR(stage, "clone5")->value().toInt(), 110);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());

    ASSERT_LIST(stage, "log1");
    auto list = GET_LIST(stage, "log1");

    for (int i = 0; i < list->size(); i++) {
        if (i < 10)
            ASSERT_EQ(value_toInt(&(*list)[i]), 1);
        else
            ASSERT_EQ(value_toInt(&(*list)[i]), 2);
    }

    ASSERT_LIST(stage, "log2");
    list = GET_LIST(stage, "log2");

    for (int i = 0; i < list->size(); i++) {
        if (i < 10)
            ASSERT_EQ(value_toInt(&(*list)[i]), 1);
        else
            ASSERT_EQ(Value((*list)[i]).toString(), "12");
    }
}

TEST(EngineTest, CloneLimit)
{
    Project p("clone_limit.sb3");
    ASSERT_TRUE(p.load());
    auto engine = p.engine();
    ASSERT_EQ(engine->cloneLimit(), 300);
    ASSERT_EQ(engine->cloneCount(), 0);

    engine->setTurboModeEnabled(true);

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    p.run();
    ASSERT_VAR(stage, "count");
    ASSERT_EQ(GET_VAR(stage, "count")->value().toInt(), 300);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());
    ASSERT_EQ(engine->cloneCount(), 300);

    engine->setCloneLimit(475);
    ASSERT_EQ(engine->cloneLimit(), 475);
    p.run();
    ASSERT_VAR(stage, "count");
    ASSERT_EQ(GET_VAR(stage, "count")->value().toInt(), 475);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());
    ASSERT_EQ(engine->cloneCount(), 475);

    engine->setCloneLimit(0);
    ASSERT_EQ(engine->cloneLimit(), 0);
    p.run();
    ASSERT_VAR(stage, "count");
    ASSERT_EQ(GET_VAR(stage, "count")->value().toInt(), 0);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());
    ASSERT_EQ(engine->cloneCount(), 0);

    engine->setCloneLimit(-1);
    ASSERT_EQ(engine->cloneLimit(), -1);
    p.run();
    ASSERT_VAR(stage, "count");
    ASSERT_GT(GET_VAR(stage, "count")->value().toInt(), 500);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());
    ASSERT_GT(engine->cloneCount(), 500);

    engine->setCloneLimit(-5);
    ASSERT_EQ(engine->cloneLimit(), -1);
    p.run();
    ASSERT_VAR(stage, "count");
    ASSERT_GT(GET_VAR(stage, "count")->value().toInt(), 500);
    ASSERT_VAR(stage, "delete_passed");
    ASSERT_TRUE(GET_VAR(stage, "delete_passed")->value().toBool());
    ASSERT_GT(engine->cloneCount(), 500);

    engine->stop();
    ASSERT_EQ(engine->cloneCount(), 0);
}

TEST(EngineTest, BackdropBroadcasts)
{
    Project p("backdrop_broadcasts.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();
    engine->setFps(1000);

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "test1");
    ASSERT_EQ(GET_VAR(stage, "test1")->value().toInt(), 4);
    ASSERT_VAR(stage, "test2");
    ASSERT_EQ(GET_VAR(stage, "test2")->value().toInt(), 14);
    ASSERT_VAR(stage, "test3");
    ASSERT_EQ(GET_VAR(stage, "test3")->value().toInt(), 10);
    ASSERT_VAR(stage, "test4");
    ASSERT_EQ(GET_VAR(stage, "test4")->value().toInt(), 10);
    ASSERT_VAR(stage, "test5");
    ASSERT_EQ(GET_VAR(stage, "test5")->value().toString(), "2 2 0 0");
}

TEST(EngineTest, BroadcastsProject)
{
    Project p("broadcasts.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    StopMock stopMock;
    EXPECT_CALL(stopMock, threadRemoved(_)).Times(21).WillRepeatedly(WithArgs<0>(Invoke([](Thread *thread) {
        ASSERT_TRUE(thread);
        ASSERT_FALSE(thread->isFinished());
    })));

    engine->threadAboutToStop().connect(&StopMock::threadRemoved, &stopMock);
    engine->setFps(1000);
    p.run();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "test1");
    ASSERT_EQ(GET_VAR(stage, "test1")->value().toInt(), 4);
    ASSERT_VAR(stage, "test2");
    ASSERT_EQ(GET_VAR(stage, "test2")->value().toInt(), 14);
    ASSERT_VAR(stage, "test3");
    ASSERT_EQ(GET_VAR(stage, "test3")->value().toInt(), 10);
    ASSERT_VAR(stage, "test4");
    ASSERT_EQ(GET_VAR(stage, "test4")->value().toInt(), 10);
    ASSERT_VAR(stage, "test5");
    ASSERT_EQ(GET_VAR(stage, "test5")->value().toString(), "2 2 0 0");
}*/

TEST(EngineTest, StopAll)
{
    Project p("stop_all.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "i");
    ASSERT_EQ(GET_VAR(stage, "i")->value().toInt(), 11);
    ASSERT_FALSE(engine->isRunning());
}

TEST(EngineTest, StopAllBypass)
{
    Project p("stop_all_bypass.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    StopMock stopMock;
    EXPECT_CALL(stopMock, threadRemoved(_)).Times(2).WillRepeatedly(WithArgs<0>(Invoke([](Thread *thread) {
        ASSERT_TRUE(thread);
        ASSERT_FALSE(thread->isFinished());
    })));

    engine->threadAboutToStop().connect(&StopMock::threadRemoved, &stopMock);
    p.run();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "i");
    ASSERT_EQ(GET_VAR(stage, "i")->value().toInt(), 1);

    ASSERT_VAR(stage, "j");
    ASSERT_EQ(GET_VAR(stage, "j")->value().toInt(), 5);

    ASSERT_FALSE(engine->isRunning());
}

TEST(EngineTest, StopOtherScriptsInSprite)
{
    Project p("stop_other_scripts_in_sprite.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    StopMock stopMock;
    EXPECT_CALL(stopMock, threadRemoved(_)).Times(4).WillRepeatedly(WithArgs<0>(Invoke([](Thread *thread) {
        ASSERT_TRUE(thread);
        ASSERT_FALSE(thread->isFinished());
    })));

    engine->threadAboutToStop().connect(&StopMock::threadRemoved, &stopMock);
    p.run();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "i");
    ASSERT_EQ(GET_VAR(stage, "i")->value().toInt(), 10);

    ASSERT_VAR(stage, "j");
    ASSERT_EQ(GET_VAR(stage, "j")->value().toInt(), 109);

    ASSERT_VAR(stage, "k");
    ASSERT_EQ(GET_VAR(stage, "k")->value().toInt(), 10);

    ASSERT_VAR(stage, "l");
    ASSERT_EQ(GET_VAR(stage, "l")->value().toInt(), 110);
}

/*TEST(EngineTest, EdgeActivatedHats)
{
    Project p("when_greater_than.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = std::static_pointer_cast<Engine>(p.engine());

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "test1");
    auto var1 = GET_VAR(stage, "test1");
    ASSERT_VAR(stage, "test2");
    auto var2 = GET_VAR(stage, "test2");

    ASSERT_EQ(var1->value().toInt(), 0);
    ASSERT_EQ(var2->value().toInt(), 0);

    TimerMock timer;
    AudioInputMock audioInput;
    engine->setTimer(&timer);
    EventBlocks::audioInput = &audioInput;
    auto audioLoudness = std::make_shared<AudioLoudnessMock>();
    EXPECT_CALL(audioInput, audioLoudness()).WillRepeatedly(Return(audioLoudness));

    EXPECT_CALL(timer, value()).WillOnce(Return(5));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(1));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 0);
    ASSERT_EQ(var2->value().toInt(), 0);

    EXPECT_CALL(timer, value()).WillOnce(Return(10));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(9));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 0);
    ASSERT_EQ(var2->value().toInt(), 1);

    EXPECT_CALL(timer, value()).WillOnce(Return(10.2));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(10));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 1);
    ASSERT_EQ(var2->value().toInt(), 1);

    EXPECT_CALL(timer, value()).WillOnce(Return(15));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(2));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 1);
    ASSERT_EQ(var2->value().toInt(), 1);

    EXPECT_CALL(timer, value()).WillOnce(Return(12));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(8));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 1);
    ASSERT_EQ(var2->value().toInt(), 2);

    EXPECT_CALL(timer, value()).WillOnce(Return(8));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(3));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 1);
    ASSERT_EQ(var2->value().toInt(), 2);

    EXPECT_CALL(timer, value()).WillOnce(Return(11));
    EXPECT_CALL(*audioLoudness, getLoudness()).WillOnce(Return(15));
    engine->step();
    ASSERT_EQ(var1->value().toInt(), 2);
    ASSERT_EQ(var2->value().toInt(), 3);

    EventBlocks::audioInput = nullptr;
}*/

TEST(EngineTest, UserAgent)
{
    Engine engine;
    ASSERT_TRUE(engine.userAgent().empty());

    engine.setUserAgent("test");
    ASSERT_EQ(engine.userAgent(), "test");
}

/*TEST(EngineTest, UnsupportedBlocks)
{
    Project p("unsupported_blocks.sb3");
    ASSERT_TRUE(p.load());
    ASSERT_EQ(
        p.engine()->unsupportedBlocks(),
        std::unordered_set<std::string>({ "ev3_motorTurnClockwise", "ev3_motorSetPower", "ev3_getMotorPosition", "ev3_whenButtonPressed", "ev3_getBrightness", "ev3_getDistance" }));
}*/

TEST(EngineTest, NoCrashAfterStop)
{
    // Regtest for #186
    Project p("regtest_projects/186_crash_after_stop.sb3");
    ASSERT_TRUE(p.load());
    p.run();
}

TEST(EngineTest, NoCrashOnBroadcastSelfCall)
{
    // Regtest for #256
    Project p("regtest_projects/256_broadcast_self_call_crash.sb3");
    ASSERT_TRUE(p.load());
    p.run();
}

TEST(EngineTest, NoRefreshWhenCallingRunningBroadcast)
{
    // Regtest for #257
    Project p("regtest_projects/257_double_broadcast_stop.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();
    engine->setFps(1000);

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "passed1");
    ASSERT_TRUE(GET_VAR(stage, "passed1")->value().toBool());

    ASSERT_VAR(stage, "passed2");
    ASSERT_TRUE(GET_VAR(stage, "passed2")->value().toBool());
}

/*TEST(EngineTest, NoStopWhenCallingRunningBroadcastFromCustomBlock)
{
    // Regtest for #257
    Project p("regtest_projects/277_custom_block_call_running_broadcast_stop.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "passed1");
    ASSERT_TRUE(GET_VAR(stage, "passed1")->value().toBool());

    ASSERT_VAR(stage, "passed2");
    ASSERT_TRUE(GET_VAR(stage, "passed2")->value().toBool());
}*/

TEST(EngineTest, ResetRunningHats)
{
    // Regtest for #395
    Project p("regtest_projects/395_reset_running_hats.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), true);
    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), false);
    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), true);
    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), false);
    engine->step();

    ASSERT_VAR(stage, "test");
    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 1);

    engine->step();
    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 1);

    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), true);
    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), false);
    engine->step();

    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 2);
}

TEST(EngineTest, StopBeforeStarting)
{
    // Regtest for #394
    Project p("regtest_projects/394_stop_before_starting.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    Thread fakeThread(nullptr, nullptr, nullptr);
    engine->broadcast(0, &fakeThread, false);
    engine->step();

    ASSERT_VAR(stage, "test");
    ASSERT_FALSE(GET_VAR(stage, "test")->value().toBool());

    GET_VAR(stage, "test")->setValue(true);
    engine->broadcast(0, &fakeThread, false);
    engine->start();
    engine->step();
    ASSERT_VAR(stage, "test");
    ASSERT_TRUE(GET_VAR(stage, "test")->value().toBool());
}

TEST(EngineTest, NoCrashWhenLoadingUndefinedVariableOrListMonitor)
{
    // Regtest for #445
    Project p("regtest_projects/445_undefined_variable_or_list_monitor_crash.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    // The undefined variable and list should now be defined
    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);
    ASSERT_VAR(stage, "test");
    auto var = GET_VAR(stage, "test");
    ASSERT_EQ(var->id(), "E,r`5qYWCdXa~yj7nDS]");
    ASSERT_EQ(var->value(), Value());

    Target *sprite = engine->targetAt(engine->findTarget("Sprite1"));
    ASSERT_TRUE(sprite);
    ASSERT_LIST(sprite, "test");
    auto list = GET_LIST(sprite, "test");
    ASSERT_EQ(list->id(), "7a5rAs|X2_[1APT7@B1V");
    ASSERT_TRUE(list->empty());
}

TEST(EngineTest, NoCrashWithUndefinedVariableOrList)
{
    // Regtest for #446
    Project p("regtest_projects/446_undefined_variable_or_list_input_crash.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    // The undefined variable and list should now be defined
    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);
    ASSERT_VAR(stage, "test var");
    auto var = GET_VAR(stage, "test var");
    ASSERT_EQ(var->id(), "J3iHmLt:dwaR}^-fnGmG");
    ASSERT_EQ(var->value(), Value());

    ASSERT_LIST(stage, "test list");
    auto list = GET_LIST(stage, "test list");
    ASSERT_EQ(list->id(), "}l+w#Er5y!:*gh~5!3t%");
    ASSERT_TRUE(list->empty());

    // Test with fields
    p.setFileName("regtest_projects/446_undefined_variable_or_list_field_crash.sb3");
    ASSERT_TRUE(p.load());

    engine = p.engine();

    // The undefined variable and list should now be defined
    stage = engine->stage();
    ASSERT_TRUE(stage);
    ASSERT_VAR(stage, "test var");
    var = GET_VAR(stage, "test var");
    ASSERT_EQ(var->id(), "J3iHmLt:dwaR}^-fnGmG");
    ASSERT_EQ(var->value(), Value());

    ASSERT_LIST(stage, "test list");
    list = GET_LIST(stage, "test list");
    ASSERT_EQ(list->id(), "}l+w#Er5y!:*gh~5!3t%");
    ASSERT_TRUE(list->empty());
}

TEST(EngineTest, AlwaysStopCloneThreads)
{
    // Regtest for #547
    Project p("regtest_projects/547_stop_clone_threads_in_stop_all.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    engine->run();

    ASSERT_VAR(stage, "test");
    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 0);
}

TEST(EngineTest, DuplicateVariableOrListIDs)
{
    // Regtest for #567
    Project p("regtest_projects/567_duplicate_variable_list_id.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    engine->run();

    ASSERT_VAR(stage, "passed");
    ASSERT_TRUE(GET_VAR(stage, "passed")->value().toBool());
}

/*TEST(EngineTest, BroadcastStopsWaitBlocks)
{
    // Regtest for #563
    Project p("regtest_projects/563_broadcast_stops_wait_blocks.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    engine->run();

    ASSERT_VAR(stage, "broadcast_passed");
    ASSERT_TRUE(GET_VAR(stage, "broadcast_passed")->value().toBool());
    ASSERT_VAR(stage, "backdrop_passed");
    ASSERT_TRUE(GET_VAR(stage, "backdrop_passed")->value().toBool());
}

TEST(EngineTest, BroadcastAndWaitCaseInsensitive)
{
    // Regtest for #578
    Project p("regtest_projects/578_broadcast_and_wait_case_insensitive.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    engine->run();

    ASSERT_VAR(stage, "passed");
    ASSERT_TRUE(GET_VAR(stage, "passed")->value().toBool());
}*/
