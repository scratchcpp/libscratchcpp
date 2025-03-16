#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/script.h>
#include <monitorhandlermock.h>
#include <randomgeneratormock.h>

#include "../common.h"
#include "testextension.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;

static const int PADDING = 5;
static const int SCREEN_WIDTH = 400;
static const int SCREEN_HEIGHT = 300;

TEST(MonitorTest, Constructors)
{
    Monitor monitor("abc", "test");
    ASSERT_EQ(monitor.id(), "abc");
    ASSERT_EQ(monitor.opcode(), "test");
    ASSERT_TRUE(monitor.block());
    ASSERT_TRUE(monitor.block()->isMonitorBlock());
    ASSERT_EQ(monitor.block()->opcode(), "test");
    ASSERT_TRUE(monitor.needsAutoPosition());
}

TEST(MonitorTest, Id)
{
    Monitor monitor("abc", "");

    monitor.setId("def");
    ASSERT_EQ(monitor.id(), "def");
}

TEST(MonitorTest, Interface)
{
    Monitor monitor("", "");
    MonitorHandlerMock iface;

    EXPECT_CALL(iface, init(&monitor));
    monitor.setInterface(&iface);
}

TEST(MonitorTest, Name)
{
    Monitor monitor("", "");
    ASSERT_TRUE(monitor.name().empty());

    monitor.setName("test");
    ASSERT_EQ(monitor.name(), "test");
}

TEST(MonitorTest, Mode)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.mode(), Monitor::Mode::Default);

    monitor.setMode(Monitor::Mode::Large);
    ASSERT_EQ(monitor.mode(), Monitor::Mode::Large);

    monitor.setMode(Monitor::Mode::Slider);
    ASSERT_EQ(monitor.mode(), Monitor::Mode::Slider);

    monitor.setMode(Monitor::Mode::List);
    ASSERT_EQ(monitor.mode(), Monitor::Mode::List);

    monitor.setMode(Monitor::Mode::Default);
    ASSERT_EQ(monitor.mode(), Monitor::Mode::Default);
}

TEST(MonitorTest, Script)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.script(), nullptr);

    auto script = std::make_shared<Script>(nullptr, nullptr, nullptr);
    monitor.setScript(script);
    ASSERT_EQ(monitor.script(), script);
}

TEST(MonitorTest, Extension)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.extension(), nullptr);

    auto extension = std::make_shared<TestExtension>();
    monitor.setExtension(extension.get());
    ASSERT_EQ(monitor.extension(), extension.get());
}

TEST(MonitorTest, Sprite)
{
    Monitor monitor("", "");
    auto block = monitor.block();
    ASSERT_EQ(monitor.sprite(), nullptr);
    ASSERT_EQ(block->target(), nullptr);

    Sprite sprite;
    monitor.setSprite(&sprite);
    ASSERT_EQ(monitor.sprite(), &sprite);
    ASSERT_EQ(block->target(), &sprite);

    monitor.setSprite(nullptr);
    ASSERT_EQ(monitor.sprite(), nullptr);
    ASSERT_EQ(block->target(), nullptr);
}

TEST(MonitorTest, UpdateValue)
{
    Monitor monitor("", "");
    monitor.updateValue("test");

    MonitorHandlerMock handler;
    EXPECT_CALL(handler, init);
    monitor.setInterface(&handler);

    Value v = 123;
    EXPECT_CALL(handler, onValueChanged(v));
    monitor.updateValue(v);
}

TEST(MonitorTest, ChangeValue)
{
    Monitor monitor("", "test_block");
    monitor.changeValue(6.2);

    MonitorHandlerMock handler;
    EXPECT_CALL(handler, init);
    monitor.setInterface(&handler);

    Value v = "test";
    EXPECT_CALL(handler, onValueChanged(v));
    monitor.changeValue(v);

    monitor.setValueChangeFunction([](Block *block, const Value &newValue) { std::cout << block->opcode() + " " + newValue.toString() << std::endl; });
    EXPECT_CALL(handler, onValueChanged(v));
    testing::internal::CaptureStdout();
    monitor.changeValue(v);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "test_block test\n");
}

TEST(MonitorTest, Width)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.width(), 0);

    monitor.setWidth(64);
    ASSERT_EQ(monitor.width(), 64);
}

TEST(MonitorTest, Height)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.height(), 0);

    monitor.setHeight(51);
    ASSERT_EQ(monitor.height(), 51);
}

TEST(MonitorTest, X)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.x(), 0);

    MonitorHandlerMock handler;
    EXPECT_CALL(handler, init);
    monitor.setInterface(&handler);

    EXPECT_CALL(handler, onXChanged(-78));
    monitor.setX(-78);
    ASSERT_EQ(monitor.x(), -78);
    ASSERT_FALSE(monitor.needsAutoPosition());
}

TEST(MonitorTest, Y)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.y(), 0);

    MonitorHandlerMock handler;
    EXPECT_CALL(handler, init);
    monitor.setInterface(&handler);

    EXPECT_CALL(handler, onYChanged(150));
    monitor.setY(150);
    ASSERT_EQ(monitor.y(), 150);
    ASSERT_FALSE(monitor.needsAutoPosition());
}

TEST(MonitorTest, Visible)
{
    Monitor monitor("", "");
    ASSERT_TRUE(monitor.visible());

    monitor.setVisible(false);
    ASSERT_FALSE(monitor.visible());

    monitor.setVisible(false);
    ASSERT_FALSE(monitor.visible());

    monitor.setVisible(true);
    ASSERT_TRUE(monitor.visible());

    MonitorHandlerMock handler;
    EXPECT_CALL(handler, init);
    monitor.setInterface(&handler);

    EXPECT_CALL(handler, onVisibleChanged(true));
    monitor.setVisible(true);
    ASSERT_TRUE(monitor.visible());

    EXPECT_CALL(handler, onVisibleChanged(false));
    monitor.setVisible(false);
    ASSERT_FALSE(monitor.visible());

    EXPECT_CALL(handler, onVisibleChanged(false));
    monitor.setVisible(false);
    ASSERT_FALSE(monitor.visible());
}

