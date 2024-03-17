#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/keyevent.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/field.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <scratch/sound_p.h>
#include <timermock.h>
#include <clockmock.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>
#include <monitorhandlermock.h>
#include <blocksectionmock.h>
#include <thread>

#include "../common.h"
#include "engine/internal/engine.h"
#include "engine/internal/clock.h"

// TODO: Remove this
#include "blocks/variableblocks.h"
#include "blocks/listblocks.h"

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

class AddRemoveMonitorMock
{
    public:
        MOCK_METHOD(void, monitorAdded, (Monitor *));
        MOCK_METHOD(void, monitorRemoved, (Monitor *, IMonitorHandler *));
};

template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f)
{
    typedef T(fnType)(U...);
    fnType **fnPointer = f.template target<fnType *>();
    return (size_t)*fnPointer;
}

TEST(EngineTest, Clock)
{
    Engine engine;
    ASSERT_EQ(engine.m_clock, Clock::instance().get());
}

TEST(EngineTest, Clear)
{
    Engine engine;

    auto target1 = std::make_shared<Target>();
    auto target2 = std::make_shared<Target>();
    engine.setTargets({ target1, target2 });

    auto broadcast1 = std::make_shared<Broadcast>("", "");
    auto broadcast2 = std::make_shared<Broadcast>("", "");
    engine.setBroadcasts({ broadcast1, broadcast2 });

    auto section = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section, registerBlocks);
    EXPECT_CALL(*section, onInit);
    engine.registerSection(section);

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
    ASSERT_TRUE(engine.registeredSections().empty());

    AddRemoveMonitorMock removeMonitorMock;
    auto handler = std::bind(&AddRemoveMonitorMock::monitorRemoved, &removeMonitorMock, std::placeholders::_1, std::placeholders::_2);
    engine.setRemoveMonitorHandler(std::function<void(Monitor *, IMonitorHandler *)>(handler));
    engine.setMonitors({ monitor1, monitor2, monitor3, monitor4 });

    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor1.get(), &iface1));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor2.get(), nullptr));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor3.get(), &iface3));
    EXPECT_CALL(removeMonitorMock, monitorRemoved(monitor4.get(), &iface4));
    engine.clear();
    ASSERT_TRUE(engine.monitors().empty());
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

    auto section = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section, registerBlocks);
    EXPECT_CALL(*section, onInit);
    engine.registerSection(section);
    engine.addCompileFunction(section.get(), m1->opcode(), [](Compiler *compiler) { compiler->addConstValue(5.4); });
    engine.addCompileFunction(section.get(), m2->opcode(), [](Compiler *compiler) { compiler->addConstValue("test"); });

    engine.addMonitorNameFunction(section.get(), m1->opcode(), [](Block *block) -> const std::string & {
        static const std::string testStr = "test";
        return testStr;
    });

    engine.addMonitorNameFunction(section.get(), m2->opcode(), [](Block *block) -> const std::string & { return block->opcode(); });

    engine.addMonitorChangeFunction(section.get(), m1->opcode(), [](Block *block, const Value &newValue) { std::cout << "change 1!" << std::endl; });
    engine.addMonitorChangeFunction(section.get(), m2->opcode(), [](Block *block, const Value &newValue) { std::cout << "change 2!" << std::endl; });

    // Compile the monitor blocks
    engine.compile();
    auto script1 = m1->script();
    auto script2 = m2->script();
    auto script3 = m3->script();
    ASSERT_TRUE(script1 && script2 && !script3);

    ASSERT_EQ(script1->bytecodeVector(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(script1->target(), stage.get());
    ASSERT_EQ(script1->topBlock(), m1->block());

    ASSERT_EQ(script2->bytecodeVector(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(script2->target(), sprite.get());
    ASSERT_EQ(script2->topBlock(), m2->block());

    ASSERT_EQ(m1->blockSection(), section);
    ASSERT_EQ(m2->blockSection(), section);
    ASSERT_FALSE(m3->blockSection());

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
    EXPECT_CALL(iface2, onValueChanged(_)).WillOnce(WithArgs<0>(Invoke([](const VirtualMachine *vm) {
        ASSERT_EQ(vm->registerCount(), 1);
        ASSERT_EQ(vm->getInput(0, 1)->toString(), "test");
        ASSERT_FALSE(vm->atEnd()); // the script shouldn't end because that would spam the console with leak warnings
    })));
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
    EXPECT_CALL(*player1, load).WillOnce(Return(true));
    EXPECT_CALL(*player2, load).WillOnce(Return(true));
    EXPECT_CALL(*player3, load).WillOnce(Return(true));
    EXPECT_CALL(*player1, setVolume).Times(2);
    EXPECT_CALL(*player2, setVolume).Times(2);
    EXPECT_CALL(*player3, setVolume).Times(2);
    Project p("sounds.sb3");
    ASSERT_TRUE(p.load());

    auto engine = p.engine();

    EXPECT_CALL(*player1, isPlaying()).WillOnce(Return(false));
    EXPECT_CALL(*player2, isPlaying()).WillOnce(Return(true));
    EXPECT_CALL(*player3, isPlaying()).WillOnce(Return(true));
    EXPECT_CALL(*player2, stop());
    EXPECT_CALL(*player3, stop());
    engine->stopSounds();

    SoundPrivate::audioOutput = nullptr;
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

TEST(EngineTest, FpsProject)
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
    engine->setRedrawHandler(std::function<void()>(handler));
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
}

TEST(EngineTest, TurboModeEnabled)
{
    Engine engine;
    ASSERT_FALSE(engine.turboModeEnabled());

    engine.setTurboModeEnabled(true);
    ASSERT_TRUE(engine.turboModeEnabled());

    engine.setTurboModeEnabled(false);
    ASSERT_FALSE(engine.turboModeEnabled());
}

TEST(EngineTest, ExecutionOrder)
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

    ASSERT_EQ((*list)[0].toString(), "Sprite2");
    ASSERT_EQ((*list)[1].toString(), "Sprite3");
    ASSERT_EQ((*list)[2].toString(), "Sprite1");
    ASSERT_EQ((*list)[3].toString(), "Stage");
    ASSERT_EQ((*list)[4].toString(), "Sprite1 1");
    ASSERT_EQ((*list)[5].toString(), "Sprite1 2");
    ASSERT_EQ((*list)[6].toString(), "Sprite1 3");
    ASSERT_EQ((*list)[7].toString(), "Sprite2 msg");
    ASSERT_EQ((*list)[8].toString(), "Sprite3 msg");
    ASSERT_EQ((*list)[9].toString(), "Sprite1 1 msg");
    ASSERT_EQ((*list)[10].toString(), "Sprite1 2 msg");
    ASSERT_EQ((*list)[11].toString(), "Sprite1 3 msg");
    ASSERT_EQ((*list)[12].toString(), "Stage msg");
}

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
    ASSERT_EQ(engine.mouseX(), -128.038);
}

TEST(EngineTest, MouseY)
{
    Engine engine;
    ASSERT_EQ(engine.mouseY(), 0);

    engine.setMouseY(179.9258);
    ASSERT_EQ(engine.mouseY(), 179.9258);
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

TEST(EngineTest, Sections)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks(&engine));
    EXPECT_CALL(*section1, onInit(&engine));
    engine.registerSection(section1);

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks(&engine));
    EXPECT_CALL(*section2, onInit(&engine));
    engine.registerSection(section2);

    EXPECT_CALL(*section1, name()).WillOnce(Return("test"));
    EXPECT_CALL(*section1, registerBlocks).Times(0);
    EXPECT_CALL(*section1, onInit).Times(0);
    engine.registerSection(section1); // register existing section

    ASSERT_EQ(engine.registeredSections().size(), 2);
    if (engine.registeredSections()[0] == section1)
        ASSERT_EQ(engine.registeredSections()[1].get(), section2.get());
    else {
        ASSERT_EQ(engine.registeredSections()[0].get(), section2.get());
        ASSERT_EQ(engine.registeredSections()[1].get(), section1.get());
    }
}

