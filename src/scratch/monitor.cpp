// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/imonitorhandler.h>

#include "monitor_p.h"
#include "engine/internal/randomgenerator.h"

using namespace libscratchcpp;

static const int PADDING = 5;
static const int SCREEN_WIDTH = 400;
static const int SCREEN_HEIGHT = 300;
static const int SCREEN_EDGE_BUFFER = 40;

/*! Constructs Monitor. */
Monitor::Monitor(const std::string &id, const std::string &opcode) :
    Entity(id),
    impl(spimpl::make_unique_impl<MonitorPrivate>(opcode))
{
}

/*! Sets the monitor interface. */
void Monitor::setInterface(IMonitorHandler *iface)
{
    impl->iface = iface;

    if (iface)
        iface->init(this);
}

/*
 * Returns the name of this monitor.
 * \note Add the monitor to a project to initialize this property.
 */
const std::string &Monitor::name() const
{
    return impl->name;
}

/*! Sets the name of this monitor. */
void Monitor::setName(const std::string &name)
{
    impl->name = name;
}

/*! Returns the monitor's mode. */
Monitor::Mode Monitor::mode() const
{
    return impl->mode;
}

/*! Sets the monitor's mode. */
void Monitor::setMode(Mode mode)
{
    impl->mode = mode;
}

/*! Returns the block used to get the monitor's value. */
std::shared_ptr<Block> Monitor::block() const
{
    return impl->block;
}

/*! Returns the script which holds information about the monitor's compiled block. */
std::shared_ptr<Script> Monitor::script() const
{
    return impl->script;
}

/*! Sets the script which holds information about the monitor's compiled block. */
void Monitor::setScript(std::shared_ptr<Script> script)
{
    impl->script = script;
}

/*
 * Returns the block section of this monitor.
 * \note This will return nullptr by default. Add the monitor to a project to set this property.
 */
std::shared_ptr<IBlockSection> Monitor::blockSection() const
{
    return impl->blockSection;
}

/*! Sets the block section of this monitor. */
void Monitor::setBlockSection(std::shared_ptr<IBlockSection> blockSection)
{
    impl->blockSection = blockSection;
}

/*! Convenience method which calls block()->target(). */
Sprite *Monitor::sprite() const
{
    return dynamic_cast<Sprite *>(impl->block->target());
}

/*! Convenience method which calls block()->setTarget(). */
void Monitor::setSprite(Sprite *sprite)
{
    impl->block->setTarget(sprite);
}

/*! Convenience method which calls block()->opcode(). */
const std::string &Monitor::opcode() const
{
    return impl->block->opcode();
}

/*!
 * Notifies the monitor's interface about value change.
 * The interaface is supposed to read it from the VirtualMachine.
 */
void Monitor::updateValue(const VirtualMachine *vm)
{
    if (impl->iface)
        impl->iface->onValueChanged(vm);
}

/*!
 * Sets the function which is called to change the monitor's value.
 * \see changeValue()
 */
void Monitor::setValueChangeFunction(MonitorChangeFunc f)
{
    impl->changeFunc = f;
}

/*!
 * Calls the monitor's value update function. For example a variable
 * monitor's function sets the value of the monitored variable.
 * \note This doesn't work with list monitors.
 */
void Monitor::changeValue(const Value &newValue)
{
    if (impl->changeFunc)
        impl->changeFunc(impl->block.get(), newValue);

    if (impl->iface) {
        impl->changeValueVM.reset();
        impl->changeValueVM.addReturnValue(newValue);
        impl->iface->onValueChanged(&impl->changeValueVM);
    }
}

/*! Returns the monitor's width. */
unsigned int Monitor::width() const
{
    return impl->width;
}

/*! Sets the monitor's width. */
void Monitor::setWidth(unsigned int width)
{
    impl->width = width;
}

/*! Returns the monitor's height. */
unsigned int Monitor::height() const
{
    return impl->height;
}

/*! Sets the monitor's height. */
void Monitor::setHeight(unsigned int height)
{
    impl->height = height;
}

/*! Returns the monitor's x-coordinate. */
int Monitor::x() const
{
    return impl->x;
}

/*! Sets the monitor's x-coordinate. */
void Monitor::setX(int x)
{
    impl->x = x;

    if (impl->iface)
        impl->iface->onXChanged(x);
}

/*! Returns the monitor's y-coordinate. */
int Monitor::y() const
{
    return impl->y;
}

/*! Sets the monitor's y-coordinate. */
void Monitor::setY(int y)
{
    impl->y = y;

    if (impl->iface)
        impl->iface->onYChanged(y);
}

/*! Returns true if the monitor is visible. */
bool Monitor::visible() const
{
    return impl->visible;
}

/*! Sets whether the monitor is visible or not. */
void Monitor::setVisible(bool visible)
{
    impl->visible = visible;

    if (impl->iface)
        impl->iface->onVisibleChanged(visible);
}

/*! Returns the minimum value of the monitor's slider. */
double Monitor::sliderMin() const
{
    return impl->sliderMin;
}

/*! Sets the minimum value of the monitor's slider. */
void Monitor::setSliderMin(double sliderMin)
{
    impl->sliderMin = sliderMin;
}

/*! Returns the maximum value of the monitor's slider. */
double Monitor::sliderMax() const
{
    return impl->sliderMax;
}

/*! Sets the maximum value of the monitor's slider. */
void Monitor::setSliderMax(double sliderMax)
{
    impl->sliderMax = sliderMax;
}

/*! Returns true if the monitor's slider allows only integer values. */
bool Monitor::discrete() const
{
    return impl->discrete;
}

/*! Sets whether the monitor's slider allows only integer values. */
void Monitor::setDiscrete(bool discrete)
{
    impl->discrete = discrete;
}

/*! Returns the initial position of a monitor. */
Rect Monitor::getInitialPosition(const std::vector<std::shared_ptr<Monitor>> &other, int monitorWidth, int monitorHeight)
{
    // TODO: Implement this like Scratch has: https://github.com/scratchfoundation/scratch-gui/blob/010e27937ecff531f23bfcf3c711cd6e565cc7f9/src/reducers/monitor-layout.js#L161-L243
    // Place the monitor randomly
    if (!MonitorPrivate::rng)
        MonitorPrivate::rng = RandomGenerator::instance().get();

    const double randX = std::ceil(MonitorPrivate::rng->randintDouble(0, SCREEN_WIDTH / 2.0));
    const double randY = std::ceil(MonitorPrivate::rng->randintDouble(0, SCREEN_HEIGHT - SCREEN_EDGE_BUFFER));
    return Rect(randX, randY, randX + monitorWidth, randY + monitorHeight);
}
