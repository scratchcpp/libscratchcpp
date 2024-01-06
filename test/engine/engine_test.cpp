#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>
#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/keyevent.h>
#include <scratch/sound_p.h>
#include <timermock.h>
#include <clockmock.h>
#include <audioplayerfactorymock.h>
#include <audioplayermock.h>
#include <thread>

#include "../common.h"
#include "testsection.h"
#include "engine/internal/engine.h"
#include "engine/internal/clock.h"

using namespace libscratchcpp;

using ::testing::Return;

// NOTE: resolveIds() and compile() are tested in load_project_test

class RedrawMock
{
    public:
        MOCK_METHOD(void, redraw, ());
};

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

    auto section = std::make_shared<TestSection>();
    engine.registerSection(section);

    engine.clear();
    ASSERT_TRUE(engine.targets().empty());
    ASSERT_TRUE(engine.broadcasts().empty());
    ASSERT_TRUE(engine.registeredSections().empty());
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
    AudioPlayerFactoryMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::playerFactory = &factory;
    EXPECT_CALL(factory, create()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
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

    SoundPrivate::playerFactory = nullptr;
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

    p.run();

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
    p.run();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 1);
    engine->setKeyState("space", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 1);
    engine->setKeyState("right arrow", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "right_arrow_pressed");
    ASSERT_EQ(GET_VAR(stage, "right_arrow_pressed")->value().toInt(), 2);
    engine->setKeyState("right arrow", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "any_key_pressed");
    ASSERT_EQ(GET_VAR(stage, "any_key_pressed")->value().toInt(), 4);
    engine->setAnyKeyPressed(false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "a_pressed");
    ASSERT_EQ(GET_VAR(stage, "a_pressed")->value().toInt(), 1);
    engine->setKeyState("a", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 1);
    engine->setKeyState("x", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "4_pressed");
    ASSERT_EQ(GET_VAR(stage, "4_pressed")->value().toInt(), 1);
    engine->setKeyState("4", false);
    p.run();

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
    p.run();

    ASSERT_VAR(stage, "space_pressed");
    ASSERT_EQ(GET_VAR(stage, "space_pressed")->value().toInt(), 2);
    ASSERT_VAR(stage, "x_pressed");
    ASSERT_EQ(GET_VAR(stage, "x_pressed")->value().toInt(), 2);
    engine->setKeyState("space", false);
    engine->setKeyState("x", false);
    p.run();

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

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);

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

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(section2.get(), "test2", &compileTest2);
    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(&section3, "test1", &compileTest1);

    ASSERT_EQ(container1->resolveBlockCompileFunc("test1"), &compileTest1);
    ASSERT_EQ(container1->resolveBlockCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test2"), &compileTest2);
}

TEST(EngineTest, HatBlocks)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

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

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

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

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

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

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addFieldValue(section1.get(), "value1", 1);
    engine.addFieldValue(section2.get(), "value2", 2);
    engine.addFieldValue(section1.get(), "value1", 3); // change ID of existing field
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
    engine->run();

    ASSERT_VAR(stage, "test");
    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 1);

    engine->run();
    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 1);

    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), true);
    engine->setKeyState(KeyEvent(KeyEvent::Type::Space), false);
    engine->run();

    ASSERT_EQ(GET_VAR(stage, "test")->value().toInt(), 2);
}