unsigned int testFunction1(VirtualMachine *)
{
    return 0;
}

unsigned int testFunction2(VirtualMachine *)
{
    return 0;
}

TEST(EngineTest, Functions)
{
    Engine engine;

    ASSERT_EQ(engine.functionIndex(&testFunction1), 0);
    ASSERT_EQ(engine.functionIndex(&testFunction2), 1);
    ASSERT_EQ(engine.functionIndex(&testFunction1), 0);
    ASSERT_EQ(engine.functionIndex(&testFunction2), 1);
}

void compileTest1(Compiler *)
{
}

void compileTest2(Compiler *)
{
}

TEST(EngineTest, CompileFunctions)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(section2.get(), "test2", &compileTest2);
    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(&section3, "test1", &compileTest1);

    ASSERT_EQ(container1->resolveBlockCompileFunc("test1"), &compileTest1);
    ASSERT_EQ(container1->resolveBlockCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test2"), &compileTest2);
}

TEST(EngineTest, HatPredicateCompileFunctions)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addHatPredicateCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addHatPredicateCompileFunction(section2.get(), "test2", &compileTest2);
    engine.addHatPredicateCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addHatPredicateCompileFunction(&section3, "test1", &compileTest1);

    ASSERT_EQ(container1->resolveHatPredicateCompileFunc("test1"), &compileTest1);
    ASSERT_EQ(container1->resolveHatPredicateCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveHatPredicateCompileFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveHatPredicateCompileFunc("test2"), &compileTest2);
}

