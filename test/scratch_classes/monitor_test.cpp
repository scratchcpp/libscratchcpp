#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
#include <scratchcpp/sprite.h>

#include "../common.h"

using namespace libscratchcpp;

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
