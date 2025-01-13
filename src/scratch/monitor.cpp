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
 * Returns the extension this monitor belongs to.
 * \note This will return nullptr by default. Add the monitor to a project to set this property.
 */
IExtension *Monitor::extension() const
{
    return impl->extension;
}

/*! Sets the extension this monitor belongs to. */
void Monitor::setExtension(IExtension *extension)
{
    impl->extension = extension;
}

/*! Convenience method which calls block()->target(). */
Sprite *Monitor::sprite() const
{
    Target *target = impl->block->target();

    if (target && !target->isStage())
        return static_cast<Sprite *>(target);

    return nullptr;
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

// TODO: Add updateValue()
/*!
 * Notifies the monitor's interface about value change.
 * The interaface is supposed to read it from the VirtualMachine.
 */
/*void Monitor::updateValue(const VirtualMachine *vm)
{
    if (impl->iface)
        impl->iface->onValueChanged(vm);
}*/

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
        // TODO: Implement this
        // impl->iface->onValueChanged(...);
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
    impl->needsAutoPosition = false;

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
    impl->needsAutoPosition = false;

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

/*! Returns true if the monitor needs auto positioning. The renderer should call autoPosition() as soon as it knows the monitor size. */
bool Monitor::needsAutoPosition() const
{
    return impl->needsAutoPosition;
}

/*!
 * Auto-positions the monitor with the other monitors.
 * \note Call this only when the monitor size is known.
 */
void Monitor::autoPosition(const std::vector<std::shared_ptr<Monitor>> &allMonitors)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/010e27937ecff531f23bfcf3c711cd6e565cc7f9/src/reducers/monitor-layout.js#L161-L243
    if (!impl->needsAutoPosition)
        std::cout << "warning: auto-positioning already positioned monitor (" << impl->name << ")" << std::endl;

    impl->needsAutoPosition = false;

    // Try all starting positions for the new monitor to find one that doesn't intersect others
    std::vector<int> endXs = { 0 };
    std::vector<int> endYs = { 0 };
    int lastX = 0;
    int lastY = 0;
    bool haveLastX = false;
    bool haveLastY = false;

    for (const auto monitor : allMonitors) {
        if (monitor.get() != this) {
            int x = monitor->x() + monitor->width();
            x = std::ceil(x / 50.0) * 50; // Try to choose a sensible "tab width" so more monitors line up
            endXs.push_back(x);
            endYs.push_back(std::ceil(monitor->y() + monitor->height()));
        }
    }

    std::sort(endXs.begin(), endXs.end());
    std::sort(endYs.begin(), endYs.end());

    // We'll use plan B if the monitor doesn't fit anywhere (too long or tall)
    bool planB = false;
    Rect planBRect;

    for (const int x : endXs) {
        if (haveLastX && x == lastX)
            continue;

        lastX = x;
        haveLastX = true;

        for (const int y : endYs) {
            if (haveLastY && y == lastY)
                continue;

            lastY = y;
            haveLastY = true;

            const Rect monitorRect(x + PADDING, y + PADDING, x + PADDING + impl->width, y + PADDING + impl->height);

            // Intersection testing rect that includes padding
            const Rect rect(x, y, x + impl->width + 2 * PADDING, y + impl->height + 2 * PADDING);

            bool intersected = false;

            for (const auto monitor : allMonitors) {
                if (monitor.get() != this) {
                    const Rect currentRect(monitor->x(), monitor->y(), monitor->x() + monitor->width(), monitor->y() + monitor->height());

                    if (monitorRectsIntersect(currentRect, rect)) {
                        intersected = true;
                        break;
                    }
                }
            }

            if (intersected) {
                continue;
            }

            // If the rect overlaps the ends of the screen
            if (rect.right() > SCREEN_WIDTH || rect.bottom() > SCREEN_HEIGHT) {
                // If rect is not too close to completely off-screen, set it as plan B
                if (!planB && !(rect.left() + SCREEN_EDGE_BUFFER > SCREEN_WIDTH || rect.top() + SCREEN_EDGE_BUFFER > SCREEN_HEIGHT)) {
                    planBRect = monitorRect;
                    planB = true;
                }

                continue;
            }

            setX(monitorRect.left());
            setY(monitorRect.top());
            return;
        }
    }

    // If the monitor is too long to fit anywhere, put it in the leftmost spot available
    // that intersects the right or bottom edge and isn't too close to the edge.
    if (planB) {
        setX(planBRect.left());
        setY(planBRect.top());
        return;
    }

    // If plan B fails and there's nowhere reasonable to put it, plan C is to place the monitor randomly
    if (!MonitorPrivate::rng)
        MonitorPrivate::rng = RandomGenerator::instance().get();

    const int randX = std::ceil(MonitorPrivate::rng->randintDouble(0, SCREEN_WIDTH / 2.0));
    const int randY = std::ceil(MonitorPrivate::rng->randintDouble(0, SCREEN_HEIGHT - SCREEN_EDGE_BUFFER));
    setX(randX);
    setY(randY);
    return;
}

bool Monitor::monitorRectsIntersect(const Rect &a, const Rect &b)
{
    // https://github.com/scratchfoundation/scratch-gui/blob/010e27937ecff531f23bfcf3c711cd6e565cc7f9/src/reducers/monitor-layout.js#L152-L158
    // If one rectangle is on left side of other
    if (a.left() >= b.right() || b.left() >= a.right())
        return false;

    // If one rectangle is above other
    if (a.top() >= b.bottom() || b.top() >= a.bottom())
        return false;

    return true;
}