TEST(EngineTest, MonitorNameFunctions)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    MonitorNameFunc f1 = [](Block *) -> const std::string & {
        static const std::string ret;
        return ret;
    };

    MonitorNameFunc f2 = [](Block *) -> const std::string & {
        static const std::string ret;
        return ret;
    };

    engine.addMonitorNameFunction(section1.get(), "test1", f1);
    engine.addMonitorNameFunction(section2.get(), "test2", f2);
    engine.addMonitorNameFunction(section1.get(), "test1", f1);
    engine.addMonitorNameFunction(&section3, "test1", f1);

    ASSERT_EQ(container1->resolveMonitorNameFunc("test1"), f1);
    ASSERT_EQ(container1->resolveMonitorNameFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveMonitorNameFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveMonitorNameFunc("test2"), f2);
}

TEST(EngineTest, MonitorChangeFunctions)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    MonitorChangeFunc f1 = [](Block *, const Value &) {};
    MonitorChangeFunc f2 = [](Block *, const Value &) {};

    engine.addMonitorChangeFunction(section1.get(), "test1", f1);
    engine.addMonitorChangeFunction(section2.get(), "test2", f2);
    engine.addMonitorChangeFunction(section1.get(), "test1", f1);
    engine.addMonitorChangeFunction(&section3, "test1", f1);

    ASSERT_EQ(container1->resolveMonitorChangeFunc("test1"), f1);
    ASSERT_EQ(container1->resolveMonitorChangeFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveMonitorChangeFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveMonitorChangeFunc("test2"), f2);
}

TEST(EngineTest, HatBlocks)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addHatBlock(section1.get(), "test1");
    engine.addHatBlock(section2.get(), "test2");
    engine.addHatBlock(section1.get(), "test1");
    engine.addHatBlock(&section3, "test1");

    ASSERT_NE(container1->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_EQ(container1->resolveBlockCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_NE(container2->resolveBlockCompileFunc("test2"), nullptr);
}

TEST(EngineTest, Inputs)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addInput(section1.get(), "VALUE1", 1);
    engine.addInput(section2.get(), "VALUE2", 2);
    engine.addInput(section1.get(), "VALUE1", 3); // change ID of existing input
    engine.addInput(&section3, "VALUE3", 4);

    ASSERT_EQ(container1->resolveInput("VALUE1"), 3);
    ASSERT_EQ(container1->resolveInput("VALUE2"), -1);
    ASSERT_EQ(container1->resolveInput("VALUE3"), -1);
    ASSERT_EQ(container2->resolveInput("VALUE1"), -1);
    ASSERT_EQ(container2->resolveInput("VALUE2"), 2);
    ASSERT_EQ(container2->resolveInput("VALUE3"), -1);
}

TEST(EngineTest, Fields)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addField(section1.get(), "VALUE1", 1);
    engine.addField(section2.get(), "VALUE2", 2);
    engine.addField(section1.get(), "VALUE1", 3); // change ID of existing field
    engine.addField(&section3, "VALUE3", 4);

    ASSERT_EQ(container1->resolveField("VALUE1"), 3);
    ASSERT_EQ(container1->resolveField("VALUE2"), -1);
    ASSERT_EQ(container1->resolveField("VALUE3"), -1);
    ASSERT_EQ(container2->resolveField("VALUE1"), -1);
    ASSERT_EQ(container2->resolveField("VALUE2"), 2);
    ASSERT_EQ(container2->resolveField("VALUE3"), -1);
}

