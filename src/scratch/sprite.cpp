// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sprite.h>
#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <cassert>
#include <iostream>

#include "sprite_p.h"

using namespace libscratchcpp;

/*! Constructs Sprite. */
Sprite::Sprite() :
    Target(),
    impl(spimpl::make_unique_impl<SpritePrivate>(this))
{
}

/*! Sets the sprite interface. */
void Sprite::setInterface(ISpriteHandler *newInterface)
{
    assert(newInterface);
    impl->iface = newInterface;
    impl->iface->init(this);
}

/*! Returns the sprite interface. */
ISpriteHandler *Sprite::getInterface() const
{
    return impl->iface;
}

/*! Creates a clone of the sprite. */
std::shared_ptr<Sprite> Sprite::clone()
{
    IEngine *eng = engine();

    if (eng && (eng->cloneLimit() == -1 || eng->cloneCount() < eng->cloneLimit())) {
        std::shared_ptr<Sprite> clone = std::make_shared<Sprite>();

        if (impl->cloneSprite == nullptr) {
            clone->impl->cloneSprite = this;
            impl->clones.push_back(clone);
        } else {
            clone->impl->cloneSprite = impl->cloneSprite;
            impl->cloneSprite->impl->clones.push_back(clone);
        }

        // Copy data
        clone->setName(name());

        const auto &vars = variables();

        for (auto var : vars)
            clone->addVariable(var->clone());

        const auto &l = lists();

        for (auto list : l)
            clone->addList(list->clone());

        clone->setCostumeIndex(costumeIndex());
        clone->setLayerOrder(layerOrder());
        clone->setVolume(volume());

        clone->impl->visible = impl->visible;
        clone->impl->x = impl->x;
        clone->impl->y = impl->y;
        clone->impl->size = impl->size;
        clone->impl->direction = impl->direction;
        clone->impl->draggable = impl->draggable;
        clone->impl->rotationStyle = impl->rotationStyle;

        clone->setEngine(engine());

        // Call "when I start as clone" scripts
        eng->initClone(clone);

        if (impl->visible)
            eng->requestRedraw();

        if (impl->iface)
            impl->iface->onCloned(clone.get());

        // Place the clone behind the original sprite
        eng->moveSpriteBehindOther(clone.get(), this);

        return clone;
    }

    return nullptr;
}

/*! Deletes this clone (if the sprite is a clone). */
void Sprite::deleteClone()
{
    assert(isClone());

    if (isClone()) {
        IEngine *eng = engine();

        if (eng)
            eng->deinitClone(shared_from_this());

        if (impl->iface)
            impl->iface->deinitClone();

        assert(impl->cloneSprite);
        impl->cloneDeleted = true;
        impl->cloneSprite->impl->removeClone(this);
    }
}

/*! Returns true if this is a clone. */
bool Sprite::isClone() const
{
    return (impl->cloneSprite != nullptr);
}

/*! Returns the sprite this clone was created from, or nullptr if this isn't a clone. */
Sprite *Sprite::cloneSprite() const
{
    return impl->cloneSprite;
}

/*! Returns list of clones of the sprite. */
const std::vector<std::shared_ptr<Sprite>> &Sprite::clones() const
{
    if (isClone()) {
        assert(impl->cloneSprite);
        return impl->cloneSprite->impl->clones;
    } else
        return impl->clones;
}

/*! Returns true if the sprite is visible. */
bool Sprite::visible() const
{
    return impl->visible;
}

/*! Sets the visibility of the sprite. */
void Sprite::setVisible(bool newVisible)
{
    impl->visible = newVisible;

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onVisibleChanged(impl->visible);
}

/*! Returns the X position of the sprite. */
double Sprite::x() const
{
    return impl->x;
}

/*! Sets the X position of the sprite. */
void Sprite::setX(double newX)
{
    setXY(newX, impl->y);

    if (impl->iface)
        impl->iface->onXChanged(impl->x);
}

/*! Returns the Y position of the sprite. */
double Sprite::y() const
{
    return impl->y;
}

/*! Sets the Y position of the sprite. */
void Sprite::setY(double newY)
{
    setXY(impl->x, newY);

    if (impl->iface)
        impl->iface->onYChanged(impl->y);
}

/* Sets the position of the sprite. */
void Sprite::setPosition(double x, double y)
{
    setXY(x, y);

    if (impl->iface) {
        impl->iface->onXChanged(impl->x);
        impl->iface->onYChanged(impl->y);
    }
}

/*! Returns the size. */
double Sprite::size() const
{
    return impl->size;
}

/*! Sets the size. */
void Sprite::setSize(double newSize)
{
    impl->size = newSize;

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onSizeChanged(impl->size);
}

/*! Overrides Target#setCostumeIndex(). */
void Sprite::setCostumeIndex(int newCostumeIndex)
{
    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    Target::setCostumeIndex(newCostumeIndex);
    auto costume = costumeAt(newCostumeIndex);

    if (costume && impl->iface)
        impl->iface->onCostumeChanged(costume.get());
}

/*! Returns the direction. */
double Sprite::direction() const
{
    return impl->direction;
}

/*! Sets the direction. */
void Sprite::setDirection(double newDirection)
{
    if (newDirection >= -180 && newDirection <= 180)
        impl->direction = newDirection;
    else if (newDirection < -180)
        impl->direction = std::fmod(newDirection - 180, 360) + 180;
    else
        impl->direction = std::fmod(newDirection + 180, 360) - 180;

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onDirectionChanged(impl->direction);
}

