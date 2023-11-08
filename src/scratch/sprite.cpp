// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sprite.h>
#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <cassert>

#include "sprite_p.h"

using namespace libscratchcpp;

/*! Constructs Sprite. */
Sprite::Sprite() :
    Target(),
    impl(spimpl::make_unique_impl<SpritePrivate>(this))
{
}

/*! Destroys the Sprite object. */
Sprite::~Sprite()
{
    if (isClone()) {
        IEngine *eng = engine();

        if (eng) {
            eng->deinitClone(this);

            auto children = allChildren();
            for (auto child : children)
                eng->deinitClone(child.get());
        }

        assert(impl->cloneParent);
        impl->cloneParent->impl->removeClone(this);
    }
}

/*! Sets the sprite interface. */
void Sprite::setInterface(ISpriteHandler *newInterface)
{
    assert(newInterface);
    impl->iface = newInterface;
    impl->iface->onSpriteChanged(this);
}

/*! Creates a clone of the sprite. */
std::shared_ptr<Sprite> Sprite::clone()
{
    IEngine *eng = engine();

    if (eng) {
        std::shared_ptr<Sprite> clone = std::make_shared<Sprite>();

        if (impl->cloneRoot == nullptr)
            clone->impl->cloneRoot = this;
        else
            clone->impl->cloneRoot = impl->cloneRoot;

        clone->impl->cloneParent = this;
        impl->childClones.push_back(clone);

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
        clone->setEngine(engine());

        clone->impl->visible = impl->visible;
        clone->impl->x = impl->x;
        clone->impl->y = impl->y;
        clone->impl->size = impl->size;
        clone->impl->direction = impl->direction;
        clone->impl->draggable = impl->draggable;
        clone->impl->rotationStyle = impl->rotationStyle;

        // Call "when I start as clone" scripts
        eng->initClone(clone.get());

        if (impl->iface)
            impl->iface->onCloned(clone.get());

        return clone;
    }

    return nullptr;
}

/*! Returns true if this is a clone. */
bool Sprite::isClone() const
{
    return (impl->cloneParent != nullptr);
}

/*! Returns the sprite this clone was created from, or nullptr if this isn't a clone. */
Sprite *Sprite::cloneRoot() const
{
    return impl->cloneRoot;
}

/*! Returns the sprite or clone this clone was created from, or nullptr if this isn't a clone. */
Sprite *Sprite::cloneParent() const
{
    return impl->cloneParent;
}

/*! Returns list of child clones. */
const std::vector<std::shared_ptr<Sprite>> &Sprite::children() const
{
    return impl->childClones;
}

/*! Returns list of child clones and their children (recursive). */
std::vector<std::shared_ptr<Sprite>> Sprite::allChildren() const
{
    std::vector<std::shared_ptr<Sprite>> ret;

    for (auto clone : impl->childClones) {
        ret.push_back(clone);
        auto children = clone->allChildren();
        ret.insert(ret.end(), children.begin(), children.end());
    }

    return ret;
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

/*! Returns the size. */
double Sprite::size() const
{
    return impl->size;
}

/*! Sets the size. */
void Sprite::setSize(double newSize)
{
    impl->size = newSize;
    if (impl->iface)
        impl->iface->onSizeChanged(impl->size);

    auto costume = currentCostume();

    if (costume)
        costume->setScale(newSize / 100);
}

/*! Overrides Target#setCostumeIndex(). */
void Sprite::setCostumeIndex(int newCostumeIndex)
{
    auto costume = costumeAt(newCostumeIndex);

    if (costume) {
        costume->setScale(impl->size / 100);
        costume->setMirrorHorizontally(impl->rotationStyle == RotationStyle::LeftRight);

        for (const auto &[effect, value] : impl->graphicsEffects)
            costume->setGraphicsEffectValue(effect, value);
    }

    Target::setCostumeIndex(newCostumeIndex);
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
    auto costume = currentCostume();

    if (costume)
        costume->setMirrorHorizontally(newRotationStyle == RotationStyle::LeftRight);

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

/*! Returns the bounding rectangle of the sprite. */
Rect Sprite::boundingRect() const
{
    Rect ret;
    impl->getBoundingRect(&ret);

    return ret;
}

/*! Returns the value of the given graphics effect. */
double Sprite::graphicsEffectValue(IGraphicsEffect *effect) const
{
    auto it = impl->graphicsEffects.find(effect);

    if (it == impl->graphicsEffects.cend())
        return 0;
    else
        return it->second;
}

/*! Sets the value of the given graphics effect. */
void Sprite::setGraphicsEffectValue(IGraphicsEffect *effect, double value)
{
    impl->graphicsEffects[effect] = value;

    auto costume = currentCostume();

    if (costume)
        costume->setGraphicsEffectValue(effect, value);
}

/*! Sets the value of all graphics effects to 0 (clears them). */
void Sprite::clearGraphicsEffects()
{
    impl->graphicsEffects.clear();

    auto costume = currentCostume();

    if (costume)
        costume->clearGraphicsEffects();
}

Target *Sprite::dataSource() const
{
    return impl->cloneRoot;
}

void Sprite::setXY(double x, double y)
{
    IEngine *eng = engine();

    if (eng && !eng->spriteFencingEnabled()) {
        impl->x = x;
        impl->y = y;
    } else
        impl->getFencedPosition(x, y, &impl->x, &impl->y);
}