TEST(EngineTest, FieldValues)
{
    Engine engine;

    auto section1 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section1, registerBlocks);
    EXPECT_CALL(*section1, onInit);
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<BlockSectionMock>();
    EXPECT_CALL(*section2, registerBlocks);
    EXPECT_CALL(*section2, onInit);
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    BlockSectionMock section3;

    engine.addFieldValue(section1.get(), "value1", 1);
    engine.addFieldValue(section2.get(), "value2", 2);
    engine.addFieldValue(section1.get(), "value1", 3); // change ID of existing field value
    engine.addFieldValue(&section3, "value3", 4);

    ASSERT_EQ(container1->resolveFieldValue("value1"), 3);
    ASSERT_EQ(container1->resolveFieldValue("value2"), -1);
    ASSERT_EQ(container1->resolveFieldValue("value3"), -1);
    ASSERT_EQ(container2->resolveFieldValue("value1"), -1);
    ASSERT_EQ(container2->resolveFieldValue("value2"), 2);
    ASSERT_EQ(container2->resolveFieldValue("value3"), -1);
}

TEST(EngineTest, Broadcasts)
{
    Engine engine;
    ASSERT_TRUE(engine.broadcasts().empty());

    auto b1 = std::make_shared<Broadcast>("a", "message1");
    auto b2 = std::make_shared<Broadcast>("b", "message2");
    auto b3 = std::make_shared<Broadcast>("c", "Test");
    engine.setBroadcasts({ b1, b2, b3 });

    ASSERT_EQ(engine.broadcasts(), std::vector<std::shared_ptr<Broadcast>>({ b1, b2, b3 }));
    ASSERT_EQ(engine.broadcastAt(0), b1);
    ASSERT_EQ(engine.broadcastAt(1), b2);
    ASSERT_EQ(engine.broadcastAt(2), b3);
    ASSERT_EQ(engine.broadcastAt(3), nullptr);
    ASSERT_EQ(engine.broadcastAt(-1), nullptr);

    ASSERT_EQ(engine.findBroadcast("invalid"), -1);
    ASSERT_EQ(engine.findBroadcast("message1"), 0);
    ASSERT_EQ(engine.findBroadcast("message2"), 1);
    ASSERT_EQ(engine.findBroadcast("Test"), 2);

    ASSERT_EQ(engine.findBroadcastById("d"), -1);
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

    auto t1 = std::make_shared<Target>();
    t1->setName("Sprite1");
    auto t2 = std::make_shared<Target>();
    auto block1 = std::make_shared<Block>("", "");
    auto block2 = std::make_shared<Block>("", "");
    t2->setName("Sprite2");
    t2->addBlock(block1);
    t2->addBlock(block2);
    auto t3 = std::make_shared<Stage>();
    t3->setName("Stage");
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

    auto t4 = std::make_shared<Target>();
    t4->setName("Stage");
    engine.setTargets({ t1, t2, t4 });
    ASSERT_EQ(engine.findTarget("Stage"), 2);
    ASSERT_EQ(engine.findTarget("_stage_"), -1);

    engine.setTargets({ t1, t2, t3, t4 });
    ASSERT_EQ(engine.findTarget("Stage"), 3);
    ASSERT_EQ(engine.findTarget("_stage_"), 2);

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

TEST(EngineTest, MoveSpriteToFront)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveSpriteToFront(sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 5);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);

    for (int i = 0; i < 2; i++) {
        engine.moveSpriteToFront(sprites[0]);
        ASSERT_EQ(sprites[0]->layerOrder(), 5);
        ASSERT_EQ(sprites[1]->layerOrder(), 3);
        ASSERT_EQ(sprites[2]->layerOrder(), 4);
        ASSERT_EQ(sprites[3]->layerOrder(), 2);
        ASSERT_EQ(sprites[4]->layerOrder(), 1);
    }

    auto stage = std::make_shared<Stage>();
    stage->setLayerOrder(0);
    auto sprite = std::make_shared<Sprite>();
    sprite->setLayerOrder(1);

    engine.setTargets({ stage, sprite });
    engine.moveSpriteToFront(sprite.get());
    ASSERT_EQ(sprite->layerOrder(), 1);
}

TEST(EngineTest, MoveSpriteToBack)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveSpriteToBack(sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);

    for (int i = 0; i < 1; i++) {
        engine.moveSpriteToBack(sprites[1]);
        ASSERT_EQ(sprites[0]->layerOrder(), 3);
        ASSERT_EQ(sprites[1]->layerOrder(), 1);
        ASSERT_EQ(sprites[2]->layerOrder(), 2);
        ASSERT_EQ(sprites[3]->layerOrder(), 5);
        ASSERT_EQ(sprites[4]->layerOrder(), 4);
    }

    auto stage = std::make_shared<Stage>();
    stage->setLayerOrder(0);
    auto sprite = std::make_shared<Sprite>();
    sprite->setLayerOrder(1);

    engine.setTargets({ stage, sprite });
    engine.moveSpriteToBack(sprite.get());
    ASSERT_EQ(sprite->layerOrder(), 1);
}