/*! Returns true if the sprite is draggable. */
bool Sprite::draggable() const
{
    return impl->draggable;
}

/*! Toggles whether the sprite is draggable. */
void Sprite::setDraggable(bool newDraggable)
{
    impl->draggable = newDraggable;
}

/*! Returns the rotation style. */
Sprite::RotationStyle Sprite::rotationStyle() const
{
    return impl->rotationStyle;
}

/*! Returns the rotation style as a string. */
std::string Sprite::rotationStyleStr() const
{
    switch (impl->rotationStyle) {
        case RotationStyle::AllAround:
            return "all around";
        case RotationStyle::LeftRight:
            return "left-right";
        case RotationStyle::DoNotRotate:
            return "don't rotate";
    }
    return "all around";
}

/*! Sets the rotation style. */
void Sprite::setRotationStyle(RotationStyle newRotationStyle)
{
    impl->rotationStyle = newRotationStyle;

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onRotationStyleChanged(impl->rotationStyle);
}

/*! \copydoc setRotationStyle() */
void Sprite::setRotationStyle(const std::string &newRotationStyle)
{
    if (newRotationStyle == "all around")
        setRotationStyle(RotationStyle::AllAround);
    else if (newRotationStyle == "left-right")
        setRotationStyle(RotationStyle::LeftRight);
    else if (newRotationStyle == "don't rotate")
        setRotationStyle(RotationStyle::DoNotRotate);
}

/*! \copydoc setRotationStyle() */
void Sprite::setRotationStyle(const char *newRotationStyle)
{
    setRotationStyle(std::string(newRotationStyle));
}

/*! Overrides Target#setLayerOrder(). */
void Sprite::setLayerOrder(int newLayerOrder)
{
    Target::setLayerOrder(newLayerOrder);

    if (impl->iface)
        impl->iface->onLayerOrderChanged(newLayerOrder);
}

/*! Returns the bounding rectangle of the sprite. */
Rect Sprite::boundingRect() const
{
    if (!impl->iface)
        return Rect(impl->x, impl->y, impl->x, impl->y);

    return impl->iface->boundingRect();
}

/*!
 * Keeps the desired position within the stage.
 * \param[in] New desired X position.
 * \param[in] New desired Y position.
 * \param[out] Fenced X position.
 * \param[out] Fenced Y position.
 */
void Sprite::keepInFence(double newX, double newY, double *fencedX, double *fencedY) const
{
    // See https://github.com/scratchfoundation/scratch-vm/blob/05dcbc176f51da34aeb9165559fc6acba8087ff8/src/sprites/rendered-target.js#L915-L948
    IEngine *eng = engine();

    if (!(fencedX && fencedY && eng))
        return;

    double stageWidth = eng->stageWidth();
    double stageHeight = eng->stageHeight();
    Rect fence(-stageWidth / 2, stageHeight / 2, stageWidth / 2, -stageHeight / 2);
    Rect bounds = boundingRect();

    // Adjust the known bounds to the target position
    bounds.setLeft(bounds.left() + newX - impl->x);
    bounds.setRight(bounds.right() + newX - impl->x);
    bounds.setTop(bounds.top() + newY - impl->y);
    bounds.setBottom(bounds.bottom() + newY - impl->y);

    // Find how far we need to move the target position
    double dx = 0;
    double dy = 0;

    if (bounds.left() < fence.left()) {
        dx += fence.left() - bounds.left();
    }
    if (bounds.right() > fence.right()) {
        dx += fence.right() - bounds.right();
    }
    if (bounds.top() > fence.top()) {
        dy += fence.top() - bounds.top();
    }
    if (bounds.bottom() < fence.bottom()) {
        dy += fence.bottom() - bounds.bottom();
    }

    *fencedX = newX + dx;
    *fencedY = newY + dy;
}

/*! Overrides Target#setGraphicsEffectValue(). */
void Sprite::setGraphicsEffectValue(IGraphicsEffect *effect, double value)
{
    Target::setGraphicsEffectValue(effect, value);

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onGraphicsEffectChanged(effect, value);
}

/*! Overrides Target#clearGraphicsEffects(). */
void Sprite::clearGraphicsEffects()
{
    Target::clearGraphicsEffects();

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onGraphicsEffectsCleared();
}

/*! Overrides Target#setBubbleType(). */
void Sprite::setBubbleType(BubbleType type)
{
    Target::setBubbleType(type);

    if (impl->iface)
        impl->iface->onBubbleTypeChanged(type);
}

/*! Overrides Target#setBubbleText(). */
void Sprite::setBubbleText(const std::string &text)
{
    Target::setBubbleText(text);

    if (impl->visible && !text.empty()) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onBubbleTextChanged(text);
}

Target *Sprite::dataSource() const
{
    return impl->cloneSprite;
}

void Sprite::setXY(double x, double y)
{
    IEngine *eng = engine();

    double oldX = impl->x;
    double oldY = impl->y;

    if (eng && !eng->spriteFencingEnabled()) {
        impl->x = x;
        impl->y = y;
    } else
        impl->getFencedPosition(x, y, &impl->x, &impl->y);

    if (impl->visible) {
        IEngine *eng = engine();

        if (eng)
            eng->requestRedraw();
    }

    if (impl->iface)
        impl->iface->onMoved(oldX, oldY, impl->x, impl->y);
}
