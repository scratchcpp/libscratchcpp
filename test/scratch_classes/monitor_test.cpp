#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/rect.h>
#include <scratch/monitor_p.h>
#include <randomgeneratormock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

static const int PADDING = 5;
static const int SCREEN_WIDTH = 400;
static const int SCREEN_HEIGHT = 300;
static const int SCREEN_EDGE_BUFFER = 40;

TEST(MonitorTest, Constructors)
{
    Monitor monitor("abc", "test");
    ASSERT_EQ(monitor.id(), "abc");
    ASSERT_EQ(monitor.opcode(), "test");
    ASSERT_TRUE(monitor.block());
    ASSERT_EQ(monitor.block()->opcode(), "test");
}

TEST(MonitorTest, Id)
{
    Monitor monitor("abc", "");

    monitor.setId("def");
    ASSERT_EQ(monitor.id(), "def");
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

    monitor.setX(-78);
    ASSERT_EQ(monitor.x(), -78);
}

TEST(MonitorTest, Y)
{
    Monitor monitor("", "");
    ASSERT_EQ(monitor.y(), 0);

    monitor.setY(150);
    ASSERT_EQ(monitor.y(), 150);
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

TEST(MonitorTest, GetInitialPosition)
{
    std::vector<std::shared_ptr<Monitor>> monitors;
    const int width = 100;
    const int height = 200;

    auto monitor1 = std::make_shared<Monitor>("", "");
    monitor1->setWidth(PADDING);
    monitor1->setHeight(height);
    monitor1->setX(100);
    monitor1->setY(0);
    monitors.push_back(monitor1);

    auto monitor2 = std::make_shared<Monitor>("", "");
    monitor2->setWidth(width);
    monitor2->setHeight(height + PADDING - 100);
    monitor2->setX(0);
    monitor2->setY(100);
    monitors.push_back(monitor2);

    RandomGeneratorMock rng;
    MonitorPrivate::rng = &rng;

    EXPECT_CALL(rng, randintDouble(0, SCREEN_WIDTH / 2.0)).WillOnce(Return(SCREEN_WIDTH / 4.5));
    EXPECT_CALL(rng, randintDouble(0, SCREEN_HEIGHT - SCREEN_EDGE_BUFFER)).WillOnce(Return(SCREEN_HEIGHT - SCREEN_EDGE_BUFFER * 2.3));
    Rect rect = Monitor::getInitialPosition(monitors, width, height);
    ASSERT_EQ(rect.left(), std::ceil(SCREEN_WIDTH / 4.5));
    ASSERT_EQ(rect.top(), std::ceil(SCREEN_HEIGHT - SCREEN_EDGE_BUFFER * 2.3));
    ASSERT_EQ(rect.width(), width);
    ASSERT_EQ(rect.height(), height);

    MonitorPrivate::rng = nullptr;
}