TEST(EngineTest, MoveSpriteForwardLayers)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveSpriteForwardLayers(sprites[4], 2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);

    engine.moveSpriteForwardLayers(sprites[4], 2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 5);

    engine.moveSpriteForwardLayers(sprites[4], -3);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);

    engine.moveSpriteForwardLayers(sprites[2], -3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
}

TEST(EngineTest, MoveSpriteBackwardLayers)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveSpriteBackwardLayers(sprites[4], -2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);

    engine.moveSpriteBackwardLayers(sprites[4], -2);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 4);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 3);
    ASSERT_EQ(sprites[4]->layerOrder(), 5);

    engine.moveSpriteBackwardLayers(sprites[4], 3);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 2);

    engine.moveSpriteBackwardLayers(sprites[2], 3);
    ASSERT_EQ(sprites[0]->layerOrder(), 2);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 1);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);
}

TEST(EngineTest, MoveSpriteBehindOther)
{
    Engine engine;
    std::vector<Sprite *> sprites;
    createTargets(&engine, sprites);

    engine.moveSpriteBehindOther(sprites[4], sprites[3]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 2);
    ASSERT_EQ(sprites[3]->layerOrder(), 4);
    ASSERT_EQ(sprites[4]->layerOrder(), 3);

    engine.moveSpriteBehindOther(sprites[3], sprites[2]);
    ASSERT_EQ(sprites[0]->layerOrder(), 1);
    ASSERT_EQ(sprites[1]->layerOrder(), 5);
    ASSERT_EQ(sprites[2]->layerOrder(), 3);
    ASSERT_EQ(sprites[3]->layerOrder(), 2);
    ASSERT_EQ(sprites[4]->layerOrder(), 4);

    engine.moveSpriteBehindOther(sprites[4], sprites[0]);
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
    engine.setAddMonitorHandler(std::function<void(Monitor *)>(handler));
    engine.setMonitors({});

    EXPECT_CALL(addMonitorMock, monitorAdded(m1.get()));
    EXPECT_CALL(addMonitorMock, monitorAdded(m2.get()));
    EXPECT_CALL(addMonitorMock, monitorAdded(m3.get()));
    engine.setMonitors({ m1, m2, m3 });
}