TEST(MonitorTest, SliderMin)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.sliderMin(), 0);

    monitor.setSliderMin(-77.05);
    ASSERT_EQ(monitor.sliderMin(), -77.05);
}

TEST(MonitorTest, SliderMax)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.sliderMax(), 0);

    monitor.setSliderMax(54.92);
    ASSERT_EQ(monitor.sliderMax(), 54.92);
}

TEST(MonitorTest, Discrete)
{
    Monitor monitor("", "");
    ASSERT_FALSE(monitor.discrete());

    monitor.setDiscrete(true);
    ASSERT_TRUE(monitor.discrete());

    monitor.setDiscrete(true);
    ASSERT_TRUE(monitor.discrete());

    monitor.setDiscrete(false);
    ASSERT_FALSE(monitor.discrete());
}

TEST(MonitorTest, AutoPosition_LineUpTopLeft)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/875bee35f178411b9149ab766d17b5fb88ddd749/test/unit/reducers/monitor-layout-reducer.test.js#L216-L238
    const int width = 100;
    const int height = 200;

    auto monitor = std::make_shared<Monitor>("", "");
    monitor->setWidth(width);
    monitor->setHeight(height);

    // Add monitors to right and bottom, but there is a space in the top left
    std::vector<std::shared_ptr<Monitor>> monitors = { std::make_shared<Monitor>("", ""), std::make_shared<Monitor>("", ""), monitor };

    monitors[0]->setX(width + 2 * PADDING);
    monitors[0]->setY(0);
    monitors[0]->setWidth(100);
    monitors[0]->setHeight(height);

    monitors[1]->setX(0);
    monitors[1]->setY(height + 2 * PADDING);
    monitors[1]->setWidth(width);
    monitors[1]->setHeight(100);

    // Check that the added monitor appears in the space
    monitor->autoPosition(monitors);
    ASSERT_EQ(monitor->width(), width);
    ASSERT_EQ(monitor->height(), height);
    ASSERT_EQ(monitor->x(), PADDING);
    ASSERT_EQ(monitor->y(), PADDING);
    ASSERT_FALSE(monitor->needsAutoPosition());
}

TEST(MonitorTest, AutoPosition_LineUpLeft)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/875bee35f178411b9149ab766d17b5fb88ddd749/test/unit/reducers/monitor-layout-reducer.test.js#L261-L270
    auto monitor = std::make_shared<Monitor>("", "");
    monitor->setWidth(20);
    monitor->setHeight(20);

    const int monitor1EndY = 60;

    // Add a monitor that takes up the upper left corner
    std::vector<std::shared_ptr<Monitor>> monitors = { std::make_shared<Monitor>("", ""), monitor };
    monitors[0]->setX(0);
    monitors[0]->setY(0);
    monitors[0]->setWidth(100);
    monitors[0]->setHeight(monitor1EndY);

    // Check that added monitor is under it and lines up left
    monitor->autoPosition(monitors);
    ASSERT_EQ(monitor->width(), 20);
    ASSERT_EQ(monitor->height(), 20);
    ASSERT_GE(monitor->y(), monitor1EndY + PADDING);
    ASSERT_FALSE(monitor->needsAutoPosition());
}

TEST(MonitorTest, LineUpTop)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/875bee35f178411b9149ab766d17b5fb88ddd749/test/unit/reducers/monitor-layout-reducer.test.js#L272-L285
    auto monitor = std::make_shared<Monitor>("", "");
    monitor->setWidth(20);
    monitor->setHeight(20);

    const int monitor1EndX = 100;

    // Add a monitor that takes up the whole left side
    std::vector<std::shared_ptr<Monitor>> monitors = { std::make_shared<Monitor>("", ""), monitor };
    monitors[0]->setX(0);
    monitors[0]->setY(0);
    monitors[0]->setWidth(monitor1EndX);
    monitors[0]->setHeight(SCREEN_HEIGHT);

    // Check that added monitor is to the right of it and lines up top
    monitor->autoPosition(monitors);
    ASSERT_EQ(monitor->width(), 20);
    ASSERT_EQ(monitor->height(), 20);
    ASSERT_EQ(monitor->y(), PADDING);
    ASSERT_GE(monitor->x(), monitor1EndX + PADDING);
}

TEST(MonitorTest, AutoPosition_NoRoom)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/875bee35f178411b9149ab766d17b5fb88ddd749/test/unit/reducers/monitor-layout-reducer.test.js#L287-L302
    auto monitor = std::make_shared<Monitor>("", "");
    monitor->setWidth(7);
    monitor->setHeight(8);

    // Add a monitor that takes up the whole screen
    std::vector<std::shared_ptr<Monitor>> monitors = { std::make_shared<Monitor>("", ""), monitor };
    monitors[0]->setX(0);
    monitors[0]->setY(0);
    monitors[0]->setWidth(SCREEN_WIDTH);
    monitors[0]->setHeight(SCREEN_HEIGHT);

    // Check that added monitor exists somewhere (we don't care where)
    monitor->autoPosition(monitors);
    ASSERT_EQ(monitor->width(), 7);
    ASSERT_EQ(monitor->height(), 8);
    ASSERT_FALSE(monitor->needsAutoPosition());
}