TEST(EngineTest, CreateMissingMonitors)
{
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
        auto block = monitor->block();
        ASSERT_EQ(monitor->id(), var->id());
        ASSERT_EQ(monitor->opcode(), "data_variable");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_FALSE(monitor->visible());
        ASSERT_EQ(block->fields().size(), 1);

        auto field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->fieldId(), VariableBlocks::VARIABLE);
        ASSERT_EQ(field->value(), var->name());
        ASSERT_EQ(field->valuePtr(), var);

        if (var->target()->isStage())
            ASSERT_EQ(monitor->sprite(), nullptr);
        else
            ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(var->target()));
    };

    auto checkListMonitor = [](std::shared_ptr<Monitor> monitor, std::shared_ptr<List> list) {
        auto block = monitor->block();
        ASSERT_EQ(monitor->id(), list->id());
        ASSERT_EQ(monitor->opcode(), "data_listcontents");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_FALSE(monitor->visible());
        ASSERT_EQ(block->fields().size(), 1);

        auto field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "LIST");
        ASSERT_EQ(field->fieldId(), ListBlocks::LIST);
        ASSERT_EQ(field->value(), list->name());
        ASSERT_EQ(field->valuePtr(), list);

        if (list->target()->isStage())
            ASSERT_EQ(monitor->sprite(), nullptr);
        else
            ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(list->target()));
    };

    // Set monitors after setting targets
    {
        Engine engine;
        engine.setTargets({ target1, target2 });
        engine.setMonitors({ m1, m2, m3 });

        const auto &monitors = engine.monitors();
        ASSERT_EQ(monitors.size(), 7);
        ASSERT_EQ(monitors[0], m1);
        ASSERT_EQ(monitors[1], m2);
        ASSERT_EQ(monitors[2], m3);
        checkVariableMonitor(monitors[3], var2);
        checkListMonitor(monitors[4], list1);
        checkVariableMonitor(monitors[5], var4);
        checkVariableMonitor(monitors[6], var5);
    }

    // Set monitors before setting targets
    {
        Engine engine;
        engine.setMonitors({ m1, m2, m3 });
        engine.setTargets({ target1, target2 });

        const auto &monitors = engine.monitors();
        ASSERT_EQ(monitors.size(), 7);
        ASSERT_EQ(monitors[0], m1);
        ASSERT_EQ(monitors[1], m2);
        ASSERT_EQ(monitors[2], m3);
        checkVariableMonitor(monitors[3], var2);
        checkListMonitor(monitors[4], list1);
        checkVariableMonitor(monitors[5], var4);
        checkVariableMonitor(monitors[6], var5);
    }

    {
        Engine engine;
        AddRemoveMonitorMock addMonitorMock;
        auto handler = std::bind(&AddRemoveMonitorMock::monitorAdded, &addMonitorMock, std::placeholders::_1);
        engine.setAddMonitorHandler(std::function<void(Monitor *)>(handler));

        EXPECT_CALL(addMonitorMock, monitorAdded(m1.get()));
        EXPECT_CALL(addMonitorMock, monitorAdded(m2.get()));
        EXPECT_CALL(addMonitorMock, monitorAdded(m3.get()));
        engine.setMonitors({ m1, m2, m3 });

        Monitor *m4, *m5, *m6, *m7;
        EXPECT_CALL(addMonitorMock, monitorAdded(_)).WillOnce(SaveArg<0>(&m4)).WillOnce(SaveArg<0>(&m5)).WillOnce(SaveArg<0>(&m6)).WillOnce(SaveArg<0>(&m7));
        engine.setTargets({ target1, target2 });

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

TEST(EngineTest, QuestionAsked)
{
    Engine engine;
    ASSERT_EQ(engine.questionAsked(), nullptr);

    static const std::function<void(const std::string &)> f = &questionFunction;
    engine.setQuestionAsked(&questionFunction);
    ASSERT_EQ(getAddress(engine.questionAsked()), getAddress(f));
}

TEST(EngineTest, QuestionAnswered)
{
    Engine engine;
    ASSERT_EQ(engine.questionAnswered(), nullptr);

    static const std::function<void(const std::string &)> f = &questionFunction;
    engine.setQuestionAnswered(f);
    ASSERT_EQ(getAddress(engine.questionAnswered()), getAddress(f));
}

TEST(EngineTest, Clones)
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
            ASSERT_EQ((*list)[i].toInt(), 1);
        else
            ASSERT_EQ((*list)[i].toInt(), 2);
    }

    ASSERT_LIST(stage, "log2");
    list = GET_LIST(stage, "log2");

    for (int i = 0; i < list->size(); i++) {
        if (i < 10)
            ASSERT_EQ((*list)[i].toInt(), 1);
        else
            ASSERT_EQ((*list)[i].toString(), "12");
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

TEST(EngineTest, StopOtherScriptsInSprite)
{
    Project p("stop_other_scripts_in_sprite.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

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

TEST(EngineTest, EdgeActivatedHats)
{
    Project p("when_greater_than.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = std::static_pointer_cast<Engine>(p.engine());

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "test");
    auto var = GET_VAR(stage, "test");
    ASSERT_EQ(var->value().toInt(), 0);

    TimerMock timer;
    engine->setTimer(&timer);

    EXPECT_CALL(timer, value()).WillOnce(Return(5));
    engine->step();
    ASSERT_EQ(var->value().toInt(), 0);

    EXPECT_CALL(timer, value()).WillOnce(Return(10));
    engine->step();
    ASSERT_EQ(var->value().toInt(), 0);

    EXPECT_CALL(timer, value()).WillOnce(Return(10.2));
    engine->step();
    ASSERT_EQ(var->value().toInt(), 1);
}

TEST(EngineTest, UserAgent)
{
    Engine engine;
    ASSERT_TRUE(engine.userAgent().empty());

    engine.setUserAgent("test");
    ASSERT_EQ(engine.userAgent(), "test");
}

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

TEST(EngineTest, NoStopWhenCallingRunningBroadcastFromCustomBlock)
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
}

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

    engine->broadcast(0);
    engine->step();

    ASSERT_VAR(stage, "test");
    ASSERT_FALSE(GET_VAR(stage, "test")->value().toBool());

    GET_VAR(stage, "test")->setValue(true);
    engine->broadcast(0);
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
